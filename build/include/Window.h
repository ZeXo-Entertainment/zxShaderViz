#pragma once

#include "Events/Event.h"

struct GLFWwindow;
using EventProcedure = std::function<void(Event&)>;

enum class RefreshRate
{
	Free,
	VSync
};

struct WindowSettings
{
	int windowSizex;
	int windowSizey;
	int windowPosx;
	int windowPosy;

	RefreshRate refreshRate;
};

class Window
{
public:
	Window
	(
		WindowSettings settings,
		std::string title = "zxShaderViz"
	) 
	{
		m_Data.m_Settings = settings;
		m_Data.m_Title = title;

		Init(settings);
	}

	~Window() = default;

	uint32_t	 GetWidth()			const { return m_Data.m_Settings.windowSizex; }
	uint32_t	 GetHeight()		const { return m_Data.m_Settings.windowSizey; }
	
	uint32_t	 GetPosX()		const { return m_Data.m_Settings.windowPosx; }
	uint32_t	 GetPosY()		const { return m_Data.m_Settings.windowPosy; }


	uint32_t	 GetCurrPosX();
	uint32_t	 GetCurrPosY();

	GLFWwindow*  GetNativeWindow()  const { return m_Window; }
	std::string  GetTitle()			const { return m_Data.m_Title; }
	RefreshRate  GetRefreshRate()	const { return m_Data.m_Settings.refreshRate; }

	bool		 IsVsync()		 const { return m_Data.m_Settings.refreshRate == RefreshRate::VSync; }
	bool		 IsActive()		 const { return m_Data.m_IsActive; }

	void SetRefreshRate(RefreshRate rrate);
	void SetTitle(std::string title);

	void SetWindowLimits(float minX = -1, float minY = -1, float maxX = -1, float maxY = -1) const;

	void SetEventCallbackProcedure(const EventProcedure& eventProcedure);

	void Update();
	void Clear();
	void Close();

private:
	void Init(WindowSettings settings);

	GLFWwindow* m_Window;
	
	struct WindowData
	{
		WindowSettings m_Settings = {};

		bool m_IsActive = false;
		std::string m_Title;
		EventProcedure m_Procedure;
	};

	WindowData m_Data;
};
