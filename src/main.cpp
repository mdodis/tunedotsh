/*
 */
// #define PLAYLIST_TEST
#define GUI_TEST
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
/*
 * Windows does not provide usleep, solution is
 * to define it here
 */

#include <chrono>
#include <thread>

void usleep(__int64 usec)
{
    std::this_thread::sleep_for(std::chrono::seconds(usec));
}

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
    cbreak();
    timeout(500);
    keypad(stdscr, true);
    refresh();

}


struct MKArgs{
    std::string     path;
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


void PopulateList(json* queue, UIGenericList* list)
{
    for(auto it = queue->begin(); it != queue->end(); it++)
    {
        std::string s = *it;
        list->AddItem(s);
    }
}

void PopulateList(Playlist& p , UIGenericList* list)
{
    if(list == nullptr) return;
    list->Clear();
    for(SongInfo* s : p.songs)
    {
        list->AddItem(s->fileName);
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

    if (p.songs.size() == 0)
        return -1;
    for(auto& it : p.songs)
    {
        std::cout << it->fileName << '\n';
        std::cout << it->filePath << '\n';
    }


    s.LoadTrack(p.songs[0]->filePath.c_str());
    s.TrackStart();
    std::cin.get();
}

#elif defined (GUI_TEST)

int main(int argc, char const *argv[])
{
    float length = 5.05f;
    float now = 1.02f;
    InitUI();

    // SelectableList list;
    UIReorderList tracklist(0,0,30,10);
    tracklist.AddItem("A Kul Song 1");
    tracklist.AddItem("Despacito 3");
    tracklist.AddItem("Hello Kitty OST");
    tracklist.AddItem("Furrytale - Alexander Rybak And Some Idiotic Swedish Dudes");
    tracklist.SetFocus(true);

    UIGenericList playlist(0, 0, 30, 10);
    // playlist.Add("YOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYOYO");
    playlist.AddItem("YOYOYOYO");
    playlist.AddItem("YaOYOYOYO");
    playlist.AddItem("YOYasOYOfYO");
    playlist.AddItem("YOYOYOYOf");
    playlist.AddItem("YOYOYasdOYO");
    playlist.AddItem("YOYOYOYOdd");

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
        tracklist.SetW(w + 1);
        playlist.SetW(w);

        playlist.SetX(w + 2);

        s.y = row-2;
        s.w = col;
        s.Print(row, col);
        ProgressBar(length, now, row, col, row - 1);
        tracklist.Print(row, col);
        playlist.Print(row, col);
        // Separator
        // PrintVertSeparator(row, col, 0, row - 4, w + 1, '|');

        PrintHoriSeparator(row, col, 0, col, row - 3, (wint_t)L'═');

        PrintVertSeparator(row, col, 0, row - 4, w + 1, (wint_t)L'║');
        UISoundPopup::Print(row, col);

        refresh();

        usleep(3000);

        // Update
        int c = getch();
        if(c == 'q')
            break;
        else if(c == '\t')
        {
            // Switch Focus
            tracklist.SetFocus(playlist.GetFocus());
            playlist.SetFocus(!tracklist.GetFocus());

        }
        bool b = tracklist.Update(c);
        bool d = playlist.Update(c);
        if(b)
        {
            const StyledLine* line = tracklist.GetSelectedItem();
            s.data = line->data;
            tracklist.SetCurrentToSelected();
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
    UIReorderList tracklist(0,0,100,50);
    UIReorderList playlist(0,0,100,50);

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

        case MKOption::SinglePlay:
        {
            
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
        float length;
        unsigned int len;
        float now = 0.0f;
        FMOD::Sound* sound = system.GetSound();
        sound->getLength(&len,FMOD_TIMEUNIT_MS );
        length = (double)len * MS_TO_MIN;
        FMOD::Channel* channel;
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
            playlist.SetCurrentToSelected();
            const StyledLine* line =  playlist.GetCurrentItem();
            s.data = line->data;
        }
        // If Song has ended TODO: add autoplay toggle option
        if(length - now <= 0.0001f && now > 0.0001f)
        {
            system.TrackPause();
            if(!playlist.ChangeCurrent(1))
            {
                continue;
            }
            // if(!system.LoadTrack(playlist.GetItem(playlist.GetCurrent())->data.c_str()))
            if(!system.LoadTrack( p.songs[playlist.GetCurrentItemIndex()]->filePath.c_str() ))
            {
                continue;
            }
            system.TrackStart();

            const StyledLine* line = playlist.GetCurrentItem();
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