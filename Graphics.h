#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <xhash>
#include <xutility>

#pragma comment(lib,"D3DCompiler.lib")

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawCube(float angle,float z);
	
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr < IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};