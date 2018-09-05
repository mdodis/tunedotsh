/*
 * Filesystem.h
 */
#pragma one

#include "SoundSystem.h"
#include <json.hpp>
#include <dirent.h>
#include <fstream>

namespace mk
{
    // TODO Keymap
 

    // TODO CONFIG


    namespace PlaylistMgr
    {
        // Loads Playlist JSON file into the json structure
        bool LoadPlaylistFromFile   (const char* path, json* playlist);
        /*
         *  Loads Playlist file from a directory specified
         *  recurse: search all subdirectories for song files
         */
        bool LoadPlaylistFromDir    (const char* dirPath, Playlist& playlist, bool recurse = false);
        // Checks whether or not a playlist JSON file is valid for mk-music
        bool ValidatePlaylist       (const json* jFile);
        // Coneverts only VALID JSON playlist file into a Playlist Class
        bool ToPlaylist             (const json& jFile, Playlist& playlist);

        bool LoadPlaylistDirFromFile(const char* filePath, Playlist& playlist, bool recurse = false);
    }
};
