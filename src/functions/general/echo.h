#pragma once
#ifndef	ECHO_H
#define ECHO_H
#include "../base.h"

class echo_command : public command
{
public:
    // constructor: initializes the command name and description
    echo_command( )
    {
        name_ = "echo";
        aliases_ = { "say", "print" };
        description_ = "echoes back the provided message.";
        category_ = "general";
    }

    // executes the echo command, printing the provided argument
    virtual void execute( const std::string& argument ) override
    {
        std::cout << argument << std::endl;
    }
};

#endif // include guard