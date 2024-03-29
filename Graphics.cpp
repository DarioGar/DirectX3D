#include "Graphics.h"
#include <cmath>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib,"d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	//Descriptor de swap chain
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//Crear dispositivo y buffers frontal/trasero, el swap chain y rendering context
	D3D11CreateDeviceAndSwapChain(
	nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&pSwap,
		&pDevice,
		nullptr,
		&pContext);

	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pTarget
	);
}

void Graphics::EndFrame()
{
	pSwap->Present(1u, 0);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	//Se encarga de "limpiar" la pantalla para que se pinte el siguiente frame 
	const float color[] = { red,green,blue,1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
}

void Graphics::DrawCube(float angle,float z)
{
	{
		namespace wrl = Microsoft::WRL;

		struct Vertice
		{
			struct {
				float x;
				float y;
				float z;
			} pos;
		};

		Vertice vertices[] =
		{
			{-1.0f,-1.0f,-1.0f},
			{1.0f,-1.0f,-1.0f},
			{-1.0f,1.0f,-1.0f},
			{1.0f,1.0f,-1.0f},
			{-1.0f,-1.0f,1.0f},
			{1.0f,-1.0f,1.0f},
			{-1.0f,1.0f,1.0f},
			{1.0f,1.0f,1.0f},
		};

		wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertice);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer);

		//Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertice);
		const UINT offset = 0;
		pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

		//Crear buffer de indices
		const unsigned short indices[] =
		{
			0,2,1, 2,3,1,
			1,3,5, 3,7,5,
			2,6,3, 3,6,7,
			4,5,7, 4,7,6,
			0,4,2, 2,4,6,
			0,1,4, 1,5,4,
		};

		wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
		D3D11_BUFFER_DESC ibd = {};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = sizeof(indices);
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);

		//bind buffer indices
		pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

		//Crear buffer constante para la matriz de transformaciones
		struct ConstantBuffer
		{
			dx::XMMATRIX transform;
		};

		//XMMATRIX est� optimizado para SIMD
		const ConstantBuffer cb =
		{
			{
				dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixRotationX(angle)*
				dx::XMMatrixTranslation(0.0f,0.0f,z + 4.0f) *
				dx::XMMatrixPerspectiveLH(1.0f,3.0f /4.0f,0.5f,10.0f)
				)
			}
		};
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cb);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &cb;
		pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer);
		//bind Buffer constante
		pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

		//Crear buffer constante para la matriz de colores
		struct ConstantBuffer2
		{
			struct 
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		//XMMATRIX est� optimizado para SIMD
		const ConstantBuffer2 cb2 =
		{
			{
				{1.0f,0.0f,1.0f},
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
				{0.0f,1.0f,1.0f},
			}
		};
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
		D3D11_BUFFER_DESC cbd2;
		cbd2.Usage = D3D11_USAGE_DEFAULT;
		cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd2.CPUAccessFlags = 0u;
		cbd2.MiscFlags = 0u;
		cbd2.ByteWidth = sizeof(cb2);
		cbd2.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd2 = {};
		csd2.pSysMem = &cb2;
		pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2);
		//bind Buffer constante
		pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());


		//create pixel shader
		wrl::ComPtr<ID3D11PixelShader> pPixelShader;
		wrl::ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"PixelShader.cso", &pBlob);
		pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader);
		//bind pixel shader
		pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

		//create vertex shader
		wrl::ComPtr<ID3D11VertexShader> pVertexShader;
		D3DReadFileToBlob(L"VertexShader.cso", &pBlob);
		pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader);
		//bind vertex shader
		pContext->VSSetShader(pVertexShader.Get(), 0, 0);

		//input (vertices) distribucion (2d position)
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{"Posicion",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		};


		pDevice->CreateInputLayout(
			ied, (UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout);
		//bind input layout
		pContext->IASetInputLayout(pInputLayout.Get());

		//bind render target
		pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

		pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Configurar viewport
		D3D11_VIEWPORT vp;
		vp.Width = 800;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pContext->RSSetViewports(1u, &vp);
		pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u);

	}
}
