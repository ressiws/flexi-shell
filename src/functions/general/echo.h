#pragma once
#ifndef	ECHO_H
#define ECHO_H
#include "../base.h"

class echo_command : public command
{
public:
    echo_command( )
    {
        name_ = "echo";
        description_ = "echoes back the provided message";
    }

    virtual void execute( const std::string& argument ) override
    {
        std::cout << argument << std::endl;
    }
};

#endif // include guard