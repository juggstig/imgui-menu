#pragma once

// DirectX
#include <D3DX11tex.h>

class CTextures
{
public:
	CTextures();
	~CTextures();

	bool Initialize(ID3D11Device* pDevice);
	void Shutdown();

public:
	ID3D11ShaderResourceView* tPreview;

	ID3D11ShaderResourceView* tPreview2;

};
