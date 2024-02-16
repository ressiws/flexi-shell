#pragma once
#ifndef	LS_H
#define LS_H
#include "../base.h"
namespace fs = std::filesystem;

// @todo: add more arguments
class ls_command : public command
{
public:
    // constructor: initializes the command name and description
    ls_command( )
    {
        name_ = "ls";
        description_ = "list directory contents";
        category_ = "filesystem";
    }

    // executes the echo command, printing the provided argument
    virtual void execute( const std::string& argument ) override
    {
        std::string _path;
        if ( !argument.empty( ) && argument.front( ) != '-' )
        {
            _path = argument;
        }
        else
        {
            // use the desktop directory if no path is specified
            char* desktop_path;
            size_t size;
            if ( _dupenv_s( &desktop_path, &size, "USERPROFILE" ) == 0 && desktop_path != nullptr )
            {
                _path = std::string( desktop_path ) + "\\Desktop";
                free( desktop_path );
            }
            else
            {
                std::cerr << "Failed to get desktop directory." << std::endl;
                return;
            }
        }

        // convert to filesystem path
        fs::path path( _path );

        // check if the provided path exists
        if ( !fs::exists( path ) )
        {
            std::cout << "Directory not found: " << path << std::endl;
            return;
        }

        // store directory entries
        std::vector<fs::directory_entry> entries;

        // populate the vector with directory entries
        for ( const auto& entry : fs::directory_iterator( path ) )
        {
            entries.push_back( entry );
        }

        // display entries with extended information if 'l' option is specified
        if ( argument.find( 'l' ) != std::string::npos )
        {
            std::cout << std::left;
            for ( const auto& entry : entries )
            {
                std::cout << std::setw( 10 ) << get_permissions( entry ) << " ";
                std::cout << std::setw( 3 )  << get_num_links( entry ) << " ";
                std::cout << std::setw( 10 ) << get_owner( entry ) << " ";
                std::cout << std::setw( 10 ) << get_group( entry ) << " ";
                std::cout << std::setw( 10 ) << get_size( entry ) << " ";
                std::cout << std::setw( 10 ) << get_last_modified( entry ) << " ";
                std::cout << entry.path( ).filename( ).string( ) << std::endl;
            }
        }
        else
        {
            // display only file/directory names if 'l' option is not specified
            for ( const auto& entry : entries )
            {
                std::cout << entry.path( ).filename( ).string( ) << std::endl;
            }
        }
    }

    std::string get_owner( const fs::directory_entry& entry )
    {
        char username[256];
        DWORD username_len = sizeof( username );
        GetUserNameA( username, &username_len );
        return std::string( username );
    }

    // windows does not have the concept of groups in the same way as linux systems do, so we will get the primary group of the current user
    std::string get_group( const fs::directory_entry& entry )
    {
        char username[256];
        DWORD username_len = sizeof( username );
        GetUserNameA( username, &username_len );
        return std::string( username );
    }

    std::string get_permissions( const fs::directory_entry& entry )
    {
        std::string result;

        // directory indicator
        result += fs::is_directory( entry.path( ) ) ? "d" : "-";

        // permissions
        result += ( entry.status( ).permissions( ) & fs::perms::owner_read ) != fs::perms::none ? "r" : "-";
        result += ( entry.status( ).permissions( ) & fs::perms::owner_write ) != fs::perms::none ? "w" : "-";
        result += ( entry.status( ).permissions( ) & fs::perms::owner_exec ) != fs::perms::none ? "x" : "-";

        // group permission
        result += ( entry.status( ).permissions( ) & fs::perms::group_read ) != fs::perms::none ? "r" : "-";
        result += ( entry.status( ).permissions( ) & fs::perms::group_write ) != fs::perms::none ? "w" : "-";
        result += ( entry.status( ).permissions( ) & fs::perms::group_exec ) != fs::perms::none ? "x" : "-";

        // other permission
        result += ( entry.status( ).permissions( ) & fs::perms::others_read ) != fs::perms::none ? "r" : "-";
        result += ( entry.status( ).permissions( ) & fs::perms::others_write ) != fs::perms::none ? "w" : "-";
        result += ( entry.status( ).permissions( ) & fs::perms::others_exec ) != fs::perms::none ? "x" : "-";

        return result;
    }

    std::string get_size( const fs::directory_entry& entry )
    {
        // fs::file_size( entry.path( ) )

        return std::to_string( fs::file_size( entry.path( ) ) ); // placeholder
    }

    std::string get_last_modified( const fs::directory_entry& entry )
    {
        HANDLE file = CreateFileW( entry.path( ).c_str( ), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( file == INVALID_HANDLE_VALUE )
        {
            return "unknown";
        }

        FILETIME last_write;
        if ( !GetFileTime( file, NULL, NULL, &last_write ) )
        {
            CloseHandle( file );
            return "unknown";
        }

        CloseHandle( file );

        SYSTEMTIME UTC, local;
        FileTimeToSystemTime( &last_write, &UTC );
        SystemTimeToTzSpecificLocalTime( NULL, &UTC, &local );

        std::string month;
        switch ( local.wMonth )
        {
            case 1: month = "Jan"; break;
            case 2: month = "Feb"; break;
            case 3: month = "Mar"; break;
            case 4: month = "Apr"; break;
            case 5: month = "May"; break;
            case 6: month = "Jun"; break;
            case 7: month = "Jul"; break;
            case 8: month = "Aug"; break;
            case 9: month = "Sep"; break;
            case 10: month = "Oct"; break;
            case 11: month = "Nov"; break;
            case 12: month = "Dec"; break;
            default: month = "Unknown"; break;
        }

        char buffer[256];
        sprintf_s( buffer, "%s %02d  %02d::%02d", month.c_str( ), local.wDay, local.wHour, local.wMinute );
        return std::string( buffer );
    }

    int get_num_links( const fs::directory_entry& entry )
    {
        HANDLE file = CreateFileW( entry.path( ).c_str( ), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( file == INVALID_HANDLE_VALUE )
        {
            return -1;
        }

        BY_HANDLE_FILE_INFORMATION file_info;
        if ( !GetFileInformationByHandle( file, &file_info ) )
        {
            CloseHandle( file );
            return -1;
        }

        CloseHandle( file );

        return file_info.nNumberOfLinks;
    }
};

#endif // include guard