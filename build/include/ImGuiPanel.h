#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Window.h"

class ImGuiPanel
{
public: 
	ImGuiPanel(std::shared_ptr<Window> window);

	void Begin();
	void End();

	void Update(float deltaTime);
	void Finalize();

	void ChangeTheme(int theme);

private:
	void Init();
	
	std::shared_ptr<Window> m_Window;
};

