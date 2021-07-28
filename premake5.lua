workspace "zxShaderViz"
  architecture "x64"
  configurations { "Debug", "Release" }
  startproject "zxShaderViz"

  outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    
  ExtLibs = {}
  ExtLibs["Glad"]  = "build/ThirdParty/Glad/include"
  ExtLibs["GLFW"]  = "build/ThirdParty/GLFW/include"
  ExtLibs["Yaml"]  = "build/ThirdParty/YAML/include"
  ExtLibs["ImGui"] = "build/ThirdParty/ImGui"

  IncludeDirectories = {}
  IncludeDirectories["glm"] = "build/ThirdParty/glm"
  
  include "build/ThirdParty/Glad"
  include "build/ThirdParty/GLFW"
  include "build/ThirdParty/ImGui"
  include "build/ThirdParty/YAML"
  
  project "zxShaderViz"
    location "build"
    language "C++"
    cppdialect "C++17"

    targetdir ( "bin/".. outputdir .. "/%{prj.name}" )
    objdir    ( "bin/intermediates/" .. outputdir .. "/%{prj.name}" )

    files
    { 
      "build/src/**.cpp", 
      "build/include/**.h",
      "%{IncludeDirectories.glm}/glm/**.hpp",
    }

    includedirs 
    {
      "build/src", 
      "build/include",
      "%{IncludeDirectories.glm}",
      "%{ExtLibs.Glad}", 
      "%{ExtLibs.GLFW}", 
      "%{ExtLibs.ImGui}",
      "%{ExtLibs.Yaml}"
    }

    links { "Glad", "GLFW", "ImGui", "Yaml-cpp", "opengl32.lib" }

    pchheader "zxpch.h"
    pchsource "build/src/zxpch.cpp"

    filter "system:windows" 
      staticruntime "On"
      systemversion "latest"
      system "windows"

      defines { "ZX_WIN" }
    
    filter "system:linux"
      staticruntime "On"
      system "linux"
      defines {"ZX_LINUX"}

    filter { "configurations:Debug" }
      defines { "DEBUG" }
      symbols "On"
      kind "ConsoleApp"

    filter { "configurations:Release" }
      defines { "ZX_RELEASE", "NDEBUG" }
      optimize "On"
      kind "WindowedApp"

