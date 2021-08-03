#include "zxpch.h"

#include "Utils/FileDialogs.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace FileDialogs
{
	std::string OpenFile(const char* filter, std::shared_ptr<Window> wHandle)
	{
		

		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };
		
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);

		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)wHandle->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}
	
	std::string SaveFileAs(const char* filter, std::shared_ptr<Window> wHandle)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };
		CHAR currentDir[256] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);

		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)wHandle->GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);

		if (GetCurrentDirectoryA(256, currentDir))
			ofn.lpstrInitialDir = currentDir;
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		// Sets the default extension by extracting it from the filter
		ofn.lpstrDefExt = strchr(filter, '\0') + 1;

		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string BrowseForFolder(std::shared_ptr<Window> wHandle, std::string title, std::string folder)
	{
		std::string ret = std::string();

		BROWSEINFOA br;
		ZeroMemory(&br, sizeof(BROWSEINFOA));
		br.lpfn = [](HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) -> int
		{
			if (uMsg == BFFM_INITIALIZED) SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
			return 0;
		};

		br.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		br.hwndOwner = glfwGetWin32Window((GLFWwindow*)wHandle->GetNativeWindow());
		br.lpszTitle = title.c_str();
		br.lParam = (LPARAM)folder.c_str();

		LPITEMIDLIST pidl = NULL;
		if ((pidl = SHBrowseForFolderA(&br)) != NULL)
		{
			char buffer[MAX_PATH];
			if (SHGetPathFromIDListA(pidl, buffer)) ret = buffer;
		}

		return ret;
	}

}