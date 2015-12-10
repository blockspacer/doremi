#pragma once
#include <unordered_map>
#include <InputModule.hpp>
#include <vector>
#include <SDL2/SDL.h>


namespace DoremiEngine
{
    namespace Input
    {
        class InputModuleImplementation : public InputModule
        {
        private:
            struct MouseMovementStruct
            {
                int x = 0;
                int y = 0;
            };

            MouseMovementStruct m_mouseMovementStruct;
            std::vector<int>m_keyboardButtonsDown;//used for typing.
            std::vector<int>m_mouseButtonsDown;//will be sent if the target wants the buttons of the mouse(no scrolling)
            int m_mouseWheelSpins = 0;//Y direction


            public:
            /**
                TODO docs
            */
            InputModuleImplementation();

            /**
                TODO docs
            */
            virtual ~InputModuleImplementation();

            /**
                TODO docs
            */
            void Startup() override;

            /**
                TODO docs
            */
            void SetWorkingDirectory(const std::string& p_workingDirectory) override;

            /**
                TODO docs
            */
            void Shutdown() override;
            void Update() override;
            int CreateWindowSDL(int p_width, int p_height);//Returns 1 if a window is created
            void PrintInputStructsDEBUG();//TODOEA Ta bort i slutet kanske
            void PrintInputMouseMovement();//TODEA Ta bort skiten slutet
            /////GET//////////////////
            const std::vector<int> GetKeyBoardInput() const { return m_keyboardButtonsDown; }
            const std::vector<int> GetMouseButtonInput() const { return m_mouseButtonsDown; }
            int GetMouseMovementX();// const { return m_mouseMovementStruct.x; }
            int GetMouseMovementY();// const { return m_mouseMovementStruct.y; }
            int GetMouseWheelSpins(); //const { return m_mouseWheelSpins; }
            //////////////////////////


        private:
            //SDL_Window *m_win;
            const Uint8* m_keyState;
            //void InputForPlayingUpdate();
            void SwitchCaseEventsForPlaying(SDL_Event &p_eventVariable);
            void SwitchCaseEventsForTyping(SDL_Event &p_eventVariable);
            void ResetMouseMovementStruct();
            void ResetButtonsDown();
            //void ResetInputForPlayingStruct();
            void AddToList(SDL_Keycode p_eventvariable, std::vector<int> &o_listToUse);
            void RemoveFromList(SDL_Keycode p_eventvariable, std::vector<int> &o_listToUse);
        };
    }
}