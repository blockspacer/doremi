#pragma once
// Project specific
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
// Standard Libraries
#include <unordered_map>
#include <map>
#include <vector>
#include <DirectXMath.h>


namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }

    namespace Logging
    {
        class Logger;
    }
}

namespace Doremi
{
    namespace Core
    {

        class InputHandler;
        /**
        Handles recording and analysis. Also handles background sound depending on game state
        */
        class AudioHandler : public Subscriber
        {
        public:
            /** Is a singleton. Use this method to get the AudioHandler*/
            static AudioHandler* GetInstance();
            static void StartAudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static void StopAudioHandler();
            explicit AudioHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~AudioHandler();
            // Initializes the handler. Mostly sets values to 0 or channelID to -1 (used as a "error" value)
            void Initialize();
            // Sets the variable for how long the reloadbutton was pressed and therefore how long the sound should be
            void SetGunButtonDownTime(double p_time);
            // Starts the recording that streams open mic
            void StartContinuousRecording();
            // Starts the recording that records while the button is pressed to be able to replay that sound
            void StartRepeatableRecording();
            // Sets up the streaming open mic recoreder
            void SetupContinuousRecording();
            // Sets up the repeatable recorder
            void SetupRepeatableRecording();
            // Plays the sound that we recorded
            void PlayRepeatableRecordedSound();
            // Update to analyse sound. Contains switch case to control what sound is supposed to be analysed and when.
            void Update(double p_deltaTime);
            float GetFrequency() const { return m_currentFrequency; };
            float GetAmplitudeOfRecording() const { return m_currentAmplitude; };
            // Returns the current frequency from the repeatable sound. It is fetched from an array with the help of the recordpointer from the
            // playing sound.
            float GetRepeatableSoundFrequency();
            // Sets mastervolume 0 is no audio 1 is max audio
            void SetMasterVolume(const float& p_volume);
            // Sets the effect volume 0 is no audio 1 is max audio
            void SetEffectVolume(const float& p_volume);
            // Sets the music volume 0 is no audio 1 is max audio
            void SetMusicVolume(const float& p_volume);
            // Sets the amplitude cut off, between 0 and 1
            void SetAmplitudeCutOff(const float& p_cutOff);
            // Stops all effect sounds
            void StopEffectSounds();

            void OnEvent(Event* p_event) override;

        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            InputHandler* m_inputHandler;

            double m_accumulatedDeltaTime;
            // Vector of the frequencies from the repeatable sound
            std::vector<float> m_frequencies;

            float m_frequencyVectorPrecision;
            static AudioHandler* m_singleton;
            // Sound IDs and ChannelIDs
            int m_continuousFrequencyAnalyserChannelID;
            int m_continuousFrequencyAnalyserSoundID;
            int m_repeatableFrequencyAnalyserChannelID;
            int m_repeatableFrequencyAnalyserSoundID;
            int m_outputRepeatableSoundChannelID;
            int m_outputRepeatableSoundID;
            // The current frequency that just got analysed, and the amplitude
            float m_currentFrequency;
            float m_currentAmplitude;
            // Used to control the flow of pdate function. Cant use the function until the array is filled with values
            bool m_repeatableAnalysisComplete;
            // Keeps the time that the gunreloadbutton was pressed down
            double m_timeGunReloadButtonWasPressed;
            // Holds the different background sounds for the game
            enum class BackgroundSound
            {
                InGameMainTheme,
                MenuMainTheme,
                CheckpointReached,
            };
            struct ChannleSoundPair
            {
                int channelID = -1;
                int soundID = -1;
            };
            std::map<BackgroundSound, ChannleSoundPair> m_backgroundSounds;

            // Used to control the flow of the update function
            enum SoundState
            {
                ANALYSECONTINUOUS,
                HOLDCONTINUOUSANALYSIS,
                ANALYSEREPEATABLE,
                HOLDREPEATABLEANALYSIS,

            };
            SoundState m_SoundState;

            // Logger
            DoremiEngine::Logging::Logger* m_logger;
        };
    }
}
