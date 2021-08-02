#include "zxpch.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "Application.h"
#include "Panels.h"
#include "Utils/Input.h"
#include "Utils/FileDialogs.h"

void TextCentered(const std::string& text)
{
	auto& io = ImGui::GetIO();
	auto font = io.Fonts->Fonts[1];
	ImGui::PushFont(font);

	auto windowWidth = ImGui::GetWindowSize().x;
	auto windowHeight = ImGui::GetWindowSize().y;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
	auto textHeight = ImGui::CalcTextSize(text.c_str()).y;

	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::SetCursorPosY((windowHeight - textHeight) * 0.5f);
	ImGui::Button(text.c_str());

	ImGui::PopFont();
}

void ViewportPanel::DrawUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::Begin("Viewport", &m_Active, m_WindowFlags);
	if (auto shader = Renderer::GetRenderer()->GetShader())
	{
		if (shader->IsLinked())
		{
			ImVec2 viewportSize = ImGui::GetContentRegionAvail();
			if (m_ViewportSize != (*reinterpret_cast<glm::vec2*>(&viewportSize)))
			{
				m_FrameBuffer->Resize((unsigned int)viewportSize.x, (unsigned int)viewportSize.y);
				m_ViewportSize = { viewportSize.x, viewportSize.y };
			}

			size_t textureID = static_cast<size_t>(m_FrameBuffer->GetColorAttachment());
			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			if (ImGui::IsWindowHovered())
			{
				auto window = Application::Get().GetWindow();
				auto width = window->GetWidth();
				auto height = window->GetHeight();

				width -= m_ViewportSize.x;
				height -= m_ViewportSize.y;

				ImVec2 mPos = ImGui::GetMousePos();
				m_MousePos = { mPos.x - width, mPos.y - height };
			}
		}
		else
		{
			TextCentered("The shader couldn't be compiled!\nCheck your source code!");
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void ShaderEditorPanel::DrawUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 3, 3 });
	ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);

	ImGui::Begin("Shader Editor", &m_Active, m_WindowFlags);
	if(m_Filename != "")
	{
		if (m_FragmentSrc == "")
			if (auto shader = Renderer::GetRenderer()->GetShader())
				m_FragmentSrc = shader->GetFragmentSource();

		auto& io = ImGui::GetIO();
		auto font = io.Fonts->Fonts[2];
		ImGui::PushFont(font);

		ImGui::BeginChild(ImGui::GetID("Fragment"));
		ImGui::InputTextMultiline("Fragment",
			(char*)m_FragmentSrc.c_str(),
			m_FragmentSrc.capacity() + 1,
			ImGui::GetWindowSize(), 
			ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize, 
			[](ImGuiInputTextCallbackData* data) -> int
			{
				if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
				{
		 			std::string* str = (std::string*)data->UserData;
					IM_ASSERT(data->Buf == str->c_str());
					str->resize((size_t)data->BufTextLen + 1ui64);
					data->Buf = (char*)str->c_str();
				}

				return 0;
			},
			reinterpret_cast<void*>(&m_FragmentSrc)
		);
		ImGui::SetScrollY(ImGui::GetScrollMaxY());
		ImGui::EndChild();

		ImGui::PopFont();

		if (auto shader = Renderer::GetRenderer()->GetShader())
		{
			if (m_FragmentSrc != Renderer::GetRenderer()->GetShader()->GetFragmentSource())
			{
				m_Saved = false;
				if (Application::Get().GetWindow()->GetTitle().find('*') == std::string::npos)
					Application::Get().GetWindow()->SetTitle(Application::Get().GetWindow()->GetTitle() + '*');

				if (Application::Get().GetSettings()->autoSaving)
					Application::Get().WriteToTempFile(m_Filename + ' ' + m_FragmentSrc);
			}
		}
	}

	ImGui::End();
	ImGui::PopStyleVar(2);
}

void MenuBarPanel::DrawUI()
{
	ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoCloseButton;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("MenuBar", nullptr, m_WindowFlags);
	ImGui::PopStyleVar(3);

	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id = ImGui::GetID("MenuBarDockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspaceFlags);

	auto& applicationInstance = Application::Get();
	auto& rendererInstance = Renderer::GetRenderer();
	
	auto activeShader = rendererInstance->GetShader();

	auto shaderEditorPanel  = GetPanel<ShaderEditorPanel>("Shader Editor");
	auto updateChangesPanel = GetPanel<UpdateChangesPanel>("Update Changes");
	
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("New", "Ctrl+N", nullptr))
			{
				if (shaderEditorPanel->IsSaved())
				{
					applicationInstance.NewFile();
				}
				else
				{
					updateChangesPanel->Activate(true, 0);
				}
			}

			if (ImGui::MenuItem("Open", "Ctrl+O", nullptr))
			{
				if (shaderEditorPanel->IsSaved())
				{
					applicationInstance.OpenFile(FileDialogs::OpenFile("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0"));
				}
				else
				{
					updateChangesPanel->Activate(true, 1);
				}
			}

			if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, activeShader || shaderEditorPanel->GetFilename() == "untitled.frag"))
			{
				if (activeShader)
				{
					auto fragSrc = shaderEditorPanel->GetFragSource();
					applicationInstance.SaveFile(fragSrc);
				}
				else
				{
					auto fragSource = shaderEditorPanel->GetFragSource();
					if (shaderEditorPanel->GetFilename() == "untitled.frag")
					{
						auto result = FileDialogs::SaveFileAs("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0");
						shaderEditorPanel->SetFilename(result == "" ? "untitled.frag" : result);
						if (result != "")
							applicationInstance.SaveFile(fragSource, shaderEditorPanel->GetFilename());
					}
				}
			}

			if (ImGui::MenuItem("Save As...", "Ctrl+Alt+S", nullptr, activeShader.get()))
			{
				if (activeShader)
				{
					auto fragSrc = shaderEditorPanel->GetFragSource();
					applicationInstance.SaveFile(fragSrc, FileDialogs::SaveFileAs("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0"));
				}
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Close File", "Shift+C", nullptr, activeShader.get()))
			{
				if (shaderEditorPanel->IsSaved())
					applicationInstance.CloseFile();
				else
					updateChangesPanel->Activate(true, 2);
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Alt+F4", nullptr))
			{
				applicationInstance.Close();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Editor Preferences", nullptr, nullptr))
			{
				auto editorPreferencePanel = GetPanel<EditorPreferencesPanel>("Editor Preferences");
				editorPreferencePanel->Activate(true);
			}
		
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Shader"))
		{
			if (ImGui::MenuItem("Recompile Shader", "Ctrl+F5", nullptr, !applicationInstance.GetSettings()->hotReload && activeShader))
			{
				auto filepath = activeShader->GetFilepath();

				applicationInstance.OpenFile(filepath, true);
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Delete Cache", nullptr, nullptr, rendererInstance->GetRawShaderCache().size() > 0))
			{
				applicationInstance.CloseFile();
				rendererInstance->DeleteShaderCache();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			for (auto [name, panel] : applicationInstance.GetPanels())
				if (name != "Editor Preferences" && name != "Menu Bar" && name != "Update Changes" && name != "About")
					if (ImGui::MenuItem(name.c_str(), nullptr, false, !panel->IsActive()))
						panel->Activate(true);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("?"))
		{
			if (ImGui::MenuItem("About", "Ctrl+H", nullptr))
			{
				auto aboutPanel = GetPanel<AboutPanel>("About");
				aboutPanel->Activate(true);
			}
			
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::End();
}

void MenuBarPanel::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);

	dispatcher.Emit<KeyPressed>([](KeyPressed& e) -> bool {

		Key keycode = (Key)e.GetKeyCode();

		bool control = Input::GetKeyPress(Key::LeftControl) || Input::GetKeyPress(Key::RightControl);
		bool shift = Input::GetKeyPress(Key::LeftShift) || Input::GetKeyPress(Key::RightShift);
		bool alt = Input::GetKeyPress(Key::LeftAlt) || Input::GetKeyPress(Key::RightAlt);

		auto& applicationInstance = Application::Get();
		auto& rendererInstance = Renderer::GetRenderer();

		auto shaderEditorPanel = GetPanel<ShaderEditorPanel>("Shader Editor");

		switch (keycode)
		{
		case Key::N:
			if (control)
			{
				if (shaderEditorPanel)
				{
					if (shaderEditorPanel->IsSaved())
					{
						applicationInstance.NewFile();
					}
					else
					{
						
						auto updateChangesPanel = GetPanel<UpdateChangesPanel>("Update Changes");
						updateChangesPanel->Activate(true, 0);
					}
				}
			}
			break;
		case Key::O:
			if (control)
				applicationInstance.OpenFile(FileDialogs::OpenFile("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader (*.fragment)\0 * .fragment\0"));
			break;
		case Key::S:
			if (control && alt)
			{
				if (rendererInstance->GetShader())
				{
					auto fragSrc = shaderEditorPanel->GetFragSource();
					applicationInstance.SaveFile(fragSrc, FileDialogs::SaveFileAs("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader (*.fragment)\0 * .fragment\0"));
				}
			}
			else if (control)
			{
				if (rendererInstance->GetShader())
				{
					auto fragSrc = shaderEditorPanel->GetFragSource();
					applicationInstance.SaveFile(fragSrc);
				}
				else
				{
					auto fragSource = shaderEditorPanel->GetFragSource();
					if (shaderEditorPanel->GetFilename() == "untitled.frag")
					{
						auto result = FileDialogs::SaveFileAs("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0");
						shaderEditorPanel->SetFilename(result == "" ? "untitled.frag" : result);
						if (result != "")
							applicationInstance.SaveFile(fragSource, shaderEditorPanel->GetFilename());
					}
				}
			}
			break;

		case Key::W:
			if (control)
			{
				auto updateChangesPanel = GetPanel<UpdateChangesPanel>("Update Changes");

				if (shaderEditorPanel->IsSaved())
					applicationInstance.CloseFile();
				else
					updateChangesPanel->Activate(true, 2);
			}
			break;
		case Key::H:
			if (control)
			{
				auto aboutPanel = GetPanel<AboutPanel>("About");
				aboutPanel->Activate(true);
			}
			break;
		case Key::F4:
			if (alt)
			{
				applicationInstance.Close();
			}
			break;
		case Key::F5:
			if (control && !applicationInstance.GetSettings()->hotReload && rendererInstance->GetShader())
			{
				applicationInstance.OpenFile(rendererInstance->GetShader()->GetFilepath(), true);
			}
			break;
		}

		return true;
	});
}

void LogPanel::DrawUI()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5.0f, 5.0f });
	ImGui::Begin("Output Logger", &m_Active, m_WindowFlags);
	if (ImGui::Button("Clear")) m_LogMsgs.clear();
	ImGui::SameLine();
	if (ImGui::Button("Copy to Clipboard"))
	{
		std::stringstream clipboard;
		for (auto& [msg, severity] : m_LogMsgs)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, GetColorFromSeverity(severity));
			clipboard << msg;
			ImGui::PopStyleColor();
		}
		
		ImGui::SetClipboardText(clipboard.str().c_str());
	}

	ImGui::Separator();

	auto& io = ImGui::GetIO();
	auto font = io.Fonts->Fonts[3];
	ImGui::PushFont(font);

	for (auto it = m_LogMsgs.rbegin(); it != m_LogMsgs.rend(); ++it)
	{
		ImGui::TextColored(GetColorFromSeverity(it->second), "%s", it->first.c_str());
	}
	
	ImGui::PopFont();

	ImGui::End();
	ImGui::PopStyleVar();
}

void LogPanel::PushMessage(Severity severity, const std::string& msg)
{
	m_LogMsgs.emplace_back(msg, severity);
}

void EditorPreferencesPanel::DrawUI()
{
	auto settings = Application::Get().GetSettings();

	if (m_Active && !ImGui::IsPopupOpen("Editor Preferences"))
		ImGui::OpenPopup("Editor Preferences");

	if (ImGui::BeginPopupModal("Editor Preferences", &m_Active, m_WindowFlags))
	{
		ImGui::SetWindowSize({ 500, 400 });
		if (ImGui::CollapsingHeader("Theme"))
		{
			if (ImGui::Combo("Selected Theme", &settings->theme, "Dark\0Light\0"))
			{
				Application::Get().ChangeEditorTheme(settings->theme);
			}
		}

		if (ImGui::CollapsingHeader("Environment"))
		{
			ImGui::Text("Uniform Names");
			ImGui::InputText("Resolution", settings->uResolutionName.data(), 25);
			ImGui::InputText("Mouse Position", settings->uMousePositionName.data(), 25);
			ImGui::InputText("Time", settings->uTimeName.data(), 25);
			ImGui::Separator();

			ImGui::Text("Shader");
			ImGui::Checkbox("Hot reload", &settings->hotReload);
			ImGui::Checkbox("Auto saving", &settings->autoSaving);

			if (settings->autoSaving)
			{
				auto file = (settings->tempFilepath == "" ? "<default file>" : settings->tempFilepath.c_str());
				ImGui::Text("%s", file);
				if (ImGui::Button("Select a file"))
					settings->tempFilepath = FileDialogs::OpenFile("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0");
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
					settings->tempFilepath = "";
			}
		}

		ImGui::EndPopup();
	}
}

ImVec4 GetColorFromSeverity(Severity severity)
{
	switch (severity)
	{
	case Severity::Trace:
		return ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
	case Severity::Info:
		return ImVec4{ 0.258f, 0.815f, 1.0f, 1.0f };
	case Severity::Success:
		return ImVec4{ 0.258f, 1.0f, 0.258f, 1.0f };
	case Severity::Warn:
		return ImVec4{ 1.0f, 0.815f, 0.258f, 1.0f };
	case Severity::Error:
		return ImVec4{ 1.0f, 0.258f, 0.258f, 1.0f };
	case Severity::Fatal:
		return ImVec4{ 0.901f, 0.0f, 0.0f, 1.0f };
	}

	return {};
}

void UpdateChangesPanel::DrawUI()
{
	if (m_Active && !ImGui::IsPopupOpen("Save changes?"))
		ImGui::OpenPopup("Save changes?");

	if (ImGui::BeginPopupModal("Save changes?", &m_Active, m_WindowFlags))
	{
		ImGui::SetWindowSize({ 650, 95 });
		
		ImGui::TextWrapped("It looks like you haven't saved your file. Would you like to update changes before opening a new file?");
		ImGui::Separator();
		if (ImGui::Button("Yes"))
		{
			auto& applicationInstance = Application::Get();
			auto shaderEditorPanel = GetPanel<ShaderEditorPanel>("Shader Editor");

			auto fragSource = shaderEditorPanel->GetFragSource();
			if (shaderEditorPanel->GetFilename() == "untitled.frag")
			{
				auto result = FileDialogs::SaveFileAs("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0");
				shaderEditorPanel->SetFilename(result == "" ? "untitled.frag" : result);
				if (result != "")
					applicationInstance.SaveFile(fragSource, shaderEditorPanel->GetFilename());
			}
			else
				applicationInstance.SaveFile(shaderEditorPanel->GetFragSource());

			switch (m_Action)
			{
			case 0:
				applicationInstance.NewFile();
				break;
			case 1:
				applicationInstance.OpenFile(FileDialogs::OpenFile("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0"));
				break;
			case 2:
				applicationInstance.CloseFile();
				break;
			}

			m_Active = false;
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			auto& applicationInstance = Application::Get();
			
			switch (m_Action)
			{
			case 0:
				applicationInstance.NewFile();
				break;
			case 1:
				applicationInstance.OpenFile(FileDialogs::OpenFile("Fragment Shader (*.frag)\0 * .frag\0Fragment Shader(*.fragment)\0 * .fragment\0"));
				break;
			case 2:
				applicationInstance.CloseFile();
				break;
			}

			m_Active = false;
		}

		ImGui::EndPopup();
	}
}

void RestoreFilePanel::DrawUI()
{
	auto settings = Application::Get().GetSettings();

	if (m_Active && !ImGui::IsPopupOpen("Restore File"))
		ImGui::OpenPopup("Restore File");

	if (ImGui::BeginPopupModal("Restore File", &m_Active, m_WindowFlags))
	{
		ImGui::SetWindowSize({ 650, 120 });

		ImGui::TextWrapped("The last time you closed the software, you didn't save your last active file. Would you like to restore it?");
		ImGui::Separator();
		if (ImGui::Button("Yes"))
		{
			Application::Get().RestoreFile();
			std::filesystem::remove(settings->tempFilepath == "" ? "tempFile.tmp" : settings->tempFilepath);
			m_Active = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			std::filesystem::remove(settings->tempFilepath == "" ? "tempFile.tmp" : settings->tempFilepath);
			m_Active = false;
		}

		ImGui::EndPopup();
	}
}

void AboutPanel::DrawUI()
{
	if (m_Active && !ImGui::IsPopupOpen("About"))
		ImGui::OpenPopup("About");

	if (ImGui::BeginPopupModal("About", &m_Active, m_WindowFlags))
	{
		ImGui::SetWindowSize({ 400, 200 });
		ImGui::TextWrapped("zxShaderViz - v1.0.0 Alpha Release\n\nThanks for downloading!\nMade by ZeXo Entertainment.\n\nDevelopers:\nCiridev\nNotManyIdeasDev\nmeepu-m0rpu");

		ImGui::EndPopup();
	}
}

void SolutionWizardPanel::DrawUI()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Wizard", nullptr, m_WindowFlags);
	ImGui::BeginChild("Wizard Buttons");

	int i = ImGui::GetWindowSize().x;
	ImGui::SetCursorPosY(80.0f);

	ImGui::Spacing();
	ImGui::SameLine(i / 2 - 250);
	if (ImGui::Button("Open zxShaderViz without any solution.", { 500, 100 })) m_Mode = 0;
	ImGui::Spacing();
	ImGui::SameLine(i / 2 - 250);
	if (ImGui::Button("Create a new zxShaderViz solution", { 500, 100 })) m_Mode = 1;
	ImGui::Spacing();
	ImGui::SameLine(i / 2 - 250);
	if (ImGui::Button("Open an existing zxShaderViz solution", { 500, 100 })) m_Mode = 2;

	ImGui::EndChild();
	ImGui::End();

	ImGui::PopStyleVar(3);
}
