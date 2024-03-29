#pragma once
#ifndef	HELP_H
#define HELP_H
#include "../base.h"

class help_command : public command
{
public:
    // constructor: initializes the command name, description, and available commands
    help_command( const std::vector<std::shared_ptr<command>>& commands )
    {
        name_ = "help";
        description_ = "displays help information on available commands.";
        category_ = "information";
        commands_ = commands;
    }

    // execute the command
    virtual void execute( const std::string& argument ) override
    {
        // create a string stream for the argument
        std::istringstream iss( argument );

        // extract the command name (first token)
        std::string command_name;
        iss >> std::ws >> command_name;

        if ( command_name.empty( ) )
        {
            // if no command name provided, print all available commands
            std::cout << "For more information on a specific command, type 'help *command*'" << std::endl << std::endl;
            for ( const auto& cmd : commands_ )
            {
                std::cout << cmd->name( ) << "\t" << cmd->description( ) << std::endl;
            }
        }
        else
        {
            // Search for the command by name or alias
            auto it = std::find_if( commands_.begin( ), commands_.end( ), [&]( const std::shared_ptr<command>& cmd )
                                    {
                                        // Check if the input matches the command name or any of its aliases
                                        bool match = ( cmd->name( ) == command_name );
                                        if ( !match )
                                        {
                                            for ( const auto& alias : cmd->aliases( ) )
                                            {
                                                if ( alias == command_name )
                                                {
                                                    match = true;
                                                    break;
                                                }
                                            }
                                        }
                                        return match;
                                    } );

            if ( it != commands_.end( ) )
            {
                // if command found, print its information
                std::cout << "Command: " << ( *it )->name( ) << std::endl;
                std::cout << "Description: " << ( *it )->description( ) << std::endl;
                std::cout << "Aliases: ";
                for ( const auto& alias : ( *it )->aliases( ) )
                {
                    std::cout << alias << " ";
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "Command '" << command_name << "' not recognized." << std::endl;
            }
        }
    }

private:
    std::vector<std::shared_ptr<command>> commands_; // store available commands
};
#endif // include guard