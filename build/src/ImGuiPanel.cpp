#include "zxpch.h"

#include "ImGuiPanel.h"
#include "Application.h"
#include "Utils/Input.h"
#include "Utils/FileDialogs.h"

#include <GLFW/glfw3.h>

ImGuiPanel::ImGuiPanel(std::shared_ptr<Window> window) : m_Window(window)
{
	Init();
}

void ImGuiPanel::Begin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiPanel::End()
{
	ImGuiIO& io = ImGui::GetIO();
	
	float x = static_cast<float>(m_Window->GetWidth());
	float y = static_cast<float>(m_Window->GetHeight());

	io.DisplaySize = ImVec2(x, y);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backupCurrentContext);
	}
}

void ImGuiPanel::Update(float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime > 0.0f ? deltaTime : (float)(1.0f / 60.0f);
}

void ImGuiPanel::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;		
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;    
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	
	io.Fonts->AddFontFromFileTTF("assets/font/OpenSans/OpenSans-regular.ttf", 20.0f);
	io.Fonts->AddFontFromFileTTF("assets/font/OpenSans/OpenSans-regular.ttf", 34.0f);
	io.Fonts->AddFontFromFileTTF("assets/font/FiraCode/FiraCode-Retina.ttf", 18.0f);
	io.Fonts->AddFontFromFileTTF("assets/font/Lato/Lato-Regular.ttf", 18.0f);

	auto& application = Application::Get();
	ChangeTheme(application.GetSettings()->theme);

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetNativeWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void ImGuiPanel::Finalize()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiPanel::ChangeTheme(int theme)
{
	switch (theme)
	{
	case 0:
		{
			ImGui::StyleColorsDark();
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.55f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.68f);
			colors[ImGuiCol_FrameBg] = ImVec4(0.29f, 0.29f, 0.29f, 0.54f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.23f, 0.23f, 0.23f, 0.40f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.24f, 0.24f, 0.67f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.73f, 0.75f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.68f, 0.70f, 0.72f, 1.00f);
			colors[ImGuiCol_Button] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.22f, 0.23f, 0.23f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 0.31f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.13f, 0.13f, 0.13f, 0.80f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.53f, 0.53f, 0.53f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.24f, 0.24f, 0.20f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.35f, 0.35f, 0.35f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.24f, 0.24f, 0.95f);
			colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 0.86f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.80f);
			colors[ImGuiCol_TabActive] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.40f, 0.41f, 0.41f, 0.29f);
			colors[ImGuiCol_DockingPreview] = ImVec4(0.51f, 0.51f, 0.51f, 0.70f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		}
		break;
	
	case 1:
		ImGui::StyleColorsLight();
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 0.77f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.85f, 0.57f, 0.32f, 0.30f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.06f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.89f, 0.89f, 0.89f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.68f, 0.68f, 0.68f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.68f, 0.68f, 0.68f, 0.67f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.69f, 0.23f, 0.85f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.82f, 0.21f, 0.80f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.54f, 0.26f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.89f, 0.65f, 0.35f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.63f, 0.22f, 0.78f);
		colors[ImGuiCol_Button] = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.79f, 0.40f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.64f, 0.15f, 0.90f);
		colors[ImGuiCol_Header] = ImVec4(1.00f, 0.75f, 0.33f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.87f, 0.34f, 0.80f);
		colors[ImGuiCol_Tab] = ImVec4(0.70f, 0.70f, 0.70f, 0.93f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.77f, 0.77f, 0.77f, 0.85f);
		colors[ImGuiCol_TabActive] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.52f, 0.52f, 0.52f, 0.99f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.97f, 0.40f, 0.14f, 0.78f);
		break;
	}
}

