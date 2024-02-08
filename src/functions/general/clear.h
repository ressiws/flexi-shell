#pragma once
#ifndef CLEAR_H
#define CLEAR_H
#include "../base.h"

class clear_command : public command
{
public:
	// constructor: initializes the command name and description
	clear_command( )
	{
		name_ = "clear";
		aliases_ = { "cls" };
		description_ = "clear the console screen";
		category_ = "general";
	}

	// executes the command
	virtual void execute( const std::string& argument ) override
	{
		// clear the console screen using platform-specific commands
#ifdef _WIN32
		std::system( "cls" ); // for Windows
#else
		std::system( "clear" ); // for macOS and Linux
#endif
	}
};

#endif // include guard