#pragma once
#ifndef	FUNCTIONS_H
#define FUNCTIONS_H
#include "../../common.h"

class command
{
public:
    virtual ~command( ) {}
    virtual void execute( const std::string& argument ) = 0;
    std::string name( ) const { return name_; }
    std::string description( ) const { return description_; }

protected:
    std::string name_;
    std::string description_;
};

#endif // include guard