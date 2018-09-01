#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fmod/fmod.hpp>
#include <fmod/fmod_errors.h>
#include <fstream>
#include <json.hpp>

using json = nlohmann::json;

namespace mk{

    enum class MKFormat{
        MPEG,
        VORBIS,
        UNSUPPORTED
    };

    struct SongInfo{
        std::string     filePath;
        std::string     fileName;
        MKFormat        fileFormat;
    };

    typedef struct SongDetails SongDetails;

    struct SongDetails{
        std::string     title;
        std::string     album;
        std::string     artist;
        std::string     year;

        static void GetTag_String(FMOD::Sound* const sound, const char* tagStr, std::string& s )
        {
            FMOD_TAG t;
            sound->getTag(tagStr, 0, &t);
            if(t.datalen < 1 || t.datatype != FMOD_TAGDATATYPE_STRING)
                s = "Unavailable";
            else
                s = (char *)t.data;
        }

        static SongDetails* From_ID3V1(FMOD::Sound* const sound)
        {
            SongDetails* details = new SongDetails;
            GetTag_String(sound, "TITLE", details->title);
            GetTag_String(sound, "ARTIST", details->artist);
            GetTag_String(sound, "ALBUM", details->album);
            GetTag_String(sound, "YEAR", details->year);
            return details;
        }
    };

    enum class MKError
    {
        FMOD_INIT_ERROR,
        FMOD_SOUND_ERROR,
        SYSTEM_ERROR
    };


    #define MS_TO_MIN 1.6667E-5

    class SoundSystem
    {
    public:
        SoundSystem();
        ~SoundSystem();


        inline bool TrackStart()
        {
            try{
                this->PlayStream(sound, &channel);

            }   catch(std::runtime_error& e)
            {
                std::cerr << "Error on playing Stream!\n" << e.what() << std::endl;
                return false;
            }
            isPlaying = true;
            return true;
        }
        inline void TrackPlay() {
            if(channel != nullptr)
                channel->setPaused(false);
        }
        inline void TrackPause() {
            if(channel != nullptr)
                channel->setPaused(true);
        }

        inline void SetVolume(float value)
        {
            if(channel)
                channel->setVolume(value);
        }

        inline FMOD::Sound*     GetSound() {return sound;} 
        inline FMOD::Channel*   GetChannel() {return (channel == nullptr ? nullptr : channel);}

        inline void TogglePlaying()
        {
            if(channel != nullptr)
            {
                isPlaying = !isPlaying;
                channel->setPaused(!isPlaying);
            }
        }
        bool LoadTrack(const char* file);
        bool LoadTrack_Buffer(const char* file);
        void ChangeTrack_Buffer();
        void CreateStream(FMOD::Sound** pSound, const char* file);
        void PlayStream(FMOD::Sound* pSound, FMOD::Channel** channel);


//      TODO Implement these
        inline double GetSoundLength() const
        {
            unsigned int len;
            FMOD_RESULT res = sound->getLength(&len, FMOD_TIMEUNIT_MS);
            if (res != FMOD_OK)
                throw std::runtime_error(FMOD_ErrorString(res));
            return (double)len * MS_TO_MIN; 
            
        }
        inline double GetSoundTime () const
        {
            unsigned int len;
            FMOD_RESULT res = channel->getPosition(&len, FMOD_TIMEUNIT_MS);
            if (res != FMOD_OK)
                throw std::runtime_error(FMOD_ErrorString(res));
            return (double)len * MS_TO_MIN;

        }

        void SetSoundTime (unsigned int time)
        {
            // double length = GetSoundLength();
            // if ((double)time * MS_TO_MIN > length ) return;
            FMOD_RESULT res = channel->setPosition(time , FMOD_TIMEUNIT_MS);
            if (res != FMOD_OK)
                throw std::runtime_error(FMOD_ErrorString(res));
        }
        void AddSoundTime (double time)
        {
            try {
                SetSoundTime((unsigned int)(time / MS_TO_MIN) + (unsigned int )(GetSoundTime() / MS_TO_MIN));
            } catch (std::runtime_error& e)
            {
                // TODO: LOG

                return;
            }
        }

        void AddSoundTimeRelative(double ratio);
        bool GetSoundFinished() const;
    private:
        FMOD::System*   system      = nullptr;
        FMOD::Sound*    sound       = nullptr;
        FMOD::Channel*  channel     = nullptr;
        FMOD::Sound*    bufferSound = nullptr;
        bool            isPlaying   = false;
    };


    struct Playlist{
        // std::vector<std::string>    files;
        std::vector<SongInfo*>      songs;
        // size_t                      currentTrack = 0;

        inline void clear() {
            for (int i = 0; i < songs.size(); i++)
            {
                delete songs[i];
            }
            songs.clear();
        }
    };
}
