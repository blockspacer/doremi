/// Project specific
// This class
#include <Manager/GroundEffectManagerServer.hpp>
// Handles
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent\Components\PressureParticleComponent.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

using namespace DirectX;
namespace Doremi
{
    namespace Core
    {

        GroundEffectManagerServer::GroundEffectManagerServer(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "GroundEffectManagerServer")
        {
            // EXPERIMENTAL PHYSICS. Hard-coded ID works since I thought ahead and made it signed. Tru story
            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().CreateArbitraryBody(-15);
        }

        GroundEffectManagerServer::~GroundEffectManagerServer() {}

        void GroundEffectManagerServer::Update(double p_dt)
        {
            EntityHandler& entityHandler = EntityHandler::GetInstance();
            const size_t length = entityHandler.GetLastEntityIndex();
            int mask = (int)ComponentType::PressureParticleSystem;
            for(size_t i = 0; i < length; i++)
            {
                // Check if we have a pressure particle system. TODOXX This will be really funny if we have just ambient particle systems
                if(entityHandler.HasComponents(i, mask))
                {
                    // Get new positions where we want to color. These are positions where particles have collided with something
                    const vector<XMFLOAT3>& newPositions = m_sharedContext.GetPhysicsModule().GetFluidManager().GetRemovedParticlesPositions(i);

                    // Loop through new positions
                    size_t size = newPositions.size();
                    if(size > 0)
                    {
                        for(size_t j = 0; j < size; j++)
                        {
                            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddShapeToBody(-15, newPositions[j]);
                        }
                    }
                }
            }
        }

        void GroundEffectManagerServer::OnEvent(Event* p_event) {}
    }
}