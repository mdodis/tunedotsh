#include "SoundSystem.h"

#include <dirent.h>
#include <exception>

namespace mk{
    SoundSystem::SoundSystem()
    {
        FMOD_RESULT result = FMOD::System_Create(&system);
        if(result != FMOD_OK)
        {
            // std::cerr << "FMOD: " << FMOD_ErrorString(result) << std::endl;
            throw std::runtime_error(FMOD_ErrorString(result));
            // throw MKError::FMOD_INIT_ERROR;
        }

        int driverCount = 0;
        system->getNumDrivers(&driverCount);

        if(driverCount == 0) return;

        result = system->init(36, FMOD_INIT_NORMAL, NULL);
        if(result != FMOD_OK)
        {
            // std::cerr << "FMOD: " << FMOD_ErrorString(result) << std::endl;
            throw std::runtime_error(FMOD_ErrorString(result));
        }
    }

    SoundSystem::~SoundSystem()
    {
        system->release();
    }

    void SoundSystem::CreateStream(FMOD::Sound** pSound, const char* file)
    {
        FMOD_RESULT result = system->createStream(file, FMOD_DEFAULT, 0, pSound);
        if(result != FMOD_OK)
        {
            // std::cerr << "FMOD: " << FMOD_ErrorString(result) << std::endl;
            throw std::runtime_error(FMOD_ErrorString(result));
        }
    }

    void SoundSystem::PlayStream(FMOD::Sound* pSound, FMOD::Channel** channel)
    {
        FMOD_RESULT result = system->playSound(
            pSound,
            0,
            false,
            channel
        );
        if(result != FMOD_OK)
        {
            throw std::runtime_error(FMOD_ErrorString(result));
        }
    }

    bool SoundSystem::LoadTrack(const char* file)
    {
        std::cout << file << '\n';
        if(sound != nullptr)
        {
            sound->release();
        }
        try{
            CreateStream(&sound, file);

        } catch (std::runtime_error err)
        {
            std::cerr << "Load Track Failed!\n" << err.what() << std::endl;
            return false;
        }
        return true;
    }

    bool SoundSystem::LoadTrack_Buffer(const char* file)
    {
        try{
            CreateStream(&bufferSound, file);

        } catch (std::runtime_error err)
        {
            std::cerr << "Load Track Failed!\n" << err.what() << std::endl;
            return false;
        }
        return true;

    }

    void SoundSystem::ChangeTrack_Buffer()
    {
        TrackPause();
        FMOD::Sound* tmp = sound;
        sound = bufferSound;
        bufferSound = tmp;
    }
}
