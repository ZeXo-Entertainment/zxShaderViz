#include "zxpch.h"

#include "Utils/FileDialogs.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace FileDialogs
{
    std::string openf(std::string filter) {
        //cant use filter yet so just ignore it for now
        std::string cmd = "zenity --file-selection";
        FILE* pipe = popen(cmd.c_str(), "r"); // open a pipe with zenity
        if (!pipe) return "ERROR"; // if failed then return "ERROR"
        char buffer[912]; // buffer to hold data
        std::string result = ""; // result that you add too

        while(!feof(pipe)) { // while not EOF read
            if(fgets(buffer, 912, pipe) != NULL) // get path and store it into buffer
                result += buffer; // add buffer to result
        }

        //I thought i needed to convert the absolute path to relative but i did not after all
        // char cwd[10000];
        // getcwd(cwd, 10000); // get cwd(current working directory)
        // result = std::filesystem::relative(result, cwd); // convert the absolute path to relative with cwd
        pclose(pipe); // cleanup
        result.erase(result.length() - 1); // remove \n at end of path
        return result;
    }

    std::string readf(std::string filename){
        std::string res;
        std::ifstream file;

        file.open(filename.c_str()); // open file
        if(file.is_open()) { // if it is open add contents to res
            while(file){
                res += file.get();
            }
        }else { // if not then show error
            res = "failed to open file " + filename;
        }
        return res;
    }
	std::string OpenFile(const char* filter)
	{
		std::string file_location = openf(filter);
        return file_location;
	}
	
	std::string SaveFileAs(const char* filter)
	{
		std::string file_location = openf(filter);
        return file_location;
	}

}