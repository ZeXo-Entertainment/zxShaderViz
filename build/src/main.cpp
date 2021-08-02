#include "zxpch.h"

#ifndef ZX_WIN
	#error zxShaderViz only supports Windows!
#else

#include "Application.h"

int main()
{
	auto& engine = Application::Get();
	if (engine.Init())
	{
		engine.RenderLoop();
	}

	return 0;
}

#ifdef ZX_RELEASE
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow)
{
	return main();
}
#endif

#endif