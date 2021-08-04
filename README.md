# zxShaderViz

![zxShaderViz Showcase](https://github.com/Ciridev/ShaderPlayground/blob/master/Screenshots/Showcase.png)

## About
zxShaderViz is a useful tool made by our team, which aims to help learning about shader development. Currently zxShaderViz is still under development. However, we have released an Alpha Version to let you test the product and give us your precious feedback. Many features are yet to come, but the essential is in there. We will keep updating the software to implement all the ideas we have in mind. 

> I recently started learning OpenGL and Shader Development so I decided to code up a simple visualizer to help me learning. Then I created this software to help others as well, but also to prove myself and see how far I can go. 

@Ciridev

## Developers
### Active 

Developer | GitHub | Platform  | Version 
------------ | ------------ | ------------ | ------------
| Lorenzo Cirillo | [@Ciridev](https://www.github.com/Ciridev) | Windows | Alpha Release v1.0.0 |
| Giancamillo Alessandroni |  [@NotManyIdeasDev](https://www.github.com/NotManyIdeasDev) | Windows | Alpha Release v1.1.0
| Anthony Sterling-Palmari | [@meep-m0rp](https://github.com/meep-m0rp) | Linux (Arch) | Alpha Release v1.1.0
 
###### We are looking for developers! [Join us!](https://github.com/ZeXo-Entertainment/Join-Us)



## Getting Started 
### Download the latest release...

  Platform | Release  
 ------------ | ------------ 
 Windows  | Download v1.0.0 Alpha 
 Windows  | Source Code 


### ...or compile yourself the project!
Using Visual Studio 2019 is recommended, you will need [Premake](https://premake.github.io/) to generate the solution files needed.
1. Start by cloning the repository with the following command: 
`git clone --recursive https://github.com/ZeXo-Entertainment/zxShaderViz`. 
Or if you cloned the repository non-recusively, you can use this command:
`git submodule update --init` to add all the dependencies.

1. Since we are not storing Premake binaries inside our project, you will need to add them manually (This could change in the future).
	* Start by creating a directory called `"ThirdParty"` and another directory called `"bin"` and put the last created directory inside the first one. 
	* Then copy the Premake binaries inside the `"bin"` folder.
	* Run the `"genprj.bat"` file.

3. Compile the project! 

**NOTE:** If you want to store the binaries in a different folder, make sure to **edit** the `"genprj.bat"` and match the `"premake5.exe"` file with the chosen path!

**NOTE:** The Source Code release already contains every submodule and the Premake binaries, so you will simply have to execute the batch script to generate all the files you need.   

### Dependencies
  Dependency | Website | GitHub |
 ------------ | ------------ | ------------
  GLFW | [glfw.org](https://www.glfw.org) | [glfw/glfw](https://www.github.com/glfw/glfw) 
 Glad | [glad.dav1d.de](https://glad.dav1d.de/) | [Dav1dde/glad](https://github.com/Dav1dde/glad)
Dear ImGui | ------------ | [ocornut/imgui](https://www.github.com/ocornut/imgui) 
 glm |[glm.g-truc.net](https://glm.g-truc.net/0.9.9/index.html) | [g-truc/glm](https://github.com/g-truc/glm) 
YAML|[yaml.org](https://yaml.org/)|[Yaml-cpp](https://github.com/jbeder/yaml-cpp/)

## Building and Platforms
The project uses **[Premake](https://premake.github.io/)** to generate **Visual Studio Solution Files**. 
Currently  **zxShaderViz** only supports **Windows** and does not run on different systems because several core functionalities have no Cross-Platform implementation.
We have planned to **extend** this project to **Linux**, thus we will use CMake as our build system, but currently we have no date set.
Unfortunately, **we will not** provide support for **MacOS** in the short term because we have no way of testing functionalities. If you are a **MacOS developer** and you want to help us with this project, **consider reaching up**.

## License 
Our software is distributed using the MIT License. 

## Contributing 
Your help is precious! 
Currently zxShaderViz is in Alpha so it may contain bugs, code may be unefficient and more. So we highly recommend you to open an Issue or even open a Pull Request if you have a fix for a bug. If you want to share some ideas or discuss about anything related, feel free to open an Issue. 

**NOTE:** If you open a Pull Request, please respect our coding conventions, otherwise the code will result inconsistent and may be harder and messy to read. 
