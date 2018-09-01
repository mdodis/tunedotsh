#include "Filesystem.h"

namespace mk
{

    namespace PlaylistMgr{

        bool FindInStr(std::string s, std::string expr)
        {
            return ( (s.find(expr, 0)) <= s.size() - 1 );
        }
                
        // Checks if filename contains valid extension
        MKFormat CheckExtension(std::string file)
        {
            if(file.find(".mp3", 0) <= file.size() - 1)
                return MKFormat::MPEG;
            else if(file.find(".ogg",0) <= file.size() - 1)
                return MKFormat::VORBIS;
            else
                return MKFormat::UNSUPPORTED;
        }


        bool LoadPlaylistFromFile(const char* path, json* playlist)
        {
            std::ifstream file(path);
            if(!file.is_open())
            {
                // failed to open
                return false;
            }
            
            try
            {
                *playlist = json::parse(file);
            }
            catch(json::parse_error& e)
            {
                std::cerr << e.what() << '\n';
                return false;
            }
            return true;
        }

        bool ValidatePlaylist(const json* jFile)
        {
            if(jFile == nullptr) return false;
            if(jFile->find("queue") == jFile->end())
            {
                return false;
            }
            if(!jFile->at("queue").is_array())
            {
                return false;
            }
            return true;
        }

        bool ToPlaylist(const json& jFile, Playlist& playlist)
        {
            // playlist.files.clear();
            playlist.clear();
            json queue = jFile.at("queue");
            for(auto it = queue.begin(); it != queue.end(); it++)
            {
                std::string data = *it;
                // playlist.files.push_back(data);
                SongInfo* info = new SongInfo;
                info->fileName = data;
                playlist.songs.push_back(info);
            }

            return true;
        }

        void AddDir(const char* dirPath, Playlist& playlist, bool recurse)
        {
            if(dirPath == nullptr)
                return;
            dirent* drnt;
            DIR* dir = opendir(dirPath);
            if (dir == nullptr) return;
            while ( (drnt = readdir(dir)) != NULL )
            {
                if(drnt->d_type == DT_REG)
                {
                    std::string fileName(drnt->d_name);
                    size_t pos;
                    pos = fileName.find(".mp3", 0);
                    if(pos <= fileName.size() -1 )
                    {
                        // Found mp3

                        // Should check for permissions
                        std::string path(dirPath);
                        const char* str = path.c_str();
                        if (str[path.length() - 1] != '/')
                            path += '/';
                        path += fileName;
                        // playlist.files.push_back(path);
                        SongInfo* s = new SongInfo;
                        s->filePath = path;
                        s->fileName = fileName;
                        playlist.songs.push_back(s);

                        // std::cout << fileName << '\n';
                    }
                }
                else if(drnt->d_type == DT_DIR && recurse)
                {
                    if(strcmp(drnt->d_name,".") == 0)
                    {
                        // std::cout << "DOT\n";
                        continue;
                    }
                    if(strcmp(drnt->d_name,"..") == 0)
                    {
                        // std::cout << "DOTDOT\n";
                        continue;
                    }
                    // std::cout << "DIR\n";
                    std::string path(dirPath);
                    const char* str = path.c_str();
                    if (str[path.length() - 1] != '/')
                        path += '/';
                    path += drnt->d_name;
                    // std::cout << path << '\n';
                    AddDir(path.c_str(), playlist, recurse);
                }
            }
        }

        bool LoadPlaylistFromDir(const char* dirPath, Playlist& playlist, bool recurse)
        {
            // playlist.files.clear();
            if(dirPath == nullptr)
                return false;
            // std::cout << dirPath << '\n';
            // Search dir for .mp3
            dirent* drnt;
            std::cout << "YO\n";

            DIR* dir = opendir(dirPath);
            if (dir == nullptr) return false;
            while ((drnt = readdir(dir)) != NULL)
            {
                if(drnt->d_type == DT_REG){
                    
                    std::string fileName(drnt->d_name);
                    // if ( CheckExtension(drnt->d_name))
                    if(CheckExtension(drnt->d_name) != MKFormat::UNSUPPORTED)
                    {
                        std::string path(dirPath);
                        const char* str = path.c_str();
                        if (str[path.length() - 1] != '/')
                            path += '/';
                        path += fileName;

                        SongInfo* s = new SongInfo;
                        s->filePath = path;
                        s->fileName = fileName;
                        playlist.songs.push_back(s);
                    }
                    // size_t pos;
                    // pos = fileName.find(".mp3", 0);
                    // if(pos <= fileName.size() -1 )
                    // {
                    //     // Found mp3

                    //     // Should check for permissions
                    //     std::string path(dirPath);
                    //     path += '/';
                    //     path += fileName;

                    //     SongInfo* s = new SongInfo;
                    //     s->filePath = path;
                    //     s->fileName = fileName;
                    //     playlist.songs.push_back(s);
                    // }
                }
                else if(drnt->d_type == DT_DIR && recurse)
                {
                    if(strcmp(drnt->d_name,".") == 0)
                    {
                        // std::cout << "DOT\n";
                        continue;
                    }
                    if(strcmp(drnt->d_name,"..") == 0)
                    {
                        // std::cout << "DOTDOT\n";
                        continue;
                    }
                    std::string path(dirPath);
                    const char* str = path.c_str();
                    if (str[path.length() - 1] != '/')
                        path += '/';
                    path += drnt->d_name;
                    std::cout << path << '\n';
                    AddDir(path.c_str(), playlist, recurse);
                }
            }
            return true;
        }


    }

};
