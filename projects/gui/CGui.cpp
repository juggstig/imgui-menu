#include "CGui.h"
#include "CDirectX.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "addons/imgui_addons.h"
#include "imgui_freetype.h"

#include "../../resources/fonts/roboto_medium.h"
#include "../../resources/fonts/IconsFontAwesome6.h"

CGui::CGui()
{
    m_hWnd              = 0;
    m_pDevice           = 0;
    m_pDeviceContext    = 0;
    m_pTextures         = 0;

    memset(m_bDrawPage, true, sizeof m_bDrawPage);
}

CGui::~CGui()
{
}

bool CGui::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    if (!hwnd)
        return false;
    
    if (!device)
        return false;
    
    if (!device_context)
        return false;

    m_pTextures = new CTextures();
    m_pTextures->Initialize(device);

    m_hWnd = hwnd;
    m_pDevice = device;
    m_pDeviceContext = device_context;
    
    bool result = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    io.IniFilename = nullptr;               // Disable INI File  
    GImGui->NavDisableHighlight = true;     // Disable Highlighting

    if (!GImGui->NavDisableHighlight)
    {
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    }

    // Setup Dear ImGui default style
    ImGui::StyleColorsDark();

    // Custom Styles
    style.WindowRounding    = 0;
    style.ChildRounding     = 0;
    style.FrameRounding     = 0;
    style.PopupRounding     = 0;
    style.ScrollbarRounding = 0;
    style.TabRounding       = 0;

    style.ButtonTextAlign   = { 0.5f, 0.5f };
    style.WindowTitleAlign  = { 0.5f, 0.5f };
    style.FramePadding      = { 3.0f, 3.0f };
    style.WindowPadding     = { 6.0f, 6.0f };
    style.ItemSpacing       = style.WindowPadding;
    style.ItemInnerSpacing  = { style.ItemSpacing.x, 5 };

    style.WindowBorderSize  = 1;
    style.FrameBorderSize   = 1;
    style.PopupBorderSize   = 1;

    style.ScrollbarSize     = 4.f;
    style.GrabMinSize       = 4;
    style.WindowShadowSize  = 50;
    
    // Custom Colors
    style.Colors[ImGuiCol_WindowBg]             = ImAdd::HexToColorVec4(0x2a2a38, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImAdd::HexToColorVec4(0x24242f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImAdd::HexToColorVec4(0x24242f, 1.00f);
    
    style.Colors[ImGuiCol_Text]                 = ImAdd::HexToColorVec4(0xc8c8c8, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImAdd::HexToColorVec4(0x888888, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImAdd::HexToColorVec4(0x000000, 1.00f); // text border

    style.Colors[ImGuiCol_SliderGrab]           = ImAdd::HexToColorVec4(0x9a7fac, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImAdd::HexToColorVec4(0x534861, 1.00f);
    
    style.Colors[ImGuiCol_Border]               = ImAdd::HexToColorVec4(0x353543, 1.00f);
    style.Colors[ImGuiCol_Separator]            = style.Colors[ImGuiCol_Border];
    
    style.Colors[ImGuiCol_Button]               = ImAdd::HexToColorVec4(0x2a2a38, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImAdd::HexToColorVec4(0x2b2b47, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]         = ImAdd::HexToColorVec4(0x282835, 1.00f);
    
    style.Colors[ImGuiCol_FrameBg]              = style.Colors[ImGuiCol_Button];
    style.Colors[ImGuiCol_FrameBgHovered]       = style.Colors[ImGuiCol_ButtonHovered];
    style.Colors[ImGuiCol_FrameBgActive]        = style.Colors[ImGuiCol_ButtonActive];
    
    style.Colors[ImGuiCol_Header]               = ImAdd::HexToColorVec4(0x2a2a38, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImAdd::HexToColorVec4(0x2b2b39, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]         = ImAdd::HexToColorVec4(0x282835, 1.00f);
    
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4();
    style.Colors[ImGuiCol_ScrollbarGrab]        = style.Colors[ImGuiCol_SliderGrab];
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = style.Colors[ImGuiCol_SliderGrab];
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = style.Colors[ImGuiCol_SliderGrab];
    style.Colors[ImGuiCol_TextSelectedBg]       = style.Colors[ImGuiCol_SliderGrab];
    style.Colors[ImGuiCol_WindowShadow]         = ImAdd::HexToColorVec4(0x000000, 0.20f);

    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4();
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4();
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4();

    // Setup Font in order

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig cfg;
    cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
    cfg.SizePixels = 12;
    ImFontConfig fa_config; fa_config.MergeMode = true; fa_config.PixelSnapH = true;
    fa_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_ForceAutoHint;
    //io.Fonts->AddFontFromMemoryCompressedTTF(roboto_medium_compressed_data, roboto_medium_compressed_size, 12, &cfg, io.Fonts->GetGlyphRangesDefault());
    io.Fonts->AddFontDefault(&cfg);
    io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 14, &cfg, icons_ranges);

    // Setup Platform/Renderer backends
    result &= ImGui_ImplWin32_Init(hwnd);
    result &= ImGui_ImplDX11_Init(device, device_context);

	return result;
}

void CGui::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    m_pTextures->Shutdown();
}

void CGui::Render(POINTS window_size)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    {
        float RadioSize = io.Fonts->Fonts[ImFont_Icons]->FontSize + style.WindowPadding.x * 2 + style.FramePadding.y;

        ImGui::SetNextWindowSize(ImVec2((RadioSize * MenuPages_COUNT) + style.WindowPadding.x * (MenuPages_COUNT + 1), RadioSize + style.WindowPadding.y * 2 + style.WindowBorderSize * 2), ImGuiCond_Once);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
        ImGui::Begin("NavBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        {
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            ImDrawList* pDrawList = ImGui::GetWindowDrawList();

            ImGui::SetWindowPos(ImVec2(window_size.x / 2 - size.x / 2, style.WindowPadding.y * 2));

            pDrawList->AddRectFilled(pos, pos + size, ImGui::GetColorU32(ImGuiCol_ChildBg));

            if (style.WindowBorderSize > 0) {
                pDrawList->AddRect(pos, pos + size, ImGui::GetColorU32(ImGuiCol_Border));
                pDrawList->AddLine(pos + ImVec2(style.WindowBorderSize, style.WindowBorderSize), pos + ImVec2(size.x - style.WindowBorderSize, style.WindowBorderSize), ImGui::GetColorU32(ImGuiCol_SliderGrab));
                pDrawList->AddLine(pos + ImVec2(style.WindowBorderSize, style.WindowBorderSize * 2), pos + ImVec2(size.x - style.WindowBorderSize, style.WindowBorderSize * 2), ImGui::GetColorU32(ImGuiCol_SliderGrabActive));
            }

            ImGui::SetCursorScreenPos(pos + ImVec2(style.WindowPadding.x, style.WindowPadding.y + style.WindowBorderSize * 2));
            ImGui::BeginGroup();
            ImGui::PushFont(io.Fonts->Fonts[ImFont_Icons]);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, style.WindowPadding);
            {
                ImAdd::FrameRadioIcon(ICON_FA_HOUSE, &m_bDrawPage[MenuPage_Home], ImVec2(RadioSize, RadioSize));
                ImGui::SameLine();
                ImAdd::FrameRadioIcon(ICON_FA_PERSON, &m_bDrawPage[MenuPage_Self], ImVec2(RadioSize, RadioSize));
                ImGui::SameLine();
                ImAdd::FrameRadioIcon(ICON_FA_USER, &m_bDrawPage[MenuPage_Players], ImVec2(RadioSize, RadioSize));
                ImGui::SameLine();
                ImAdd::FrameRadioIcon(ICON_FA_LIST, &m_bDrawPage[MenuPage_Misc], ImVec2(RadioSize, RadioSize));
                ImGui::SameLine();
                ImAdd::FrameRadioIcon(ICON_FA_CODE, &m_bDrawPage[MenuPage_Scripts], ImVec2(RadioSize, RadioSize));
                ImGui::SameLine();
                ImAdd::FrameRadioIcon(ICON_FA_PALETTE, &m_bDrawPage[MenuPage_Appearance], ImVec2(RadioSize, RadioSize));
                ImGui::SameLine();
                ImAdd::FrameRadioIcon(ICON_FA_CLOUD_ARROW_DOWN, &m_bDrawPage[MenuPage_Updates], ImVec2(RadioSize, RadioSize));
            }
            ImGui::PopStyleVar();
            ImGui::PopFont();
            ImGui::EndGroup();
        }
        ImGui::End();

        // make this a global or smth
        static std::vector<const char*> keys =
        {
            "[ Toggle ] Preview Toggle - E",
            "[ Toggle ] Menu Toggle - INSERT"
        };

        if (m_bDrawPage[MenuPage_Scripts])
        {
            ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::Begin("KeyList", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            {
                int max_lenght = ImGui::CalcTextSize(keys[0]).x;

                for (int i = 1; i < keys.size(); ++i) {
                    if (ImGui::CalcTextSize(keys[i]).x > max_lenght) {
                        max_lenght = ImGui::CalcTextSize(keys[i]).x;
                    }
                }

                ImGui::SetWindowSize(ImVec2(max_lenght + style.WindowPadding.x * 2, (ImGui::GetFontSize() + style.WindowPadding.y * 2) + style.WindowPadding.y * 2 + (ImGui::GetFontSize() * keys.size()) + (style.ItemSpacing.y * (keys.size() - 1))));

                ImAdd::BeginChild("Keybinds", ImVec2(0, ImGui::GetFontSize() + style.WindowPadding.y * 2));
                {
                }
                ImAdd::EndChild();
                ImGui::SetCursorPosY(ImGui::GetFontSize() + style.WindowPadding.y * 2);
                ImGui::BeginChild("Content", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_NoBackground);
                {
                    for (int i = 0; i < keys.size(); i++)
                    {
                        ImAdd::Text(style.Colors[ImGuiCol_Text], keys[i]);
                    }
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }

        if (m_bDrawPage[MenuPage_Home])
        {
            ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(390, 430), ImGuiCond_Once);
            ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::Begin("Home", nullptr, ImGuiWindowFlags_NoTitleBar);
            ImGui::PopStyleColor(2);
            {
                ImAdd::Text(style.Colors[ImGuiCol_SliderGrab], "main");
                ImGui::BeginChild("SecondLayer", ImVec2(), ImGuiChildFlags_Border);
                {
                    static int iTabID = 0;
                    float fRadioWidth = (ImGui::GetWindowWidth() - style.WindowPadding.x * 2 - style.ItemSpacing.x * 3) / 4;

                    ImGui::BeginGroup();
                    {
                        ImAdd::FrameRadio("Legit", &iTabID, 0, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2));
                        ImGui::SameLine();
                        ImAdd::FrameRadio("Players", &iTabID, 1, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2));
                        ImGui::SameLine();
                        ImAdd::FrameRadio("Visuals", &iTabID, 2, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2));
                        ImGui::SameLine();
                        ImAdd::FrameRadio("Misc", &iTabID, 3, ImVec2(fRadioWidth, ImGui::GetFontSize() + style.WindowPadding.y * 2));
                    }
                    ImGui::EndGroup();
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
                    ImGui::BeginChild("TabsLayer", ImVec2(0, 0), ImGuiChildFlags_Border);
                    ImGui::PopStyleColor();
                    {
                        float fGroupWidth = (ImGui::GetWindowWidth() - style.ItemSpacing.x - style.WindowPadding.x * 2) / 2;
                        float fGroupHeight = (ImGui::GetWindowHeight() - style.ItemSpacing.y - style.WindowPadding.y * 2) / 2;

                        ImGui::BeginGroup();
                        {
                            ImAdd::BeginChild("Aim Assit", ImVec2(fGroupWidth, 0));
                            {
                                static bool bEnabled = true;
                                static bool bDisabled = false;
                                static int iKey = VK_INSERT;
                                ImAdd::CheckBox("Enabled", &bEnabled);
                                ImAdd::CheckBox("Disabled", &bDisabled);
                               // ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(szKeyNames[iKey]).x - style.WindowPadding.x);
                           //     ImAdd::KeyBind("keybind_name", &iKey, ImGui::CalcTextSize(szKeyNames[iKey]).x);

                               // ImGui::Separator();

                                ImAdd::Button("Button", ImVec2(-0.1f, 0));
                                ImGui::Separator();

                                static ImVec4 colPick = style.Colors[ImGuiCol_SliderGrab];
                                ImAdd::ColorEdit4("Color Picker", (float*)&colPick);
                            }
                            ImAdd::EndChild();
                        }
                        ImGui::EndGroup();
                        ImGui::SameLine();
                        ImGui::BeginGroup();
                        {
                            ImAdd::BeginChild("Trigger Assit", ImVec2(0, fGroupHeight));
                            {
                                static int iSlider = 7;
                                static float fSlider = 3;
                                ImAdd::SliderInt("Slider Int", &iSlider, 0, 10);
                                ImAdd::SliderFloat("Slider Float", &fSlider, 0, 10);
                            }
                            ImAdd::EndChild();
                            ImAdd::BeginChild("Misc", ImVec2(0, 0));
                            {
                                static std::vector<int> multiC = { 1, 0, 0, 1, 0 };
                                ImAdd::MultiCombo("Multi Combo", &multiC, { "Banana", "Apple", "Tomato", "Shit", "123" });

                                static int iCombo = 0;
                                ImAdd::Combo("Combo", &iCombo, "Tomato\0" "Lemon\0" "Orange\0");
                            }
                            ImAdd::EndChild();
                        }
                        ImGui::EndGroup();
                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }

        if (m_bDrawPage[MenuPage_Self])
        {
            ImGui::SetNextWindowSize(ImVec2(250, 360));
            ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowBorderSize, style.WindowBorderSize));
            ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            {
                ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrab]);
                ImGui::BeginChild("preview", ImVec2(), ImGuiChildFlags_Border);
                ImGui::PopStyleColor();
                {
                    ImAdd::Text(style.Colors[ImGuiCol_Text], "Preview");
                    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrabActive]);
                    ImGui::BeginChild("frame_preview", ImVec2(), ImGuiChildFlags_Border);
                    ImGui::PopStyleColor();
                    {
                      //  ImGui::Image(m_pTextures->tPreview, ImGui::GetWindowSize() - style.WindowPadding * 2);

                      //  ImGui::Image(m_pTextures->tPreview2, ImGui::GetWindowSize() - style.WindowPadding * 2);

                        const char* items[] = { "Phantom Force", "Dahoodian" };
                        static int currentItem = 0;

                        ImGui::Combo("##combo", &currentItem, items, IM_ARRAYSIZE(items));

                        if (currentItem == 0) {


                            ImGui::Image(m_pTextures->tPreview2, ImGui::GetWindowSize() - style.WindowPadding * 2);


                        }
                        else {
                            ImGui::Image(m_pTextures->tPreview, ImGui::GetWindowSize() - style.WindowPadding * 2);
                        }

                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
        
        if (m_bDrawPage[MenuPage_Self])
        {

        }
        
        if (m_bDrawPage[MenuPage_Players])
        {
            ImGui::SetNextWindowSize(ImVec2(320, 370));
            ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowBorderSize, style.WindowBorderSize));
            ImGui::Begin("Bones", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            {
                ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrab]);
                ImGui::BeginChild("Bones", ImVec2(), ImGuiChildFlags_Border);
                ImGui::PopStyleColor();
                {
                    ImAdd::Text(style.Colors[ImGuiCol_Text], "Bones");
                    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrabActive]);
                    ImGui::BeginChild("Bones", ImVec2(), ImGuiChildFlags_Border);
                    ImGui::PopStyleColor();
                    {
                        //  ImGui::Image(m_pTextures->tPreview, ImGui::GetWindowSize() - style.WindowPadding * 2);

                        //  ImGui::Image(m_pTextures->tPreview2, ImGui::GetWindowSize() - style.WindowPadding * 2);

                        ImVec2 headSize(60, 56);
                        ImVec2 torsoSize(80, 100);
                        ImVec2 armSize(40, 100);
                        ImVec2 legSize(40, 100);

                        static bool headClicked = false;
                        static bool leftArmClicked = false;
                        static bool rightArmClicked = false;
                        static bool torsoClicked = false;
                        static bool leftLegClicked = false;
                        static bool rightLegClicked = false;

                        ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
                        ImVec4 clickedColor = ImVec4(
                            ((float)((0xa8) / 255.0f)),
                            ((float)((0x9d) / 255.0f)), 
                            ((float)((0x9f) / 255.0f)),  
                            1.0f 
                        );
                        // Head
                        ImGui::SetCursorPos(ImVec2(120, 44.3));
                        ImGui::PushStyleColor(ImGuiCol_Button, headClicked ? clickedColor : normalColor);
                        if (ImGui::Button("##Head", headSize)) {
                            headClicked = !headClicked;  // Toggle state
                            printf("Head clicked\n");
                        }
                        ImGui::PopStyleColor();

                        // Left Arm
                        ImGui::SetCursorPos(ImVec2(70, 100));
                        ImGui::PushStyleColor(ImGuiCol_Button, leftArmClicked ? clickedColor : normalColor);
                        if (ImGui::Button("##LeftArm", armSize)) {
                            leftArmClicked = !leftArmClicked;  // Toggle state
                            printf("Left arm clicked\n");
                        }
                        ImGui::PopStyleColor();

                        // Torso
                        ImGui::SetCursorPos(ImVec2(110, 100));
                        ImGui::PushStyleColor(ImGuiCol_Button, torsoClicked ? clickedColor : normalColor);
                        if (ImGui::Button("##Torso", torsoSize)) {
                            torsoClicked = !torsoClicked;  // Toggle state
                            printf("Torso clicked\n");
                        }
                        ImGui::PopStyleColor();

                        // Right Arm
                        ImGui::SetCursorPos(ImVec2(190, 100));
                        ImGui::PushStyleColor(ImGuiCol_Button, rightArmClicked ? clickedColor : normalColor);
                        if (ImGui::Button("##RightArm", armSize)) {
                            rightArmClicked = !rightArmClicked;  // Toggle state
                            printf("Right arm clicked\n");
                        }
                        ImGui::PopStyleColor();

                        // Left Leg
                        ImGui::SetCursorPos(ImVec2(110, 200));
                        ImGui::PushStyleColor(ImGuiCol_Button, leftLegClicked ? clickedColor : normalColor);
                        if (ImGui::Button("##LeftLeg", legSize)) {
                            leftLegClicked = !leftLegClicked;  // Toggle state
                            printf("Left leg clicked\n");
                        }
                        ImGui::PopStyleColor();

                        // Right Leg
                        ImGui::SetCursorPos(ImVec2(150, 200));
                        ImGui::PushStyleColor(ImGuiCol_Button, rightLegClicked ? clickedColor : normalColor);
                        if (ImGui::Button("##RightLeg", legSize)) {
                            rightLegClicked = !rightLegClicked;  // Toggle state
                            printf("Right leg clicked\n");
                        }
                        ImGui::PopStyleColor();



                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
        }
        
        if (m_bDrawPage[MenuPage_Misc])
        {
            ImGui::SetNextWindowSize(ImVec2(400, 360));
            ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowBorderSize, style.WindowBorderSize));
            ImGui::Begin("Explorer", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            {
                ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrab]);
                ImGui::BeginChild("Explorer", ImVec2(), ImGuiChildFlags_Border);
                ImGui::PopStyleColor();
                {
                    ImAdd::Text(style.Colors[ImGuiCol_Text], "Explorer");
                    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrabActive]);
                    ImGui::BeginChild("Explorer", ImVec2(), ImGuiChildFlags_Border);
                    ImGui::PopStyleColor();
                    {

                        ImGui::TreeNode("Workspace");


                        ImGui::TreeNode("Run Service");
                        ImGui::TreeNode("GuiService");



                        ImGui::TreeNode("Run Service");
                        ImGui::TreeNode("Stats");
                        ImGui::Separator();

                        ImGui::Text("N");

                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
        
        if (m_bDrawPage[MenuPage_Scripts])
        {

        }
        
        if (m_bDrawPage[MenuPage_Appearance])
        {

        }
        
        if (m_bDrawPage[MenuPage_Updates])
        {
            ImGui::SetNextWindowSize(ImVec2(350, 360));
            ImGui::PushStyleColor(ImGuiCol_WindowShadow, style.Colors[ImGuiCol_SliderGrab]);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowBorderSize, style.WindowBorderSize));
            ImGui::Begin("Configurations", nullptr, ImGuiWindowFlags_NoDecoration);
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            {
                ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrab]);
                ImGui::BeginChild("Configurations", ImVec2(), ImGuiChildFlags_Border);
                ImGui::PopStyleColor();
                {
                    ImAdd::Text(style.Colors[ImGuiCol_Text], "Configurations");
                    ImGui::PushStyleColor(ImGuiCol_Border, style.Colors[ImGuiCol_SliderGrabActive]);
                    ImGui::BeginChild("frame_preview", ImVec2(), ImGuiChildFlags_Border);
                    ImGui::PopStyleColor();
                    {

                        static char inputText[256] = "";
                        ImGui::InputText("Config Name", inputText, IM_ARRAYSIZE(inputText));
                        ImAdd::Button("Button", ImVec2(-0.1f, 0));

                        //ImGui::SameLine();

                        ImAdd::Button("Buttonl", ImVec2(-0.1f, 0));



                    }
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
        }

    }
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool CGui::MsgProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    return ImGui_ImplWin32_WndProcHandler(hwnd, umsg, wparam, lparam);
}
