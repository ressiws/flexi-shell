#pragma once
#ifndef	COMMON_H
#define COMMON_H

// stl
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iomanip>

// libs
#ifdef _WIN32
#include <windows.h> // for Windows
#elif defined __linux__
#include <unistd.h> // for Linux and macOS
#endif

// commands
#include "functions/general/echo.h"
#include "functions/general/clear.h"

#include "functions/info/help.h"

#endif // include guard