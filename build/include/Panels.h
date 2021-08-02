#pragma once

#include <FrameBuffer.h>
#include <Renderer.h>

#include "Events/Event.h"

class UIPanel
{
public:
	virtual void DrawUI() = 0;
	virtual void OnEvent(Event& e) = 0;

	std::string GetName() const { return m_Name; }

	bool IsActive() const { return m_Active; }
	void Activate(bool state) { m_Active = state; }

protected:
	UIPanel(const std::string& name, bool active = true) : m_Name(name), m_Active(active) 
	{ m_WindowFlags = ImGuiWindowFlags_NoCollapse; }

	std::string m_Name;
	bool m_Active;

	ImGuiWindowFlags m_WindowFlags;
};

class MenuBarPanel : public UIPanel
{
public:
	MenuBarPanel(const std::string& name) : UIPanel(name)
	{
		m_WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		m_WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		m_WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	virtual void DrawUI() override;
	virtual void OnEvent(Event& e);
};

class ViewportPanel : public UIPanel
{
public:
	ViewportPanel(const std::string& name) : UIPanel(name) {}

	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}

	void SetFrameBuffer(std::shared_ptr<FrameBuffer> fb) { m_FrameBuffer = fb; }
	glm::vec2 GetMousePos() const { return m_MousePos; }
	glm::vec2 GetViewportSize() const { return m_ViewportSize; }

private:
	glm::vec2 m_ViewportSize = { 0, 0 };
	glm::vec2 m_MousePos = { 0,  0 };
	std::shared_ptr<FrameBuffer> m_FrameBuffer;
};

class ShaderEditorPanel : public UIPanel
{
public:
	ShaderEditorPanel(const std::string& name) : UIPanel(name), m_Saved(true), m_Filename("") {}
	
	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}

	void Save(bool save = true) { m_Saved = save; }
	bool IsSaved() const { return m_Saved; }

	void SetFragSource(std::string src) { m_FragmentSrc = src; }
	std::string GetFragSource() const { return m_FragmentSrc; }

	void SetFilename(std::string filename) { m_Filename = filename; }
	std::string GetFilename() const { return m_Filename; }

private:
	bool m_Saved;

	std::string m_FragmentSrc;
	std::string m_Filename;
};


class DemoPanel : public UIPanel
{
public:
	DemoPanel(const std::string& name) : UIPanel(name) { }
	
	virtual void DrawUI() override { ImGui::ShowDemoWindow(); }
	virtual void OnEvent(Event& e) {}
};

enum class Severity
{
	Trace,
	Info,
	Success,
	Warn,
	Error,
	Fatal
};

static ImVec4 GetColorFromSeverity(Severity severity);

class LogPanel : public UIPanel
{
public: 
	LogPanel(const std::string& name) : UIPanel(name), m_LogMsgs({}) 
	{ m_WindowFlags |= ImGuiWindowFlags_AlwaysAutoResize; }

	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}

	void PushMessage(Severity severity, const std::string& msg);

private:
	std::vector<std::pair<std::string, Severity>> m_LogMsgs;
};

class EditorPreferencesPanel : public UIPanel
{
public:
	EditorPreferencesPanel(const std::string& name) : UIPanel(name, false)
	{
		m_WindowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;
	}
	
	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}
};

class UpdateChangesPanel : public UIPanel
{
public:
	UpdateChangesPanel(const std::string& name) : UIPanel(name, false)
	{
		m_WindowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;
	}

	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}
	
	void Activate(bool state, int action) { m_Active = state; m_Action = action; }

private:
	int m_Action = 0;
};

class RestoreFilePanel : public UIPanel
{
public:
	RestoreFilePanel(const std::string& name) : UIPanel(name, true)
	{
		m_WindowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;
	}

	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}

	void Activate(bool state, int action) { m_Active = state; m_Action = action; }

private:
	int m_Action = 0;
};

class AboutPanel : public UIPanel
{
public: 
	AboutPanel(const std::string& name) : UIPanel(name, false)
	{
		m_WindowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking;
	}

	virtual void DrawUI() override;
	virtual void OnEvent(Event& e) {}

};

class SolutionWizardPanel : public UIPanel
{
public: 
	SolutionWizardPanel(const std::string& name) : UIPanel(name)
	{
		m_WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		m_WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		m_WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		m_Mode = -1;
	}

	int GetSelectedMode() const { return m_Mode; }

	virtual void DrawUI() override; 
	virtual void OnEvent(Event& e) {}

private:
	int m_Mode;
};