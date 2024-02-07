#pragma once
#ifndef	FUNCTIONS_H
#define FUNCTIONS_H
#include "../../common.h"

class command
{
public:
    virtual ~command( ) {}
    virtual void execute( const std::string& argument ) = 0;
};

#endif // include guard