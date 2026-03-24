#include "CTextures.h"

#include "../../resources/textures/image_preview.h"

CTextures::CTextures()
{
    tPreview = nullptr;
}

CTextures::~CTextures()
{
}

bool CTextures::Initialize(ID3D11Device* pDevice)
{
    // Result of the Initialization.
    D3DX11_IMAGE_LOAD_INFO info0;
    ID3DX11ThreadPump* pump0 = nullptr;

    bool result = true;

    // Setup textures.
    D3DX11CreateShaderResourceViewFromMemory(pDevice, preview_bytes, sizeof(preview_bytes), &info0, pump0, &tPreview, 0);


    D3DX11CreateShaderResourceViewFromMemory(pDevice, preview2_bytes, sizeof(preview2_bytes), &info0, pump0, &tPreview2, 0);

    return result;
}

void CTextures::Shutdown()
{
    tPreview->Release();
}
