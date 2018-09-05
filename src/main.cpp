/*
 *  TODO
 *  ====
 *  -- Add time elapsed / song length  
 *  -- Add Reorder toggle rather than hold
 *  -- In FileSystem.h or w/e add support for config files
 *  -- Create platform abstraction funcs( either compile with or separately as lib)
 */


// #define PLAYLIST_TEST
// #define GUI_TEST
// #define FMOD_TEST

#define MS_TO_MIN 1.6667E-5

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "SoundSystem.h"
#include "UISystem.h"
#include "Filesystem.h"
#include <curses.h>
#include <json.hpp>
#include <locale.h>

#ifdef WIN32

#include <windows.h>
/*
 * Windows does not provide usleep, solution is
 * to define it here
 */

#include <chrono>
#include <thread>

void usleep(__int64 usec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(usec));
}

// Alternate Windows usleep function using win32 API
// void usleep(__int64 usec) 
// { 
//     HANDLE timer; 
//     LARGE_INTEGER ft; 

//     ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

//     timer = CreateWaitableTimer(NULL, TRUE, NULL); 
//     SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
//     WaitForSingleObject(timer, INFINITE); 
//     CloseHandle(timer); 
// }

#else
#include <unistd.h>
#endif

using namespace mk;

using json = nlohmann::json;

enum class MKOption{
    SinglePlay,
    PlaylistFile,
    Directory,
    DirectoryRecursive,
    Unspecified
};

void InitUI()
{
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    curs_set(0);
    nonl();
    cbreak();
    start_color();
    timeout(500);
    keypad(stdscr, true);
    refresh();

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED | COLOR_GREEN, COLOR_BLACK);
}

typedef struct{
    int argc;
    const char ** argv;

    void Args(int argc, char const * argv[])
    {
        this->argc = argc;
        this->argv = argv;
    }

    unsigned short FindFullOption(const char* name) const
    {
        std::string query(name);
        query.insert(0,"--");
        for (unsigned short i = 1; i < argc; ++i)
        {
            std::string option(argv[i]);
            if (option == query)
            {
                return i;
            }
        }
        return 0;
    }

    unsigned short FindShortOption(const char* name) const
    {
        std::string query(name);
        query.insert(0,"-");
        for (unsigned short i = 1; i < argc; ++i)
        {
            std::string option(argv[i]);
            if (option == query)
            {
                return i;
            }
        }
        return 0;
    }
} ProgArgs;

struct MKArgs{
    std::string     path;
    MKOption        option;
};



MKArgs ParseArgs(int argc, char const * argv[])
{
    MKArgs out;
    out.option = MKOption::Unspecified;

    ProgArgs a;
    a.Args(argc, argv);
    unsigned short pos;    
    if ((pos = a.FindShortOption("d")))
    {
        if (pos + 1 < argc) 
        {
            out.option = MKOption::Directory;
            out.path = argv[pos + 1];
        }

    }
    else if ((pos = a.FindShortOption("dr")))
    {
        if (pos + 1 < argc) 
        {
            out.option = MKOption::DirectoryRecursive;
            out.path = argv[pos + 1];
        }
    }
    else if (argc == 2)
    {
        out.option = MKOption::SinglePlay;
        out.path = argv[1];
    }

    return out;
}
// void PopulateList(Playlist& p , UIGenericList* list)
// {
//     if(list == nullptr) return;
//     list->Clear();
//     for(SongInfo* s : p.songs)
//     {
//         list->AddItem(s->fileName);
//     }
// }

#ifdef FMOD_TEST
int main(int argc, char const *argv[])
{
    SoundSystem sSystem;
    sSystem.LoadTrack("music2.ogg");
    FMOD::Sound* s = sSystem.GetSound();

    
    SongDetails* dt = SongDetails::From_ID3V1(s);
    // s->getName(name, 128);

    std::cout << dt->title << '\n';
    std::cout << dt->artist << '\n';
    std::cout << dt->album << '\n';
    std::cout << dt->year << '\n';
    // int numTags;
    // FMOD_TAG tag;

    // s->getNumTags(&numTags,0);
    // s->getTag(0, 0, &tag);

    // std::cout <<  numTags<< '\n';
    // std::cout << tag.name << '\n';
    // std::cout << (tag.datatype == FMOD_TAGDATATYPE_STRING) << '\n';
    // std::cout << (tag.type == FMOD_TAGTYPE_ID3V1) << '\n';
    // std::cout << (char*)tag.data << '\n';


    // sSystem.PlayStream(sound2, &channel1);
    int i;
    std::cin >> i;


    return 0;
}
#elif defined (PLAYLIST_TEST)

int main(int argc, char const *argv[])
{
    MKArgs a = ParseArgs(argc, argv);
    SoundSystem s;
    Playlist p;
    
    if(a.option == MKOption::Directory)
    {
        if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p))
        {
            std::cout << "ERR\n";
            return -1;
        }
    }
    else if(a.option == MKOption::DirectoryRecursive)
    {
        std::cout << "DR\n";
        if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p, true))
        {
            std::cout << "ERR\n";
            return -1;
        }
    }
    else if (a.option == MKOption::SinglePlay)
    {
        std::string file(a.path);
        std::string path;
        size_t pos = file.rfind('/');
        path = file.substr(0, pos + 1);
        if (!PlaylistMgr::LoadPlaylistFromDir(path.c_str(), p))
        {
            std::cout << "ERR\n";
            return -1;
        }
        for (size_t i = 0; i < p.songs.size(); i++)
        {
            if (p.songs[i]->filePath == file && i != 0)
            {
                std::swap(p.songs[0], p.songs[i]);
            }
        }
    }


    
//    if (p.songs.size() == 0)
//        return -1;
//    for(auto& it : p.songs)
//    {
//        std::cout << it->fileName << '\n';
//        std::cout << it->filePath << '\n';
//    }
//
//
//    s.LoadTrack(p.songs[0]->filePath.c_str());
//    
//    s.TrackStart();
    std::cin.get();
}

#elif defined (GUI_TEST)

int main(int argc, char const *argv[])
{
    float length = 5.05f;
    float now = 1.02f;
    InitUI();
    // SelectableList list;
    UIGenericList tracklist(0,0,30,10);
    tracklist.AddItem("A Kul Song 1");
    tracklist.AddItem("Despacito 3");
    tracklist.AddItem("Hello Kitty OST");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.SetFocus(true);

    UIReorderList playlist(0, 0, 30, 10);
    // playlist.Add("YOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYO");
    playlist.AddItem("Song 2");
    playlist.AddItem("Nightwish - Fishmaster");
    playlist.AddItem("Metadeth");
    playlist.AddItem("Greetings professor Falken");
    playlist.AddItem("Song 465");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    playlist.AddItem("YOYOYOYOdd");
    UILine s(std::string("A Kul Song 1"), 0,0);
    refresh();
    // int barSize = 20;
    // float volume = 1.0f;
 
    for(;;)
    {
        int c;
        clear();
        int row, col;
        getmaxyx(stdscr, row, col);
    #ifdef WIN32
        resize_term(row,col);
    #endif

        unsigned int w = col / 2 - 1;
        tracklist.SetW(w + 1);
        playlist.SetW(w);
        playlist.SetX(w + 2);

        playlist.SetH(row - 3);
        tracklist.SetH(row - 3);

        s.y = row-2;
        s.w = col;
        s.Print(row, col);
        // ProgressBar(length, now, row, col, row - 1);
        ProgressBarGeneric(length, now, row, col, 0, row - 1, col);
        tracklist.Print(row, col);
        playlist.Print(row, col);
        // Separator
        // PrintVertSeparator(row, col, 0, row - 4, w + 1, '|');
        PrintHoriSeparator(row, col, 0, col, row - 3, (wint_t)L'═');

        PrintVertSeparator(row, col, 0, row - 4, w + 1, (wint_t)L'║');

        // PrintVertSeparator(row, col, 0, row - 4, col / 2 + 1, '|');
        UISoundPopup::Print(row, col);

        refresh();


        
        usleep(3000);
        // Update
        c = getch();
        if(c == 'q')
            break;
        else if(c == '\t')
        {
            // Switch Focus
            tracklist.SetFocus(playlist.GetFocus());
            playlist.SetFocus(!tracklist.GetFocus());

        }
        else if (c == '>')
        {
            now += 5.f / 60.f;
        }
        else if (c == '<')
        {
            now -= 5.f / 60.f;
        }
        if ( now < 0.f) now = 0.f;
        if (now > length) now = length;
        bool b = playlist.Update(c, row, col);
        bool d = tracklist.Update(c, row, col);
        if(b)
        {
            const UITrackLine* line = playlist.GetSelectedItem();
            s.data = line->data;
            playlist.SetCurrentToSelected();
        }
        UISoundPopup::Update(c, 3000u);
    }

    endwin();
    return 0;
}

#else

#include "UISystem.h"
using namespace mk;



int main(int argc, char const *argv[])
{
    SoundSystem system;
    UIGenericList tracklist(0,0,100,50);
    UIReorderList playlist(0,0,100,50);
    playlist.SetFocus(true);
    MKArgs a = ParseArgs(argc, argv);

    Playlist p;
    switch(a.option)
    {
        case MKOption::Directory:{
            if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p))
            {
                return -1;
            }
            // PopulateList(p, &playlist);
            playlist.SetPlaylist(&p);
        } break;

        case MKOption::DirectoryRecursive:{
            if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p, true))
            {
                return -1;
            }
            // PopulateList(p, &playlist);
            playlist.SetPlaylist(&p);
            
        } break;

        case MKOption::SinglePlay:
        {
            /* std::string file(a.path);
            std::string path;
            size_t pos = file.rfind('/');
            path = file.substr(0, pos + 1);
            if (!PlaylistMgr::LoadPlaylistFromDir(path.c_str(), p))
            {
                std::cout << "ERR\n";
                return -1;
            }
            for (size_t i = 0; i < p.songs.size(); i++)
            {
                if (p.songs[i]->filePath == file && i != 0)
                {
                    std::swap(p.songs[0], p.songs[i]);
                }
            } */
            if (!PlaylistMgr::LoadPlaylistDirFromFile(a.path.c_str(), p, false))
                return -1;
            playlist.SetPlaylist(&p);
        } break;

        default:
            return -1;
            break;
    }

    if(playlist.GetSize() < 1)
    {
        std::cout << "No music files found in directory or playlist specified\n";
        return -1;
    }
    // size_t current = 0;
    InitUI();
    UILine s(playlist.GetItem(0)->data, 0,0);
    
    system.LoadTrack(p.songs[0]->filePath.c_str());
    system.TrackStart();
    
    for(;;)
    {
        clear();
        int row, col;
        getmaxyx(stdscr, row, col);
        // Draws Current track
        s.y = row-2;
        s.w = col;
        s.Print(row, col);

        playlist.SetW(col / 2);
        playlist.SetH(row - 3);

        // Draws Progress bar
        float length;
        // unsigned int len;
        float now = 0.0f;
        // FMOD::Sound* sound = system.GetSound();
        // sound->getLength(&len,FMOD_TIMEUNIT_MS );

        // length = (double)len * MS_TO_MIN;
        try{
            length = system.GetSoundLength();
            now = system.GetSoundTime();
            // ProgressBar(length, now, row, col, row - 1);
            ProgressBarGeneric(length, now, row, col, 0, row - 1, col);
        } catch( std::runtime_error& s)
        {
            // TODO: Log error in status bar
        }
        playlist.Print(row, col);


        PrintVertSeparator(row, col, 0, row - 4, col / 2 , '|');

        PrintHoriSeparator(row, col, 0, col, row - 3, '-');

        attron(COLOR_PAIR(1));
        UISoundPopup::Print(row, col);

        attroff(COLOR_PAIR(1));
        refresh();
 
        int c = getch();
        if(c == 'q')
            break;
        else if(c == ' ')
        {
            system.TogglePlaying();
        }
        else if(c == '\t')
        {
            // Switch Focus
            tracklist.SetFocus(playlist.GetFocus());
            playlist.SetFocus(!tracklist.GetFocus());

        }
        else if (c == '>')
        {
            system.AddSoundTime(5.0 / 60.0);
        }
        else if (c == '<')
        {
            system.AddSoundTime(-(5.0 / 60.0));
        }
        bool b = playlist.Update(c, row, col);
        tracklist.Update(c, row, col);
        if(b)
        {
            // const std::vector<UITrackLine*> v = playlist.GetElVec();
            system.TrackPause();
            if(!system.LoadTrack(p.songs[playlist.GetSelectedItem()->trackNumber]->filePath.c_str()))
            {
                continue;
            }
            system.TrackStart();
            playlist.SetCurrentToSelected();
            const UITrackLine* line =  playlist.GetCurrentItem();
            s.data = line->data;
        }
        // If Song has ended TODO: add autoplay toggle option
        if((length - now <= 0.0001f && now > 0.0001f) || c == 'n')
        {
            system.TrackPause();
            if(!playlist.ChangeCurrent(1))
            {
                continue;
            }
            // if(!system.LoadTrack(playlist.GetItem(playlist.GetCurrent())->data.c_str()))
            if(!system.LoadTrack( p.songs[playlist.GetCurrentItem()->trackNumber]->filePath.c_str() ))
            {
                continue;
            }
            system.TrackStart();

            const UITrackLine* line = playlist.GetCurrentItem();
            s.data = line->data;
        }

        UISoundPopup::Update(c, 3000);
        system.SetVolume(UISoundPopup::GetVolume());


        usleep(3000);
    }

    endwin();
    return 0;
}


#endif
