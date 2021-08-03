#include "zxpch.h"
#include "Utils/Utility.h"

namespace Environment
{
	std::string GetHomePath()
	{
		std::string environment = std::getenv("PATH");
		if (environment == "")
			return std::string();

		int idx = environment.find("C:\\Users\\");
		int where;
		for (where = idx + 10; environment[where] != '\\'; where++);

		return std::string(environment.c_str() + idx, (size_t)(where - idx));
	}
}