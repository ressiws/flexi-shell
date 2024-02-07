#include "../common.h"

void register_commands( std::vector<std::shared_ptr<command>>& commands )
{
    commands.push_back( std::make_shared<echo_command>( ) );
}

int main( )
{
    std::vector<std::shared_ptr<command>> commands;
    register_commands( commands );

    std::cout << "Welcome to flexi-shell!\n" << std::endl;

    std::string input;
    while ( true )
    {
        std::cout << "flexi-shell> ";
        std::cin >> input;

        std::string argument;
        std::getline( std::cin, argument );

        auto it = find_if( commands.begin( ), commands.end( ), [&]( const std::shared_ptr<Command>& cmd ) { return cmd->name( ) == input; } );
        if ( it != commands.end( ) )
        {
            ( *it )->execute( argument );
        }
        else
        {
            std::cout << "Command not recognized." << std::endl;
        }
    }

    return 0;
}