#include "../common.h"

// register commands into the provided vector of commands
void register_commands( std::vector<std::shared_ptr<command>>& commands )
{
    // general
    commands.push_back( std::make_shared<echo_command>( ) );
    commands.push_back( std::make_shared<clear_command>( ) );

    // information
    commands.push_back( std::make_shared<help_command>( commands ) );

    // filesystem
    commands.push_back( std::make_shared<ls_command>( ) );
}

int main( )
{
    std::vector<std::shared_ptr<command>> commands;

    // register available commands
    register_commands( commands );

    // print welcome message
    std::cout << "Welcome to flexi-shell! Type 'help' to view all available commands." << std::endl;

    std::string input;
    while ( true )
    {
        std::cout << std::endl << "flexi-shell> ";

        // read the entire line of input
        std::getline( std::cin, input );

        // parse the input into command name and argument
        std::istringstream iss( input );
        std::string command_name;
        std::string argument;
        iss >> command_name; // read the command name
        std::getline( iss >> std::ws, argument ); // read the remaining input as the argument

        // search for the command by name or aliases
        auto it = std::find_if( commands.begin( ), commands.end( ), [&]( const std::shared_ptr<command>& cmd )
                                {
                                    if ( cmd->name( ) == command_name )
                                        return true;

                                    // check if the command has aliases
                                    for ( const auto& alias : cmd->aliases( ) )
                                    {
                                        if ( alias == command_name )
                                            return true;
                                    }

                                    return false;
                                } );


        if ( it != commands.end( ) )
        {
            // execute the command if found
            ( *it )->execute( argument );
        }
        else
        {
            // print an error message if command not recognized
            std::cout << "Command not recognized. Type 'help' for available commands." << std::endl;
        }
    }

    return 0;
}