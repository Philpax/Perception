/********************************************************************
Vireio Perception: Open-Source Stereoscopic 3D Driver
Copyright (C) 2012 Andres Hernandez

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "D3DProxyDevice.h"
#include "StereoViewFactory.h"
#include "MotionTrackerFactory.h"

#include <sstream>							// for stringstream

#pragma comment(lib, "d3dx9.lib")

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define PI 3.141592654
#define RADIANS_TO_DEGREES(rad) ((float) rad * (float) (180.0 / PI))

D3DProxyDevice::D3DProxyDevice(IDirect3DDevice9* pDevice):BaseDirect3DDevice9(pDevice)
{
	num_d3d++;
	OutputDebugString("D3D ProxyDev Created\n");
	hudFont = NULL;
	centerlineR = 0.0f;
	centerlineL = 0.0f;
	lastInputTime = clock();
}

D3DProxyDevice::~D3DProxyDevice()
{
	if(hudFont) {
		hudFont->Release();
		hudFont = NULL;
	}
}

void D3DProxyDevice::Init(ProxyHelper::ProxyConfig& cfg)
{
	OutputDebugString("D3D ProxyDev Init\n");
	yaw_mode = 0;
	pitch_mode = 0;
	roll_mode = 0;
	translation_mode = 0;
	trackingOn = true;
	SHOCT_mode = 0;

	stereoView = StereoViewFactory::Get(cfg);
	SetupOptions(cfg);
	SetupMatrices();
	SetupText();
}

HRESULT WINAPI D3DProxyDevice::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	if(stereoView != NULL)
		stereoView->Reset();
	if(hudFont) {
		hudFont->Release();
		hudFont = NULL;
	}

	return BaseDirect3DDevice9::Reset(pPresentationParameters);
}

void D3DProxyDevice::SetupOptions(ProxyHelper::ProxyConfig& cfg)
{
	separation = cfg.separation;
	convergence = cfg.convergence;
	game_type = cfg.game_type;
	aspect_multiplier = cfg.aspect_multiplier;
	yaw_multiplier = cfg.yaw_multiplier;
	pitch_multiplier = cfg.pitch_multiplier;
	roll_multiplier = cfg.roll_multiplier;
	config = cfg;
	eyeShutter = 1;
	matrixIndex = 0;
	offset = 0.0f;
	swap_eyes = cfg.swap_eyes;
	stereoView->SwapEyes(swap_eyes);// why are there two swap_eyes
	centerlineL = cfg.centerlineL;
	centerlineR = cfg.centerlineR;
	keybinds.LoadXML();

	saveDebugFile = false;
	trackerInitialized = false;

	if(yaw_multiplier == 0.0f) yaw_multiplier = 25.0f;
	if(pitch_multiplier == 0.0f) pitch_multiplier = 25.0f;
	if(roll_multiplier == 0.0f) roll_multiplier = 1.0f;

	char buf[64];
	LPCSTR psz = NULL;

	sprintf_s(buf, "type: %d, aspect: %f", cfg.game_type, cfg.aspect_multiplier);
	psz = buf;
	OutputDebugString(psz);
	OutputDebugString("\n");
}

void D3DProxyDevice::SetupMatrices()
{	// call this function after StereoView::Init()

	aspectRatio = (float)stereoView->viewport.Width/(float)stereoView->viewport.Height;

	n = 0.1f;					
	f = 10.0f;
	l = -0.5f;
	r = 0.5f;
	t = 0.5f / aspectRatio;
	b = -0.5f / aspectRatio;

	D3DXMatrixPerspectiveOffCenterLH(&matProjection, l, r, b, t, n, f);
	D3DXMatrixInverse(&matProjectionInv, 0, &matProjection);
}

void D3DProxyDevice::ComputeViewTranslation()
{
	D3DXMATRIX transform;
	D3DXMatrixTranslation(&transform, separation*eyeShutter*10.0f+offset*10.0f, 0, 0);

	float adjustedFrustumOffset = convergence*eyeShutter*0.1f*separation;		
	D3DXMATRIX reProject;
	D3DXMatrixPerspectiveOffCenterLH(&reProject, l, r, b, t, n, f);
	D3DXMatrixInverse(&matProjectionInv, 0, &reProject);
	D3DXMatrixPerspectiveOffCenterLH(&reProject, l+adjustedFrustumOffset, r+adjustedFrustumOffset, b, t, n, f);

	if(trackerInitialized && tracker->isAvailable() && roll_mode != 0)
	{
		D3DXMATRIX rollMatrix;
		D3DXMatrixRotationZ(&rollMatrix, tracker->currentRoll);
		D3DXMatrixMultiply(&transform, &rollMatrix, &transform);
	}

	matViewTranslation = matProjectionInv * transform * reProject;
}


void D3DProxyDevice::SetupText()
{
	D3DXCreateFont( m_pDevice, 22, 0, FW_BOLD, 4, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &hudFont );
}

void D3DProxyDevice::HandleControls()
{
	clock_t curTime;
	curTime = clock();

	elapsedTime = (double(curTime - lastInputTime)) / ((double)CLOCKS_PER_SEC);
	lastInputTime = curTime;

	float keySpeed = float(0.00002f * 2500.0f * elapsedTime);
	float keySpeed2 = float(0.00002f * 6000.0f * elapsedTime);
	float mouseSpeed = float(0.25f * 60.0f * elapsedTime);
	float rollSpeed = float(0.01f * 60.0f * elapsedTime);

	static float keyWaitCount = 0; 
	keyWaitCount -= float(elapsedTime);
	static float saveWaitCount = 0; 
	saveWaitCount -= float(elapsedTime);
	static bool doSaveNext = false;

	if(keyWaitCount < 0)
		keyWaitCount = 0.0f;
	if(saveWaitCount < 0)
		saveWaitCount = 0.0f;

	if(keybinds.CommandPressed(KeyBindings::STEREO_ENABLED_T))		// turn on/off stereo3D
	{
		if(keyWaitCount <= 0)
		{
			if(stereoView->stereoEnabled)
				stereoView->stereoEnabled = false;
			else
				stereoView->stereoEnabled = true;
			keyWaitCount = 0.25f;
		}
	}

//////////  SHOCT
	if(keybinds.CommandPressed(KeyBindings::SHOCT_L_DEC))
	{
		centerlineL  -= keySpeed2;
		saveWaitCount = 5;
		doSaveNext = true;
	}
	if(keybinds.CommandPressed(KeyBindings::SHOCT_L_INC))
	{
		centerlineL  += keySpeed2;
		saveWaitCount = 5;
		doSaveNext = true;
	}

	if(keybinds.CommandPressed(KeyBindings::SHOCT_R_DEC))
	{
		centerlineR  -= keySpeed2;
		saveWaitCount = 5;
		doSaveNext = true;
	}
	if(keybinds.CommandPressed(KeyBindings::SHOCT_R_INC))
	{
		centerlineR  += keySpeed2;
		saveWaitCount = 5;
		doSaveNext = true;
	}

	if(keybinds.CommandPressed(KeyBindings::SHOCT_MODE_T))
	{
		if(keyWaitCount <= 0)
		{
			SHOCT_mode++;
			SHOCT_mode %= 3;
			if(SHOCT_mode == 0){//off
				trackingOn = true;
			}
			if(SHOCT_mode == 1){// seperation
				trackingOn = false;
			}
			if(SHOCT_mode == 2){// convergence
				trackingOn = false;
			}
			keyWaitCount = 0.25f;
		}
	}
//////////
	if(keybinds.CommandPressed(KeyBindings::SCREEN_OFFSET_INC))
	{
		stereoView->LensShift[0] += keySpeed;
	}
	if(keybinds.CommandPressed(KeyBindings::SCREEN_OFFSET_DEC))
	{
		stereoView->LensShift[0] -= keySpeed;
	}

	if(keybinds.CommandPressed(KeyBindings::SAVE_SCREEN))
	{
		if(stereoView->initialized)
			stereoView->SaveScreen();
	}

	if(keybinds.CommandPressed(KeyBindings::OFFSET_INC))
	{
		offset += keySpeed;
		saveWaitCount = 5;
		doSaveNext = true;
	}
	if(keybinds.CommandPressed(KeyBindings::OFFSET_DEC))
	{
		offset -= keySpeed;
		saveWaitCount = 5;
		doSaveNext = true;
	}

	if(keybinds.CommandPressed(KeyBindings::SEPARATION_INC))
	{
		separation += keySpeed * 0.2f;
		saveWaitCount = 5;
		doSaveNext = true;
	}
	if(keybinds.CommandPressed(KeyBindings::SEPARATION_DEC))
	{
		separation -= keySpeed * 0.2f;
		if(separation < 0)		// no negative seperation
			separation = 0;
		saveWaitCount = 5;
		doSaveNext = true;
	}

	if(keybinds.CommandPressed(KeyBindings::DISTORTIONSCALE_INC))
	{
		stereoView->DistortionScale  += keySpeed*10;
		saveWaitCount = 5;
		doSaveNext = true;
	}

	if(keybinds.CommandPressed(KeyBindings::DISTORTIONSCALE_DEC))
	{
		stereoView->DistortionScale  -= keySpeed*10;
		saveWaitCount = 5;
		doSaveNext = true;
	}


	if(keybinds.CommandPressed(KeyBindings::CONVERGENCE_INC))
	{
		convergence += keySpeed2*10;
		saveWaitCount = 5;
		doSaveNext = true;
	}
	if(keybinds.CommandPressed(KeyBindings::CONVERGENCE_DEC))
	{
		convergence -= keySpeed2*10;
		saveWaitCount = 5;
		doSaveNext = true;
	}

	if(keybinds.CommandPressed(KeyBindings::SWAP_EYES))
	{
		if(keyWaitCount <= 0)
		{
			swap_eyes = !swap_eyes;
			stereoView->SwapEyes(swap_eyes);
			keyWaitCount = 0.25f;
			saveWaitCount = 5;
			doSaveNext = true;
		}
	}

	if(keybinds.CommandPressed(KeyBindings::DEFAULT_VALUES))
	{
		separation = 0.0f;
		convergence = 0.0f;
		offset = 0.0f;
		yaw_multiplier = 25.0f;
		pitch_multiplier = 25.0f;
		roll_multiplier = 1.0f;
		//matrixIndex = 0;
		saveWaitCount = 5;
		doSaveNext = true;
	}
	
//	if(KEY_DOWN(VK_F7) && keyWaitCount <= 0)
//	{
//		matrixIndex++;
//		if(matrixIndex > 15) 
//		{
//			matrixIndex = 0;
//		}
//		keyWaitCount = 200;
//	}

	if(keybinds.CommandPressed(KeyBindings::PITCH_MUL_INC))
	{
		pitch_multiplier += mouseSpeed;
		saveWaitCount = 5;
		doSaveNext = true;
		if(trackerInitialized && tracker->isAvailable())
			tracker->setMultipliers(yaw_multiplier, pitch_multiplier, roll_multiplier);
	}
	if(keybinds.CommandPressed(KeyBindings::PITCH_MUL_DEC))
	{
		pitch_multiplier -= mouseSpeed;
		saveWaitCount = 5;
		doSaveNext = true;
		if(trackerInitialized && tracker->isAvailable())
			tracker->setMultipliers(yaw_multiplier, pitch_multiplier, roll_multiplier);
	}

	if(keybinds.CommandPressed(KeyBindings::YAW_MUL_INC))
	{
		yaw_multiplier += mouseSpeed;
		saveWaitCount = 5;
		doSaveNext = true;
		if(trackerInitialized && tracker->isAvailable())
			tracker->setMultipliers(yaw_multiplier, pitch_multiplier, roll_multiplier);
	}
	if(keybinds.CommandPressed(KeyBindings::YAW_MUL_DEC))
	{
		yaw_multiplier -= mouseSpeed;
		saveWaitCount = 5;
		doSaveNext = true;
		if(trackerInitialized && tracker->isAvailable())
			tracker->setMultipliers(yaw_multiplier, pitch_multiplier, roll_multiplier);
	}

	if(keybinds.CommandPressed(KeyBindings::ROLL_MUL_INC))
	{
		roll_multiplier += mouseSpeed;
		saveWaitCount = 5;
		doSaveNext = true;
		if(trackerInitialized && tracker->isAvailable())
			tracker->setMultipliers(yaw_multiplier, pitch_multiplier, roll_multiplier);
	}
	if(keybinds.CommandPressed(KeyBindings::ROLL_MUL_DEC))
	{
		roll_multiplier -= mouseSpeed;
		saveWaitCount = 5;
		doSaveNext = true;
		if(trackerInitialized && tracker->isAvailable())
			tracker->setMultipliers(yaw_multiplier, pitch_multiplier, roll_multiplier);
	}

	if(saveDebugFile)
	{
		debugFile.close();
	}
	saveDebugFile = false;

//	if(KEY_DOWN(VK_F12) && keyWaitCount <= 0)
//	{
//		// uncomment to save text debug file
//		//saveDebugFile = true;
//		keyWaitCount = 200;
//	}

	if(doSaveNext && saveWaitCount <= 0)
	{
		doSaveNext = false;
		ProxyHelper* helper = new ProxyHelper();
		helper->SaveProfile(separation, convergence, swap_eyes, yaw_multiplier, pitch_multiplier, roll_multiplier);
		helper->SaveUserConfig(centerlineL, centerlineR);
	}

}

void D3DProxyDevice::HandleTracking()
{
	if(!trackingOn){
		tracker->currentRoll = 0;
		return;
	}
	if(!trackerInitialized)
	{
		OutputDebugString("Try to init Tracker\n");
		tracker = MotionTrackerFactory::Get(config);
		trackerInitialized = true;
	}

	if(trackerInitialized && tracker->isAvailable())
	{
		tracker->updateOrientation();
	}
}

bool D3DProxyDevice::validRegister(UINT reg)
{
	return true;
}

void ClearVLine(LPDIRECT3DDEVICE9 Device_Interface,int x1,int y1,int x2,int y2,int bw,D3DCOLOR Color)
{
	D3DRECT rec;
	rec.x1 = x1-bw;
	rec.y1 = y1;
	rec.x2 = x2+bw;
	rec.y2 = y2;

	Device_Interface->Clear(1,&rec,D3DCLEAR_TARGET,Color,0,0);
}

void ClearHLine(LPDIRECT3DDEVICE9 Device_Interface,int x1,int y1,int x2,int y2,int bw,D3DCOLOR Color)
{
	D3DRECT rec;
	rec.x1 = x1;
	rec.y1 = y1-bw;
	rec.x2 = x2;
	rec.y2 = y2+bw;

	Device_Interface->Clear(1,&rec,D3DCLEAR_TARGET,Color,0,0);
}

HRESULT WINAPI D3DProxyDevice::EndScene()
{
///// hud text
	if(hudFont == NULL)
		SetupText();

//	RECT rec2 = {255,10,800,800};
//	std::stringstream ss;
//	ss << elapsedTime << "asd:" << num_d3d;
//	hudFont->DrawText(NULL, ss.str().c_str(), -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));

	if(hudFont && SHOCT_mode !=0) {
		char vcString[512];
		int width = stereoView->viewport.Width;
		int height = stereoView->viewport.Height;

		float horWidth = 0.15f;
		int beg = (int)(width*(1.0f-horWidth)/2.0);
		int end = (int)(width*(0.5f+(horWidth/2.0f)));

		int hashTop = (int)(height * 0.48f);
		int hashBottom = (int)(height * 0.52f);

		RECT rec2 = {(int)(width*0.27f), (int)(height*0.3f),width,height};
		sprintf_s(vcString, 512, "Schneider-Hicks Optical Calibration Tool (S.H.O.C.T.).\n");
		hudFont->DrawText(NULL, vcString, -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));


		if(SHOCT_mode == 1){//Separation mode
			if((eyeShutter > 0 && stereoView->swap_eyes == false) || (eyeShutter < 0 && stereoView->swap_eyes == true)) {// left eye
				//eye center line
				ClearVLine(this, (int)(width/2 + (centerlineL * width)), 0, (int)(width/2 + (centerlineL * width)), height, 1, D3DCOLOR_ARGB(255,255,0,0));
			}else{// right eye
				ClearVLine(this, (int)(width/2 + (centerlineR * width)), 0, (int)(width/2 + (centerlineR * width)), height, 1, D3DCOLOR_ARGB(255,255,0,0));
			}
		}
		if(SHOCT_mode == 2){//Convergence mode
			//screen center line
			ClearVLine(this,width/2,0,width/2, height,1,D3DCOLOR_ARGB(255,0,0,255));
			if((eyeShutter > 0 && stereoView->swap_eyes == false) || (eyeShutter < 0 && stereoView->swap_eyes == true)) {// left eye
				// horizontal line
				ClearHLine(this,beg,(height/2),end, (height/2),1,D3DCOLOR_ARGB(255,0,0,255));

				// hash lines
				int hashNum = 10;
				float hashSpace = horWidth*width / (float)hashNum;
				for(int i=0; i<=hashNum; i++) {
					ClearVLine(this,beg+(int)(i*hashSpace),hashTop,beg+(int)(i*hashSpace),hashBottom,1,D3DCOLOR_ARGB(255,255,255,0));
				}

				RECT rec2 = {(int)(width*0.37f), (int)(height*0.525f), width, height};
				sprintf_s(vcString, 512, "Positive Parallax");
				hudFont->DrawText(NULL, vcString, -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));

				rec2.left = (int)(width *0.52f);
				sprintf_s(vcString, 512, "Negative Parallax");
				hudFont->DrawText(NULL, vcString, -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));

			}else{// right eye
				RECT rec2 = {(int)(width*0.37f), (int)(height*0.44f), width, height};
				sprintf_s(vcString, 512, "Walk up as close as possible to a 90 degree\n vertical object, and align this line with its edge.\n Good examples include a wall, corner, a table corner,\n a squared post, etc.");
				hudFont->DrawText(NULL, vcString, -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));
			}
		}

		rec2.left = (int)(width*0.35f);
		rec2.top = (int)(height*0.33f);
		if(SHOCT_mode == 1)
			sprintf_s(vcString, 512, "Separation");
		if(SHOCT_mode == 2)
			sprintf_s(vcString, 512, "Convergence");
		hudFont->DrawText(NULL, vcString, -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));

	}
/////
	return BaseDirect3DDevice9::EndScene();
}

