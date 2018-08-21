/*
 *  TODO: PlaylistManager
 *  TODO: Play Single from first arg
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
#include <curses.h>
#include <json.hpp>
#include <locale.h>

#ifdef WIN32

#include <windows.h>

void usleep(__int64 usec) 
{ 
    HANDLE timer; 
    LARGE_INTEGER ft; 

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL); 
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
    WaitForSingleObject(timer, INFINITE); 
    CloseHandle(timer); 
}

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
    initscr();
    noecho();
    curs_set(0);
    cbreak();
    timeout(500);
    keypad(stdscr, true);
    refresh();

    setlocale(LC_ALL, "");
}


struct MKArgs{
    std::string     path;
    bool            isDirMode;
    MKOption        option;
};



MKArgs ParseArgs(int argc, char const * argv[])
{
    MKArgs out;
    out.option = MKOption::Unspecified;
    if(argc > 2)
    {
        if(strcmp(argv[1],"-d") == 0)
        {
            out.option = MKOption::Directory;
        }
        else if(strcmp(argv[1], "-dr") == 0)
        {
            out.option = MKOption::DirectoryRecursive;
        }
        else if(strcmp(argv[1], "-p") == 0)
        {
            out.option = MKOption::PlaylistFile;
        }
        out.path = argv[2];
    }

    return out;
}


void PopulateList(json* queue, StyledList* list)
{
    for(auto it = queue->begin(); it != queue->end(); it++)
    {
        std::string s = *it;
        list->Add(s);
    }
}

void PopulateList(Playlist& p , StyledList* list)
{
    if(list == nullptr) return;
    list->Clear();
    for(SongInfo* s : p.songs)
    {
        list->Add(s->fileName);
    }
}

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

    std::cout << (a.option != MKOption::Unspecified) << '\n';
    std::cout << a.path << '\n';

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
        if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p, true))
        {
            std::cout << "ERR\n";
            return -1;
        }
    }

    for(auto& it : p.songs)
    {
        std::cout << it->fileName << '\n';
    }


    s.LoadTrack(p.songs[0]->filePath.c_str());
    s.TrackStart();
    std::cin.get();
}

#elif defined (GUI_TEST)

int main(int argc, char const *argv[])
{
    float length = 2.05f;
    float now = 1.02f;
    InitUI();

    // SelectableList list;
    StyledList list(0,0,30,10);
    list.Add("A Kul Song 1");
    list.Add("Despacito 3");
    list.Add("Hello Kitty OST");
    list.Add("Furrytale - Alexander Rybak");

    StyledLine s(std::string("A Kul Song 1"), 0,0);
    refresh();
    int barSize = 20;
    float volume = 1.0f;

    for(;;)
    {
        clear();
        int row, col;
        getmaxyx(stdscr, row, col);
        unsigned int w = col / 2 - 1;
        list.SetW(w);

        s.y = row-2;
        s.w = col;
        s.Print(row, col);
        ProgressBar(length, now, row, col, row - 1);
        list.Print(row, col);
        // Separator
        PrintVertSeparator(row, col, 0, row - 4, w + 1, '|');

        PrintHoriSeparator(row, col, 0, col, row - 3, '-');

        UISoundPopup::Print(row, col);

        refresh();

        usleep(3000);
        // sleep(1);

        int c = getch();
        if(c == 'q')
            break;
        bool b = list.Update(c);
        if(b)
        {
            StyledLine* line = list.GetSelectedItem();
            s.data = line->data;
            list.ApplySelectedToCurrent();
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
    StyledList playlist(0,0,100,50);

    MKArgs a = ParseArgs(argc, argv);

    Playlist p;
    switch(a.option)
    {
        case MKOption::Directory:{
            if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p))
            {
                return -1;
            }
            PopulateList(p, &playlist);
        } break;

        case MKOption::DirectoryRecursive:{
            if(!PlaylistMgr::LoadPlaylistFromDir(a.path.c_str(), p, true))
            {
                return -1;
            }
            PopulateList(p, &playlist);
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
    StyledLine s(playlist.GetItem(0)->data, 0,0);
    
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
        float length = 0.0f;
        unsigned int len;
        float now = 0.0f;
        FMOD::Sound* sound = system.GetSound();
        sound->getLength(&len,FMOD_TIMEUNIT_MS );
        length = (double)len * MS_TO_MIN;
        FMOD::Channel* channel = nullptr;
        channel = system.GetChannel();
        len = 0u;
        if(channel)
            channel->getPosition(&len, FMOD_TIMEUNIT_MS);

        if (!len || channel == nullptr)
            now = 0.f;
        else
            now = (double)len * MS_TO_MIN;

        ProgressBar(length, now, row, col, row - 1);
        playlist.Print(row, col);


        PrintVertSeparator(row, col, 0, row - 4, col / 2 + 1, '|');

        PrintHoriSeparator(row, col, 0, col, row - 3, '-');

        UISoundPopup::Print(row, col);

        refresh();

        int c = getch();
        if(c == 'q')
            break;
        else if(c == ' ')
        {
            system.TogglePlaying();
        }
        bool b = playlist.Update(c);
        // TODO: Fix current playing song not changing
        if(b)
        {
            system.TrackPause();
            if(!system.LoadTrack(p.songs[playlist.GetSelectedItemIndex()]->filePath.c_str()))
            {
                continue;
            }
            system.TrackStart();
            playlist.ApplySelectedToCurrent();
            StyledLine* line =  playlist.GetCurrentSelectedItem();
            s.data = line->data;
        }
        // If Song has ended TODO: add autoplay toggle option
        if(length - now <= 0.0001f && now > 0.0001f)
        {
            system.TrackPause();
            if(!playlist.Next())
            {
                continue;
            }
            // if(!system.LoadTrack(playlist.GetItem(playlist.GetCurrent())->data.c_str()))
            if(!system.LoadTrack( p.songs[playlist.GetCurrent()]->filePath.c_str() ))
            {
                continue;
            }
            system.TrackStart();

            StyledLine* line = playlist.GetCurrentSelectedItem();
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