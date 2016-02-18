/// Project
#include <Doremi/Core/Include/Manager/AI/AITargetManager.hpp>
#include <PlayerHandler.hpp>

// Components
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/RangeComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/AITimerComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
// Helper
#include <Helper/ProximityChecker.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/RayCastManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
// Third party
#include <DirectXMath.h>
#include <iostream>
namespace Doremi
{
    namespace Core
    {
        AITargetManager::AITargetManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "AITargetManager") {}

        AITargetManager::~AITargetManager() {}

        void AITargetManager::Update(double p_dt)
        {
            using namespace DirectX;
            // gets all the players in the world, used to see if we can see anyone of them
            std::map<uint32_t, Player*> t_players = PlayerHandler::GetInstance()->GetPlayerMap();
            size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();

            for(size_t i = 0; i < length; i++)
            {
                // Check and update the attack timer
                bool shouldFire = false;
                if(t_entityHandler.HasComponents(i, (int)ComponentType::AITimer | (int)ComponentType::AIAgent))
                {
                    AITimerComponent* timer = t_entityHandler.GetComponentFromStorage<AITimerComponent>(i);
                    timer->attackTimer += p_dt;
                    // If above attack freq we should attack
                    if(timer->attackTimer > timer->attackFrequency)
                    {
                        timer->attackTimer -= timer->attackFrequency;
                        shouldFire = true;
                    }
                    else
                    {
                        // else continue to next for itteration
                        shouldFire = false;
                    }
                }
                else
                {
                    shouldFire = false; // No timer? TODOKO log error and dont let it fire!
                }
                if(t_entityHandler.HasComponents(i, (int)ComponentType::Range | (int)ComponentType::AIAgent | (int)ComponentType::Transform))
                {
                    // They have a range component and are AI agents, let's see if a player is in range!
                    // I use proximitychecker here because i'm guessing it's faster than raycast
                    TransformComponent* AITransform = t_entityHandler.GetComponentFromStorage<TransformComponent>(i);
                    RangeComponent* aiRange = t_entityHandler.GetComponentFromStorage<RangeComponent>(i);
                    int closestVisiblePlayer = -1;
                    float closestDistance = aiRange->range;

                    // Check waht player is close and visible
                    for(auto pairs : t_players)
                    {
                        int playerID = pairs.second->m_playerEntityID;
                        float distanceToPlayer = ProximityChecker::GetInstance().GetDistanceBetweenEntities(pairs.second->m_playerEntityID, i);
                        if(distanceToPlayer < closestDistance)
                        {
                            // Potential player found, check if we see him
                            // We are in range!! Let's raycast in a appropirate direction!! so start with finding that direction!
                            if(!EntityHandler::GetInstance().HasComponents(playerID, (int)ComponentType::Transform)) // Just for saftey :D
                            {
                                // TODOKO Log error!!!!!!!
                                return;
                            }
                            // Fetch some components
                            TransformComponent* playerTransform = t_entityHandler.GetComponentFromStorage<TransformComponent>(playerID);

                            // Get things in to vectors
                            XMVECTOR playerPos = XMLoadFloat3(&playerTransform->position);
                            XMVECTOR AIPos = XMLoadFloat3(&AITransform->position);

                            // Calculate direction
                            XMVECTOR direction = playerPos - AIPos; // Might be the wrong way
                            direction = XMVector3Normalize(direction);
                            XMFLOAT3 directionFloat;
                            XMStoreFloat3(&directionFloat, direction);
                            // Offset origin of ray so we dont hit ourself
                            XMVECTOR rayOrigin = AIPos + direction * 5.0f; // TODOCONFIG x.xf is offset from the units body, might need to increase if
                            // the bodies radius is larger than x.x
                            XMFLOAT3 rayOriginFloat;
                            XMStoreFloat3(&rayOriginFloat, rayOrigin);
                            // Send it to physx for raycast calculation
                            int bodyHit = m_sharedContext.GetPhysicsModule().GetRayCastManager().CastRay(rayOriginFloat, directionFloat, aiRange->range);

                            if(bodyHit == playerID)
                            {
                                closestDistance = distanceToPlayer;
                                closestVisiblePlayer = pairs.second->m_playerEntityID;

                                // Rotate the enemy to face the player
                                XMMATRIX mat = XMMatrixInverse(nullptr, XMMatrixLookAtLH(AIPos, AIPos - direction, XMLoadFloat3(&XMFLOAT3(0, 1, 0))));
                                XMVECTOR rotation = XMQuaternionRotationMatrix(mat);
                                XMFLOAT4 quater;
                                XMStoreFloat4(&quater, rotation);
                                AITransform->rotation = quater;
                            }
                        }
                    }
                    if(closestVisiblePlayer != -1)
                    {
                        // We now know what player is closest and visible
                        if(shouldFire)
                        {
                            TransformComponent* playerTransform = t_entityHandler.GetComponentFromStorage<TransformComponent>(closestVisiblePlayer);
                            // Get things in to vectors
                            XMVECTOR playerPos = XMLoadFloat3(&playerTransform->position);
                            XMVECTOR AIPos = XMLoadFloat3(&AITransform->position);

                            // calculate direction again...
                            XMVECTOR direction = playerPos - AIPos; // Might be the wrong way
                            direction = XMVector3Normalize(direction);
                            XMFLOAT3 directionFloat;
                            XMStoreFloat3(&directionFloat, direction);

                            XMVECTOR bulletOrigin =
                                AIPos + direction * 5.0f; // TODOCONFIG x.xf is offset from the units body, might need to increase if
                            // the bodies radius is larger than x.x
                            XMFLOAT3 bulletOriginFloat;
                            XMStoreFloat3(&bulletOriginFloat, bulletOrigin);
                            int id = t_entityHandler.CreateEntity(Blueprints::BulletEntity, bulletOriginFloat);

                            // Add a force to the body TODOXX should not be hard coded the force amount
                            direction *= 1500.0f;
                            XMFLOAT3 force;
                            XMStoreFloat3(&force, direction);
                            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddForceToBody(id, force);
                        }
                        // If we see a player turn off the phermonetrail
                        if(t_entityHandler.HasComponents(i, (int)ComponentType::PotentialField))
                        {
                            PotentialFieldComponent* pfComp = t_entityHandler.GetComponentFromStorage<PotentialFieldComponent>(i);
                            pfComp->ChargedActor->SetUsePhermonetrail(false);
                            pfComp->ChargedActor->SetActivePotentialVsType(DoremiEngine::AI::AIActorType::Player, true);
                        }
                    }
                }
            }
        }
    }
}