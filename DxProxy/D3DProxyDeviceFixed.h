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

#ifndef D3DPROXYDEVICEFIXED_H_INCLUDED
#define D3DPROXYDEVICEFIXED_H_INCLUDED

#include "Direct3DDevice9.h"
#include "D3DProxyDevice.h"
#include "ProxyHelper.h"

class D3DProxyDeviceFixed : public D3DProxyDevice
{
public:
	D3DProxyDeviceFixed(IDirect3DDevice9* pDevice);
	virtual ~D3DProxyDeviceFixed();
	virtual HRESULT WINAPI BeginScene();
	virtual HRESULT WINAPI EndScene();
	virtual HRESULT WINAPI Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	virtual HRESULT WINAPI SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);

	virtual void Init(ProxyHelper::ProxyConfig& cfg);
};

#endif