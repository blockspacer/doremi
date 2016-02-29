#pragma once
/// Project specific
#include <Handler/StateHandler.hpp>
#include <EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
// Events
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
#include <Doremi/Core/Include/EventHandler/Events/TriggerEvent.hpp>

/// Engine
// Audio
#include <DoremiEngine/Audio/Include/AudioModule.hpp>
// Graphic
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
/// Standard Libraries
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>


namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        StateHandler* StateHandler::m_singleton = nullptr;
        StateHandler* StateHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("StateHandler called GetInstance befor startup.");
            }
            return m_singleton;
        }
        void StateHandler::StartStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new StateHandler(p_sharedContext);
            }
            else
            {
                // log in personal logger, not critical
            }
        }
        void StateHandler::StopStateHandler()
        {
            // TODOKO implement
        }
        StateHandler::StateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            EventHandler::GetInstance()->Subscribe(EventType::ChangeMenuState, this);
            EventHandler::GetInstance()->Subscribe(EventType::Trigger, this);
            m_state = DoremiStates::MAINMENU;
        }
        void StateHandler::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::ChangeMenuState)
            {
                ChangeMenuState* realEvent = static_cast<ChangeMenuState*>(p_event);
                // This is a bit ugly but since buttons only can send events for now i figured we do it here!
                if(realEvent->state == DoremiStates::FULLSCREEN)
                {
                    m_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager().SetFullscreen(fullscreen);
                    fullscreen = !fullscreen;
                }
                else
                {
                    m_state = realEvent->state;
                }
            }
            else if(p_event->eventType == EventType::Trigger)
            {
                TriggerEvent* realEvent = static_cast<TriggerEvent*>(p_event);
                CheckTrigger(realEvent);
            }
        }
        void StateHandler::CheckTrigger(TriggerEvent* p_event)
        {
            // Does this work?
            TriggerType type = p_event->triggerType;
            if(((int)type & (int)TriggerType::GoalTrigger) == (int)TriggerType::GoalTrigger)
            {
                ChangeMenuState* menuStateEvent =
                    new ChangeMenuState(); // Send event so we easily can call the event changing algorithm to see if the new state is valid
                menuStateEvent->state = DoremiStates::VICTORY;
                EventHandler::GetInstance()->BroadcastEvent(menuStateEvent); // TODOKO review if this is a good idea
                // m_state = DoremiStates::MAINMENU;
            }
        }
    }
}
