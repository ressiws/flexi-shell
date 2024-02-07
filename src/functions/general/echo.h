#pragma once
#ifndef	ECHO_H
#define ECHO_H
#include "../base.h"

class echo_command : public command
{
public:
    virtual void execute( const std::string& argument ) override
    {
        std::cout << argument << std::endl;
    }
};

#endif // include guard