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
#include <iostream>
#include <sstream>							// for stringstream
/////
//	std::stringstream ss;
//	ss << a;
//	char out[1000];
//	strcpy(out,ss.str().c_str());
//	MessageBox(NULL,out,"",MB_OK);
/////

#include "D3DProxyDeviceTest.h"

D3DProxyDeviceTest::D3DProxyDeviceTest(IDirect3DDevice9* pDevice):D3DProxyDevice(pDevice)
{
	menueDisplay = false;
	menueMethod = 0;
	transformMethod = 0;
	vertexShaderConstantFMethod = 0;
	menueMethodItem = 0;

	for(int i =0; i < sizeof(registersUsed); i++) {
		registersUsed[i] = false;
	}

	for(int i =0; i < sizeof(validRegisters); i++) {
		validRegisters[i] = false;
	}
	validRegisters[0] = true;
	validRegisters[4] = true;
	validRegisters[16] = true;

	pCurrentRenderTarget = NULL;
	targets = 0;

}

D3DProxyDeviceTest::~D3DProxyDeviceTest()
{
}

void D3DProxyDeviceTest::Init(ProxyHelper::ProxyConfig& cfg)
{
	OutputDebugString("D3D ProxyDev Test Init\n");
	D3DProxyDevice::Init(cfg);
	roll_mode = 1;

	//matrixIndex = getMatrixIndex();
}

HRESULT WINAPI D3DProxyDeviceTest::BeginScene()
{

	HandleControls();
	HandleTracking();
	ComputeViewTranslation();

	if(saveDebugFile)
	{
		debugFile.open("d3d9_debug.txt", std::ios::out);
	}

	return D3DProxyDevice::BeginScene();
}

HRESULT WINAPI D3DProxyDeviceTest::EndScene()
{
//	D3DRECT rec = {1,1,25,25};
//	m_pDevice->Clear(1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(225,255,255,0),0 ,0);

	// delay to avoid crashing on start
	static int initDelay = 120;
	initDelay--;

	if(!stereoView->initialized && initDelay < 0)
	{
		stereoView->Init(m_pDevice);
		SetupMatrices();
	}

//	RECT rec2 = {30,10,400,200};
//	char vcString[512];
//	sprintf_s(vcString, 512, "mat index: %d\n",
//		matrixIndex
//	);
	//hudFont->DrawText(NULL, vcString, -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));
	if(menueDisplay)
		DisplayMethodsMenu();

	return D3DProxyDevice::EndScene();
}

void D3DProxyDeviceTest::DisplayMethodsMenu()
{
	D3DRECT rec = {250,0,800,800};
	m_pDevice->Clear(1, &rec, D3DCLEAR_TARGET, D3DCOLOR_ARGB(225,0,0,0),0 ,0);

	RECT rec2 = {255,10,800,800};

	std::stringstream ss;
	ss << "Target changes: " << targets << "\n";
	targets = 0;
	ss << "\n\nValidRegisters> " << menueMethodItem << ", " << validRegisters[menueMethodItem] << "\n";


	ss << "transformMethod:" << transformMethod << "\n";


	ss << "registersUsed :\n";

	for(int i =0; i < sizeof(registersUsed); i++) {
		if(registersUsed[i])
			ss << "\t\t" << i << ":" << registersUsed[i] << "\n";
	}

//	char out[2048];
//	strcpy(out,ss.str().c_str());
//	MessageBox(NULL,out,"D3DProxyDeviceAdv",MB_OK);
/////

//	sprintf_s(vcString, 512, "registerUsed: \n");

//	sprintf_s(vcString, 512, "registerUsed: \n");
	if(hudFont == NULL)
		D3DXCreateFont( m_pDevice, 20, 0, FW_BOLD, 4, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &hudFont );
	if(hudFont)
		hudFont->DrawText(NULL, ss.str().c_str(), -1, &rec2, 0, D3DCOLOR_ARGB(255,255,255,255));

}

HRESULT WINAPI D3DProxyDeviceTest::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	if(stereoView->initialized && stereoView->stereoEnabled)
	{
		if(eyeShutter > 0)
		{
			stereoView->UpdateEye(StereoView::LEFT_EYE);
		}
		else 
		{
			stereoView->UpdateEye(StereoView::RIGHT_EYE);
		}

		stereoView->Draw();

		eyeShutter *= -1;
	}

	if(eyeShutter > 0) return D3D_OK;
	HRESULT hr = D3DProxyDevice::Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);

	return hr;
}



// Method 1 Set Transform
HRESULT WINAPI D3DProxyDeviceTest::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	/*
	State values
	D3DTS_VIEW         = 2,
	D3DTS_PROJECTION   = 3,
	D3DTS_TEXTURE0     = 16,
	D3DTS_TEXTURE1     = 17,
	D3DTS_TEXTURE2     = 18,
	D3DTS_TEXTURE3     = 19,
	D3DTS_TEXTURE4     = 20,
	D3DTS_TEXTURE5     = 21,
	D3DTS_TEXTURE6     = 22,
	D3DTS_TEXTURE7     = 23,
	D3DTS_FORCE_DWORD  = 0x7fffffff
	*/

	if(transformMethod == 0)
		return D3DProxyDevice::SetTransform(State, pMatrix);

	if(State == D3DTS_VIEW && (transformMethod == 1 || transformMethod == 3))
	{
		D3DXMATRIX sourceMatrix(*pMatrix);

		D3DXMATRIX transMatrix;
		D3DXMatrixIdentity(&transMatrix);

		if(trackerInitialized && tracker->isAvailable())
		{
			D3DXMATRIX rollMatrix;
			D3DXMatrixRotationZ(&rollMatrix, tracker->currentRoll);
			D3DXMatrixMultiply(&sourceMatrix, &sourceMatrix, &rollMatrix);
		}

		D3DXMatrixTranslation(&transMatrix, (separation*eyeShutter+offset)*6000.0f, 0, 0);
		D3DXMatrixMultiply(&sourceMatrix, &sourceMatrix, &transMatrix);

		return D3DProxyDevice::SetTransform(State, &sourceMatrix);
	}
	else if(State == D3DTS_PROJECTION && (transformMethod == 2 || transformMethod == 3))
	{
		D3DXMATRIX sourceMatrix(*pMatrix);

		D3DXMATRIX transMatrix;
		D3DXMatrixIdentity(&transMatrix);

		D3DXMatrixMultiply(&sourceMatrix, &sourceMatrix, &matProjectionInv);

		transMatrix[8] += convergence*eyeShutter*0.0075f;
		D3DXMatrixMultiply(&sourceMatrix, &sourceMatrix, &transMatrix);

		D3DXMatrixMultiply(&sourceMatrix, &sourceMatrix, &matProjection);

		return D3DProxyDevice::SetTransform(State, &sourceMatrix);
	}

	return D3DProxyDevice::SetTransform(State, pMatrix);
}





//testing
HRESULT WINAPI D3DProxyDeviceTest::SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget)
{
	pCurrentRenderTarget = pRenderTarget;
	targets++;
	return BaseDirect3DDevice9::SetRenderTarget(RenderTargetIndex, pRenderTarget);
}
HRESULT WINAPI D3DProxyDeviceTest::SetClipPlane(DWORD Index,CONST float* pPlane)
{
//	targets++;
	return BaseDirect3DDevice9::SetClipPlane(Index, pPlane);
}
HRESULT WINAPI D3DProxyDeviceTest::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
//	targets++;
	return BaseDirect3DDevice9::SetViewport(pViewport);
}
HRESULT WINAPI D3DProxyDeviceTest::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain)
{
	targets++;
	return BaseDirect3DDevice9::CreateAdditionalSwapChain(pPresentationParameters,pSwapChain);
}

//SetRenderState
//SetScissorRect
//SetStreamSource

//UpdateSurface











HRESULT WINAPI D3DProxyDeviceTest::SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
{
//return D3DProxyDevice::SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);	

//(fabs(pConstantData[12]) + fabs(pConstantData[13]) + fabs(pConstantData[14]) > 0.001f) from source  WTF?

	if(stereoView->initialized && validVectorCount(Vector4fCount) && validRegister(StartRegister))
	{
		currentMatrix = const_cast<float*>(pConstantData);

		D3DXMATRIX sourceMatrix(currentMatrix);

		D3DXMatrixTranspose(&sourceMatrix, &sourceMatrix);
			
		sourceMatrix = sourceMatrix * matViewTranslation; 

		D3DXMatrixTranspose(&sourceMatrix, &sourceMatrix);

		currentMatrix = (float*)sourceMatrix;

		return D3DProxyDevice::SetVertexShaderConstantF(StartRegister, currentMatrix, Vector4fCount);
	}

	return D3DProxyDevice::SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
}

/*
HRESULT WINAPI D3DProxyDeviceTest::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil)
{
	//m_pDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);

	return D3DProxyDevice::SetDepthStencilSurface(pNewZStencil);
}

*/

bool D3DProxyDeviceTest::validRegister(UINT reg)
{
	if(reg >=0 && reg < sizeof(registersUsed))
		registersUsed[reg] = true;

	if(reg >=0 && reg < sizeof(validRegisters))
		return validRegisters[reg];

	return false;
//	if(reg == 0) return true;
//	else return false;
	//return true;
}

bool D3DProxyDeviceTest::validVectorCount(UINT count)
{
	//if(count == 4) return true;
	if(count >= 4) return true;
	else return false;
}


int D3DProxyDeviceTest::getMatrixIndex()
{
	return 0;
}

/*
HRESULT WINAPI D3DProxyDeviceTest::SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix)
{
	if(State == D3DTS_VIEW)
	{
		D3DXMATRIX sourceMatrix(*pMatrix);

		D3DXMATRIX transMatrix;
		D3DXMatrixIdentity(&transMatrix);

		D3DXMatrixTranslation(&transMatrix, (separation*eyeShutter+offset)*500.0f, 0, 0);
		D3DXMatrixMultiply(&sourceMatrix, &sourceMatrix, &transMatrix);

		return D3DProxyDevice::SetTransform(State, &sourceMatrix);
	}
	return D3DProxyDevice::SetTransform(State, pMatrix);
}
*/

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

void D3DProxyDeviceTest::HandleControls()
{
	float keySpeed = 0.00002f;
	float keySpeed2 = 0.0005f;
	float mouseSpeed = 0.25f;
	float rollSpeed = 0.01f;
	static int keyWaitCount = 0; 
	keyWaitCount--;
	static int saveWaitCount = 0; 
	saveWaitCount--;
	static bool doSaveNext = false;

	if(keyWaitCount<0)
		keyWaitCount=0;

	if(KEY_DOWN(0x42) && menueDisplay)		//VK_KEY_B
	{
		if(keyWaitCount <= 0)
		{
			transformMethod++;
			if(transformMethod > 3)
				transformMethod = 0;
			keyWaitCount = 50;
		}
	}
	if(KEY_DOWN(0x4E))		//VK_KEY_N
	{
		if(keyWaitCount <= 0)
		{
			if(menueDisplay)
				menueDisplay = false;
			else
				menueDisplay = true;
			keyWaitCount = 50;
		}
	}


	if(KEY_DOWN(VK_RIGHT) && menueDisplay)
	{
		if(keyWaitCount <= 0)
		{
			if(menueMethod == 0) {
				menueMethodItem++;
				if(menueMethodItem >= sizeof(validRegisters))
					menueMethodItem = 0;
			}
			keyWaitCount = 50;
		}
	}
	if(KEY_DOWN(VK_LEFT) && menueDisplay)
	{
		if(keyWaitCount <= 0)
		{
			if(menueMethod == 0) {
				menueMethodItem--;
				if(menueMethodItem < 0 )
					menueMethodItem = sizeof(validRegisters) -1;
			}
			keyWaitCount = 50;
		}
	}
	if(KEY_DOWN(VK_SPACE) && menueDisplay)
	{
		if(keyWaitCount <= 0)
		{
			if(menueMethod == 0) {
				if(validRegisters[menueMethodItem])
					validRegisters[menueMethodItem] = false;
				else
					validRegisters[menueMethodItem] = true;
			}
			keyWaitCount = 50;
		}
	}


	D3DProxyDevice::HandleControls();

}