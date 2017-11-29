#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

/** Log Class */
class Log
{
public:
	static void Log::AppendToLogFile(std::string text);
	static void Log::AppendToLogFileWithDate(std::string text);
};

