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

#ifndef D3DPROXYDEVICETEST_H_INCLUDED
#define D3DPROXYDEVICETEST_H_INCLUDED

#include "Direct3DDevice9.h"
#include "D3DProxyDevice.h"
#include "ProxyHelper.h"

class D3DProxyDeviceTest : public D3DProxyDevice
{
public:
	D3DProxyDeviceTest(IDirect3DDevice9* pDevice);
	virtual ~D3DProxyDeviceTest();
	virtual HRESULT WINAPI BeginScene();
	virtual HRESULT WINAPI EndScene();
	virtual HRESULT WINAPI Present(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion);
	virtual HRESULT WINAPI SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);
	//virtual HRESULT WINAPI SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil);
	virtual HRESULT WINAPI SetTransform(D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix);

	virtual void Init(ProxyHelper::ProxyConfig& cfg);
	bool validRegister(UINT reg);
	bool validVectorCount(UINT count);
	int getMatrixIndex();


	virtual void HandleControls();
	void DisplayMethodsMenu();

	bool menueDisplay;		// display the menu

	int menueMethod;// which method the menu is displaying
	int transformMethod;
	bool transpose;

	int vertexShaderConstantFMethod;
	int menueMethodItem;// which item the menu is displaying
	bool validRegisters[256];
	bool registersUsed[256];

	// used for roll fix
	virtual HRESULT WINAPI SetRenderTarget(DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget);
	virtual HRESULT WINAPI SetClipPlane(DWORD Index,CONST float* pPlane);
	virtual HRESULT WINAPI SetViewport(CONST D3DVIEWPORT9* pViewport);
	virtual HRESULT WINAPI CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain);

	IDirect3DSurface9* pCurrentRenderTarget;
	int targets;
	/*
	SetPixelShaderConstantF 	Sets a floating-point shader constant.
		vs_1_1
				#
			a0	1		address
			c#	96		constant float
			v#	16		input
			r#	12		temp
		vs_2_0
			v#	16		input
			r#	12		temp
			c#	>256	constant float
			a0	1		address
			b#	16		constant bool
			i#	16		constant int
			aL	1		loop counter
		vs_2_x
			v#	16		input
			r#	>12		temp
			c#	>256	constant float
			a0	1		address
			b#	16		constant bool
			i#	16		constant int
			aL	1		loop counter
			p0	1		Predicate 
		vs_3_0
			v#	16		input
			r#	32		temp
			c#	>256	constant float
			a0	1		address
			b#	16		constant bool
			i#	16		constant int
			aL	1		loop counter
			p0	1		Predicate
			s#	4		Sampler


	SetTransform		Sets a single device transformation-related state.
	SetVertexShaderConstantF	Sets a floating-point vertex shader constant.
	SetVertexShader		need to parse shader looking for const matrix



	SetViewport 	Sets the viewport parameters for the device.

	MultiplyTransform 	Multiplies a device's world, view, or projection matrices by a specified matrix. 
	SetDepthStencilSurface 	Sets the depth stencil surface.


	*/

};

#endif