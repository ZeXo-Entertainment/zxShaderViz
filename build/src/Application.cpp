#include "zxpch.h"

#include <yaml-cpp/yaml.h>

#include "Application.h"
#include "FrameBuffer.h"
#include "Utils/TimeStep.h"

template<typename T>
T* GetPanel(const std::string& name)
{
	auto panel = Application::Get().GetPanels()[name];
	return reinterpret_cast<T*>(panel);
}

template<>
UpdateChangesPanel* GetPanel(const std::string& name)
{
	auto panel = Application::Get().GetPanels()[name];
	return reinterpret_cast<UpdateChangesPanel*>(panel);
}

template<>
EditorPreferencesPanel* GetPanel(const std::string& name)
{
	auto panel = Application::Get().GetPanels()[name];
	return reinterpret_cast<EditorPreferencesPanel*>(panel);
}

template<>
AboutPanel* GetPanel(const std::string& name)
{
	auto panel = Application::Get().GetPanels()[name];
	return reinterpret_cast<AboutPanel*>(panel);
}

Application* Application::s_Instance = nullptr;

Application::Application()
{
	s_Instance = this;
	WindowSettings windowSettings;

	if (std::filesystem::exists("config.cfg"))
	{
		windowSettings = InitializeSettings("config.cfg");
	}
	else
	{
		windowSettings = InitializeSettings();
		WriteToConfigFile();
	}

	m_Window = std::make_shared<Window>(windowSettings);
	m_Window->SetEventCallbackProcedure(BIND_FUNCTION(&Application::OnEvent, this));
	m_ImGuiFrame = std::make_unique<ImGuiPanel>();
	m_Renderer = Renderer::GetRenderer();
	m_Minimized = (m_Window->GetWidth() == 0 && m_Window->GetHeight() == 0);
	m_Running = m_Window->IsActive();

	auto editorPreferences  = new EditorPreferencesPanel("Editor Preferences");
	auto updateChanges		= new UpdateChangesPanel("Update Changes");
	auto shaderEditor		= new ShaderEditorPanel("Shader Editor");
	auto viewport			= new ViewportPanel("Viewport");
	auto menuBar			= new MenuBarPanel("Menu Bar");
	auto logPanel			= new LogPanel("Log Panel");
	auto aboutPanel			= new AboutPanel("About");

	m_PanelsContainer[menuBar->GetName()] = menuBar;
	m_PanelsContainer[editorPreferences->GetName()] = editorPreferences;
	m_PanelsContainer[updateChanges->GetName()] = updateChanges;
	m_PanelsContainer[shaderEditor->GetName()] = shaderEditor;
	m_PanelsContainer[viewport->GetName()] = viewport;
	m_PanelsContainer[logPanel->GetName()] = logPanel;
	m_PanelsContainer[aboutPanel->GetName()] = aboutPanel;

	if (m_Settings->autoSaving)
	{
		if (std::filesystem::exists(m_Settings->tempFilepath == "" ? "tempFile.tmp" : m_Settings->tempFilepath))
		{
			auto restoreFile = new RestoreFilePanel("Restore File");
			m_PanelsContainer[restoreFile->GetName()] = restoreFile;
		}
	}
}

void Application::RenderLoop()
{
	TimeStep ts;
	auto viewport = GetPanel<ViewportPanel>("Viewport");
	auto shaderEditorPanel = GetPanel<ShaderEditorPanel>("Shader Editor");

	while (m_Running)
	{
		m_Window->Update();
		if (m_Minimized || !m_Window->IsActive()) continue;

		m_Window->Clear();
		ts.Update();

		viewport->SetFrameBuffer(m_Renderer->m_FrameBuffer);

		m_ImGuiFrame->Begin();
		for (auto& [k, panel] : m_PanelsContainer)
			if(panel->IsActive())
				panel->DrawUI();
		m_ImGuiFrame->End();

		if (!m_Renderer->m_ActiveShader) continue;

		if(shaderEditorPanel->IsSaved())
			m_Window->SetTitle("zxShaderViz | " + m_Renderer->GetShader()->GetFilepath());

		m_Renderer->m_ActiveShader->SetUniform(m_Settings->uResolutionName, viewport->GetViewportSize());
		m_Renderer->m_ActiveShader->SetUniform(m_Settings->uMousePositionName, viewport->GetMousePos());
		m_Renderer->m_ActiveShader->SetUniform(m_Settings->uTimeName, ts.GetExecutionTimef());	
		m_Renderer->Draw();
	}

	WriteToConfigFile();

	if(shaderEditorPanel->IsSaved())
		std::filesystem::remove(m_Settings->tempFilepath == "" ? "tempFile.tmp" : m_Settings->tempFilepath);

	m_Renderer->DeleteShaderCache();
	m_Window->Close();
}

Application& Application::Get()
{
	if (!s_Instance) new Application();
	return *s_Instance;
}

void Application::NewFile()
{
	auto shaderEditorPanel = GetPanel<ShaderEditorPanel>("Shader Editor");
	shaderEditorPanel->SetFragSource("");
	shaderEditorPanel->SetFilename("untitled.frag");
	shaderEditorPanel->Save(false);
	m_Renderer->m_ActiveShader = nullptr;
	m_Window->SetTitle("zxShaderViz | " + shaderEditorPanel->GetFilename() + '*');
}

void Application::OpenFile(const std::string& filepath, bool recache)
{
	if (auto shader = m_Renderer->LoadShaderFromFile(filepath, recache))
	{
		auto shaderEditorPanel = GetPanel<ShaderEditorPanel>("Shader Editor");

		if (!shaderEditorPanel->IsSaved())
			shaderEditorPanel->Save();

		m_Window->SetTitle("zxShaderViz | " + filepath);
 		shaderEditorPanel->SetFragSource(shader->GetFragmentSource());		
		shaderEditorPanel->SetFilename(filepath);
		m_Renderer->m_FrameBuffer->Invalidate();
	}
}

void Application::SaveFile(std::string fragSource, const std::string& filepath)
{
	std::string path;
	path = filepath == "" ? m_Renderer->m_ActiveShader->GetFilepath() : filepath;

	std::string extension = {};
	for (auto idx = path.find_last_of("."); idx < path.size(); idx++)
		extension += path[idx];

	std::transform(extension.begin(), extension.end(), extension.begin(), [](char c) -> char { return std::tolower(c); });

	ShaderFileType type = GetTypeFromExtension(extension);
	std::fstream file(path, std::fstream::in | std::fstream::out | std::fstream::trunc);

	if (!file)
	{
		std::stringstream msg;
		msg << "Could not open file!\n";

		LogMessage(Severity::Fatal, msg.str());
		return;
	}

	switch (type)
	{
	case ShaderFileType::frag:
		file << fragSource << '\n';
		break;
	default:
		break;
	}

	file.close();
	OpenFile(path, m_Settings->hotReload);
}

void Application::CloseFile()
{
	auto shaderEditorPanel = GetPanel<ShaderEditorPanel>("Shader Editor");
	shaderEditorPanel->SetFilename("");
	m_Renderer->m_ActiveShader = nullptr;
	shaderEditorPanel->Save(true);
	m_Window->SetTitle("zxShaderViz");
}

void Application::RestoreFile()
{
	std::fstream file(m_Settings->tempFilepath == "" ? "tempFile.tmp" : m_Settings->tempFilepath);
	if (!file)
	{
		LogMessage(Severity::Fatal, "Something bad happened, and the file couldn't be restored.");
		return;
	}
	std::string prevFileFilepath;
	std::stringstream buffer;
	file >> prevFileFilepath;
	buffer << file.rdbuf();
	file.close();

	std::fstream prevFile(prevFileFilepath, std::fstream::in | std::fstream::out | std::fstream::trunc);
	if (!prevFile)
	{
		LogMessage(Severity::Fatal, "Something bad happened, and the file couldn't be restored.");
		return;
	}
	prevFile << buffer.str();
	prevFile.close();
	
	LogMessage(Severity::Success, "The file has been correctly restored!");
	OpenFile(prevFileFilepath);
}

void Application::WriteToTempFile(const std::string& src)
{
	std::fstream file(
		m_Settings->tempFilepath == "" ? "tempFile.tmp" : m_Settings->tempFilepath,
		std::fstream::in | std::fstream::out | std::fstream::trunc
	);

	if (!file)
	{
		LogMessage(Severity::Fatal, "Cannot write on tempFile.tmp!");
		return;
	}

	file << src;
}

void Application::LogMessage(Severity severity, std::string msg)
{
	GetPanel<LogPanel>("Log Panel")->PushMessage(severity, msg);
}

void Application::ChangeEditorTheme(int theme)
{
	m_ImGuiFrame->ChangeTheme(theme);
}

void Application::Close()
{
	m_Running = false;
}

void Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Emit<WindowResized>(BIND_FUNCTION(&Application::OnWindowResized, this));
	dispatcher.Emit<WindowClosed>(BIND_FUNCTION(&Application::OnWindowClosed, this));
	dispatcher.Emit<FilesDropped>(BIND_FUNCTION(&Application::OnFilesDropped, this));

	if(!e.IsHandled())
	{
		for (auto& [key, panel] : m_PanelsContainer)
			panel->OnEvent(e);
	}
}

bool Application::OnWindowClosed(WindowClosed& e)
{
	Close();
	return true;
}

bool Application::OnFilesDropped(FilesDropped& e)
{
	for (auto& files : e.GetFiles())
		this->OpenFile(files);

	return true;
}

bool Application::OnWindowResized(WindowResized& e)
{
	m_Minimized = (e.GetWidth() == 0 && e.GetHeigth() == 0);
	return true;
}

WindowSettings Application::InitializeSettings(const std::string& filename)
{
	m_Settings = std::make_shared<EngineSettings>();
	WindowSettings windowSettings = { 1280, 720, 0, 30, (RefreshRate)0 };

	if (filename != "")
	{
		// Deserialize with YAML
		std::fstream file(filename);
		
		if (file)
		{
			std::stringstream fileBuffer;
			fileBuffer << file.rdbuf();
			file.close();

			YAML::Node nodes = YAML::Load(fileBuffer);
			auto engineNode = nodes["Engine"];
			auto windowNode = nodes["Window"];

			// Engine Settings
			if (engineNode)
			{
				auto themeSettings = engineNode["Theme Settings"];
				if (themeSettings)
				{
					m_Settings->theme = themeSettings["Theme"].as<int>();
				}
				auto environmentSettings = engineNode["Environment Settings"];
				if (environmentSettings)
				{
					auto uniformSettings = environmentSettings["Uniforms"];
					if (uniformSettings)
					{
						m_Settings->uResolutionName = uniformSettings["Resolution"].as<std::string>();
						m_Settings->uMousePositionName = uniformSettings["Mouse Position"].as<std::string>();
						m_Settings->uTimeName = uniformSettings["Time"].as<std::string>();
					}

					auto shaderSettings = environmentSettings["Shaders"];
					if (shaderSettings)
					{
						m_Settings->hotReload = shaderSettings["Hot Reload"].as<bool>();
						m_Settings->autoSaving = shaderSettings["Auto Saving"].as<bool>();
						m_Settings->tempFilepath = shaderSettings["Temp Filepath"].as<std::string>();
					}
				}
			}

			// Window Settings
			if (windowNode)
			{
				auto windowSize = windowNode["Window Size"];
				if (windowSize)
				{
					windowSettings.windowSizex = windowSize[0].as<int>();
					windowSettings.windowSizey = windowSize[1].as<int>();
				}

				auto windowPos = windowNode["Window Position"];
				if (windowPos)
				{
					windowSettings.windowPosx = windowPos[0].as<int>();
					windowSettings.windowPosy = windowPos[1].as<int>();
				}

				if (auto refreshRate = windowNode["Refresh Rate"])
				{
					windowSettings.refreshRate = (RefreshRate)refreshRate.as<unsigned int>();
				}
			}

			if(engineNode && windowNode)
				return windowSettings;
		}
	}

	return windowSettings;
}

void Application::WriteToConfigFile()
{
	YAML::Emitter emitter;

	// Config
	{
		emitter << YAML::Comment("zxShaderViz - v1.0.0 Alpha");
		emitter << YAML::BeginMap;

		// Engine settings
		emitter << YAML::Key << "Engine";
		{
			emitter << YAML::BeginMap;

			// Theme settings
			emitter << YAML::Key << "Theme Settings";
			{
				emitter << YAML::BeginMap;
				emitter << YAML::Key << "Theme" << YAML::Value << m_Settings->theme;
				emitter << YAML::EndMap;
			}

			// Environment settings
			emitter << YAML::Key << "Environment Settings";
			{
				emitter << YAML::BeginMap;
				// Uniforms
				emitter << YAML::Key << "Uniforms";
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "Resolution" << YAML::Value << m_Settings->uResolutionName;
					emitter << YAML::Key << "Mouse Position" << YAML::Value << m_Settings->uMousePositionName;
					emitter << YAML::Key << "Time" << YAML::Value << m_Settings->uTimeName;
					emitter << YAML::EndMap;
				}

				// Shaders settings
				emitter << YAML::Key << "Shaders";
				{
					emitter << YAML::BeginMap;
					emitter << YAML::Key << "Hot Reload" << YAML::Value << m_Settings->hotReload;
					emitter << YAML::Key << "Auto Saving" << YAML::Value << m_Settings->autoSaving;
					emitter << YAML::Key << "Temp Filepath" << YAML::Value << m_Settings->tempFilepath;
					emitter << YAML::EndMap;
				}

				emitter << YAML::EndMap;
			}

			emitter << YAML::EndMap;
		}

		// Window settings
		emitter << YAML::Key << "Window";
		{
			std::vector<int> wSize = { 1280, 720 };
			std::vector<int> wPos = { 0, 30 };
			RefreshRate rr = RefreshRate::Free;

			if (m_Window)
			{
				wSize[0] = m_Window->GetWidth();
				wSize[1] = m_Window->GetHeight();

				wPos[0] = m_Window->GetPosX();
				wPos[1] = std::max(m_Window->GetPosY(), 30u);

				rr = m_Window->GetRefreshRate();
			}

			emitter << YAML::BeginMap;
			emitter << YAML::Key << "Window Size" << YAML::Value << wSize;
			emitter << YAML::Key << "Window Position" << YAML::Value << wPos;
			emitter << YAML::Key << "Refresh Rate" << YAML::Value << (unsigned int)rr;
			emitter << YAML::EndMap;
		}
	
		emitter << YAML::EndMap;
	}

	std::fstream file("config.cfg", std::fstream::in | std::fstream::out | std::fstream::trunc);
	file << emitter.c_str();
	file.close();
}
