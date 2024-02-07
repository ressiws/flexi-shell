#pragma once
#ifndef	HELP_H
#define HELP_H
#include "../base.h"

class help_command : public command
{
public:
    help_command( const std::vector<std::shared_ptr<command>>& commands )
    {
        name_ = "help";
        description_ = "displays help information on available commands";
        commands_ = commands;
    }

    virtual void execute( const std::string& argument ) override
    {
        std::cout << "for more information on a specific command, type 'help *command*'" << std::endl;
        for ( const auto& cmd : commands_ )
        {
            std::cout << cmd->name( ) << " : " << cmd->description( ) << std::endl;
        }
    }

private:
    std::vector<std::shared_ptr<command>> commands_;
};

#endif // include guard