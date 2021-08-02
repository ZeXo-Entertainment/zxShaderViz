#include "zxpch.h"

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

#include "Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static bool s_glfwInit = false;
static unsigned int s_KeyRepeats = 0;

void Window::SetEventCallbackProcedure(const EventProcedure& eventProcedure)
{
	m_Data.m_Procedure = eventProcedure;
}

void Window::Update()
{
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

void Window::Clear()
{
	glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Window::Close()
{
	glfwDestroyWindow(m_Window);
	glfwTerminate();
	s_glfwInit = false;
	m_Data.m_IsActive = false;
}

unsigned int Window::GetCurrPosX()
{
	glfwGetWindowPos(m_Window, &m_Data.m_Settings.windowPosx, nullptr);
	return static_cast<unsigned int>(m_Data.m_Settings.windowPosx);
}

uint32_t Window::GetCurrPosY()
{
	glfwGetWindowPos(m_Window, nullptr, &m_Data.m_Settings.windowPosy);
	return static_cast<unsigned int>(m_Data.m_Settings.windowPosy);
}

void Window::SetRefreshRate(RefreshRate rrate)
{
	m_Data.m_Settings.refreshRate = rrate;
	glfwSwapInterval(static_cast<int>(m_Data.m_Settings.refreshRate));
}

void Window::SetTitle(std::string title)
{
	m_Data.m_Title = title;
	glfwSetWindowTitle(m_Window, m_Data.m_Title.c_str());
}

void Window::SetWindowLimits(float minX, float minY, float maxX , float maxY) const
{
	glfwSetWindowSizeLimits(m_Window, minX, minY, maxX, maxY);
}

void Window::Init(WindowSettings settings)
{
	if (!s_glfwInit)
	{
		if (!glfwInit())
		{
			std::cerr << "Couldn't initialize GLFW!" << std::endl;
			return;
		}

		s_glfwInit = true;
	}

	m_Window = glfwCreateWindow(m_Data.m_Settings.windowSizex, m_Data.m_Settings.windowSizey, m_Data.m_Title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(m_Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Couldn't initialize Glad!" << std::endl;
		return;
	}

	glfwSetWindowPos(m_Window, settings.windowPosx, settings.windowPosy);
	glfwSetWindowUserPointer(m_Window, reinterpret_cast<void*>(&m_Data));
	glViewport(0, 0, m_Data.m_Settings.windowSizex, m_Data.m_Settings.windowSizey);

	// Setup Event Handling
	glfwSetKeyCallback(m_Window, [](GLFWwindow* wnd, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(wnd);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressed e(key, s_KeyRepeats);
					data.m_Procedure(e);
					break;
				}
				case GLFW_RELEASE:
				{
					s_KeyRepeats = 0;
					KeyReleased e(key);
					data.m_Procedure(e);
					break;
				}
				case GLFW_REPEAT:
				{
					s_KeyRepeats++;
					KeyPressed e(key, s_KeyRepeats);
					data.m_Procedure(e);
					break;
				}
			}
		});

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* wnd, int width, int height)
		{
			WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));
			data.m_Settings.windowSizex = width;
			data.m_Settings.windowSizey = height;

			WindowResized e(width, height);
			data.m_Procedure(e);
		});

	glfwSetWindowPosCallback(m_Window, [](GLFWwindow* wnd, int posx, int posy) 
	{
			WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));
			data.m_Settings.windowPosx = posx;
			data.m_Settings.windowPosy = posy;

			WindowMoved e(posx, posy);
			data.m_Procedure(e);
	});
	

	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* wnd)
		{
			WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));

			WindowClosed e;
			data.m_Procedure(e);
		});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* wnd, int button, int action, int mods)
		{
			WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressed e(button);
					data.m_Procedure(e);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleased e(button);
					data.m_Procedure(e);
					break;
				}
			}
		});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* wnd, double xoffset, double yoffset)
		{
			WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));

			MouseWheelScrolled e(xoffset, yoffset);
			data.m_Procedure(e);
		});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* wnd, double xPos, double yPos)
		{
			WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));

			MouseMoved e(xPos, yPos);
			data.m_Procedure(e);
		});

	glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* wnd, int width, int height) {
			glViewport(0, 0, width, height);
	});

	glfwSetDropCallback(m_Window, [](GLFWwindow* wnd, int num, const char* files[]) {
		WindowData& data = *reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(wnd));

		FilesDropped e(num, files);
		data.m_Procedure(e);
	});

	m_Data.m_IsActive = true;
}
