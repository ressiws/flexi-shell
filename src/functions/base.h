#pragma once
#ifndef	FUNCTIONS_H
#define FUNCTIONS_H
#include "../../common.h"

class command
{
public:
    // ensures proper cleanup in derived classes
    virtual ~command( ) {}

    // executes the command with the provided argument
    virtual void execute( const std::string& argument ) = 0;

    // getter for the command informations
    std::string name( ) const { return name_; }
    std::vector<std::string> aliases( ) const { return aliases_; }
    std::string description( ) const { return description_; }
    std::string category( ) const { return category_; }

protected:
    std::string name_;                 // name of the command
    std::vector<std::string> aliases_; // aliases of the command
    std::string description_;          // description of the commnd
    std::string category_;             // category of the command
};

#endif // include guard