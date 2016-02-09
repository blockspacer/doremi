#pragma once
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>

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

        /**
            Stores latest spawn point
        */
        class PlayerSpawnerHandler : public Subscriber
        {

            static PlayerSpawnerHandler* GetInstance();

            static void StartupPlayerSpawnerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
                Returns the id of the latest spawn point
            */
            EntityID GetCurrentSpawnerEntityID();

            /**
                Set the ID of a spawner as current spawnpoint
            */
            void SetCurrentSpawner(EntityID p_entityID);

            /**
                Respawns player on last spawnpoint, throws event
            */
            void RespawnPlayer(EntityID p_entityID);

            /**
                Events checking for triggering spawn point
            */
            void OnEvent(Event* p_event) override;

        private:
            PlayerSpawnerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~PlayerSpawnerHandler();

            static PlayerSpawnerHandler* m_singleton;

            const DoremiEngine::Core::SharedContext& m_sharedContext;

            EntityID m_currentPlayerSpawner;
        };
    }
}