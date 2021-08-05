#pragma once

#include "Events/Event.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

#include "Solution.h"
#include "Renderer.h"
#include "Window.h"

#include "ImGuiPanel.h"
#include "Panels.h"

template<typename T>
T* GetPanel(const std::string& name);

struct EngineSettings
{
	// Theme settings
	int theme = 0;											// 0 dark 1 light

	// Environment settings
	std::string uResolutionName = "uResolution";			// resolution uniform name
	std::string uMousePositionName = "uMousePosition";		// mouse position uniform name
	std::string uTimeName = "uTime";						// time uniform name

	bool hotReload = true;									// hot reload will Recompile a function on saving
	bool autoSaving = true;									// if autoSaving is true it will save temporary data 
															// to prevent losses in case of closing without saving

	std::string tempFilepath = "";							// the location where the temp file will be stored 
};

class Application
{
public:
	Application();
	~Application() = default;

	void RenderLoop();

	static Application& Get();

	std::shared_ptr<Window> GetActiveWindow() const { return m_ActiveWindow; }
	std::shared_ptr<EngineSettings> GetSettings() const { return m_Settings; }
	std::unordered_map<std::string, UIPanel*> GetPanels() const { return m_PanelsContainer; }

	void NewFile();
	void OpenFile(const std::string& filepath, bool recache = false);
	void SaveFile(std::string fragSource, const std::string& filepath = "");
	void CloseFile();
	void RestoreFile();

	void OpenSolution(const std::string& filepath);
	void WriteToTempFile(const std::string& src);

	void LogMessage(Severity severity, std::string msg);

	void ChangeEditorTheme(int theme);

	bool Init();  
	void Close();

private: 
	void OnEvent(Event&);
	bool OnWindowClosed(WindowClosed&);
	bool OnFilesDropped(FilesDropped&);
	bool OnWindowResized(WindowResized&);

	WindowSettings InitializeSettings(const std::string& filename = "");
	int OpenSelectionWindow();
	void WriteToConfigFile();

	std::shared_ptr<Window> m_ActiveWindow;
	std::shared_ptr<Renderer> m_Renderer;
	std::unique_ptr<ImGuiPanel> m_ImGuiFrame;

	static Application* s_Instance;

	std::unordered_map<std::string, UIPanel*> m_PanelsContainer;

	std::shared_ptr<EngineSettings> m_Settings;
	std::shared_ptr<Solution> m_ActiveSolution;

	bool m_Minimized;
	bool m_Running;
	float lastFrameTime = 0.0f;
};
