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

#include "D3DProxyDeviceMono.h"

D3DProxyDeviceMono::D3DProxyDeviceMono(IDirect3DDevice9* pDevice):D3DProxyDevice(pDevice)
{
	Log( "[D3DProxyDeviceMono] Created" );
}

D3DProxyDeviceMono::~D3DProxyDeviceMono()
{
}

HRESULT WINAPI D3DProxyDeviceMono::BeginScene()
{
	HandleControls();
//	HandleTracking();
	ComputeViewTranslation();

	return D3DProxyDevice::BeginScene();
}

HRESULT WINAPI D3DProxyDeviceMono::EndScene()
{
	return D3DProxyDevice::EndScene();
}

HRESULT WINAPI D3DProxyDeviceMono::Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion)
{
	if(stereoView->initialized && stereoView->stereoEnabled)
	{
		stereoView->UpdateEye(StereoView::LEFT_EYE);
		stereoView->UpdateEye(StereoView::RIGHT_EYE);
		stereoView->Draw();
	}

	return D3DProxyDevice::Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}