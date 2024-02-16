#pragma once
#ifndef	LS_H
#define LS_H
#include "../base.h"
namespace fs = std::filesystem;

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

        bool long_format = ( argument.find( 'l' ) != std::string::npos ); 
        bool show_hidden = ( argument.find( 'a' ) != std::string::npos ); 
        bool reverse_order = ( argument.find( 'r' ) != std::string::npos ); 
        bool sort_by_time = ( argument.find( 't' ) != std::string::npos ); 
        bool sort_by_size = ( argument.find( 'S' ) != std::string::npos ); 
        bool sort_by_extension = ( argument.find( 'X' ) != std::string::npos ); 
        bool human_readable = ( argument.find( 'h' ) != std::string::npos ); 
        bool show_inode = ( argument.find( 'i' ) != std::string::npos ); 
        bool list_directories = ( argument.find( 'd' ) != std::string::npos ); 
        bool list_subdirectories = ( argument.find( 'R' ) != std::string::npos ); 
        bool append_indicator = ( argument.find( 'F' ) != std::string::npos ); 
        int depth = -1; // default depth (unlimited)

        std::cout << std::left;
        for ( const auto& entry : entries )
        {
            if ( reverse_order )
            {
                std::reverse( entries.begin( ), entries.end( ) );
            }

            if ( sort_by_time )
            {
                std::sort( entries.begin( ), entries.end( ), []( const auto& a, const auto& b )
                           {
                               return fs::last_write_time( a.path( ) ) > fs::last_write_time( b.path( ) );
                           } );
            }

            if ( sort_by_size )
            {
                std::sort( entries.begin( ), entries.end( ), []( const auto& a, const auto& b )
                           {
                               return fs::file_size( a.path( ) ) > fs::file_size( b.path( ) );
                           } );
            }
            
            if ( sort_by_extension )
            {
                std::sort( entries.begin( ), entries.end( ), []( const auto& a, const auto& b )
                           {
                               auto extension_a = a.path( ).extension( );
                               auto extension_b = b.path( ).extension( );
                               return extension_a < extension_b;
                           } );
            }

            if ( !show_hidden && entry.path( ).filename( ).string( ).front( ) == '.' )
            {
                continue; // skip hidden files if not requested
            }

            if ( long_format )
            {
                std::cout << std::setw( 10 ) << get_permissions( entry ) << " ";
                if ( show_inode )
                {
                    get_inode( entry.path( ) );
                }
                std::cout << std::setw( 10 ) << get_num_links( entry ) << " ";
                std::cout << std::setw( 10 ) << get_owner( entry ) << " ";
                std::cout << std::setw( 10 ) << get_group( entry ) << " ";
                std::cout << std::setw( 10 ) << get_size( entry, human_readable ) << " ";
                std::cout << std::setw( 20 ) << get_last_modified( entry ) << " ";
            }

            std::cout << get_filename( entry );

            if ( append_indicator )
            {
                if ( fs::is_directory( entry.path( ) ) )
                {
                    std::cout << "/";
                }
                else if ( fs::is_symlink( entry ) )
                {
                    std::cout << "@";
                }
                else if ( ( entry.status( ).permissions( ) & fs::perms::owner_exec ) != fs::perms::none ||
                          ( entry.status( ).permissions( ) & fs::perms::group_exec ) != fs::perms::none ||
                          ( entry.status( ).permissions( ) & fs::perms::others_exec ) != fs::perms::none )
                {
                    std::cout << "*";
                }
            }

            std::cout << std::endl;

            if ( list_directories && fs::is_directory( entry.path( ) ) )
            {
                std::cout << entry.path( ).string( ) << std::endl;
            }

            if ( list_subdirectories )
            {
                size_t depth_pos = argument.find( "-d" );

                if ( depth_pos + 3 < argument.size( ) )
                {
                    depth = std::stoi( argument.substr( depth_pos + 3 ) );
                }

                get_subdirectories( entry, depth - 1 );
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

    std::string get_size( const fs::directory_entry& entry, bool human_readable )
    {
        std::ostringstream oss;
        auto size = fs::file_size( entry.path( ) );

        if ( human_readable )
        {
            // convert size to human-readable format
            const char* suffixes[] = { "B", "KB", "MB", "GB", "TB" };
            int i = 0;
            while ( size > 1024 && i < 4 )
            {
                size /= 1024;
                ++i;
            }
            oss << size << suffixes[i];
        }
        else
        {
            oss << size;
        }

        return oss.str( );
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

    std::size_t get_inode( const fs::path& path )
    {
        std::hash<std::string> hasher;
        return hasher( path.string( ) );
    }

    std::string get_filename( const fs::directory_entry& entry )
    {
        std::string name = entry.path( ).filename( ).string( );
        if ( fs::is_directory( entry.path( ) ) )
        {
            name += "/";
        }
        return name;
    }

    void get_subdirectories( const fs::path& current_path, int depth )
    {
        // limit recursion depth
        if ( depth <= 0 ) 
            return; 

        for ( const auto& entry : fs::directory_iterator( current_path ) )
        {
            std::cout << entry.path( ).filename( ).string( ) << std::endl;

            // recursively list subdirectories if the entry is a directory
            if ( fs::is_directory( entry ) )
            {
                get_subdirectories( entry, depth - 1 );
            }
        }
    }
};

#endif // include guard