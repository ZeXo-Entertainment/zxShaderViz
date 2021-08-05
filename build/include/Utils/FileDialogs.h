#pragma once

#include "Application.h"

namespace FileDialogs
{
	std::string OpenFile(const char* filter);
	std::string SaveFileAs(const char* filter);

	std::string BrowseForFolder(std::string title = "Select a folder", std::string folder = "C:\\");
}
