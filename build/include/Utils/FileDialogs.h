#pragma once

#include "Application.h"

namespace FileDialogs
{
	std::string OpenFile(const char* filter, std::shared_ptr<Window> wHandle = Application::Get().GetWindow());
	std::string SaveFileAs(const char* filter, std::shared_ptr<Window> wHandle = Application::Get().GetWindow());

	std::string BrowseForFolder(std::shared_ptr<Window> wHandle = Application::Get().GetWindow(), std::string title = "Select a folder", std::string folder = "C:\\");
}
