#include "zxpch.h"

#include "Utils/Input.h"
#include "Application.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Input
{
    bool GetKeyPress(Key key)
    {
        auto window = Application::Get().GetActiveWindow()->GetNativeWindow();
        bool state = glfwGetKey(window, (int)key);
        return state == GLFW_PRESS;
    }

    bool GetKeyRelease(Key key)
    {
        auto window = Application::Get().GetActiveWindow()->GetNativeWindow();
        bool state = glfwGetKey(window, (int)key);
        return state == GLFW_RELEASE;
    }

    bool GetMouseButtonPress(Key button)
    {
        auto window = Application::Get().GetActiveWindow()->GetNativeWindow();
        bool state = glfwGetMouseButton(window, (int)button);
        return state == GLFW_PRESS;
    }

    bool GetMouseButtonRelease(Key button)
    {
        auto window = Application::Get().GetActiveWindow()->GetNativeWindow();
        bool state = glfwGetMouseButton(window, (int)button);
        return state == GLFW_RELEASE;
    }
}