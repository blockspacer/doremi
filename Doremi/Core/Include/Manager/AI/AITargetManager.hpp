#include <Doremi/Core/Include/Manager/Manager.hpp>
namespace Doremi
{
    namespace Core
    {
        class AITargetManager : public Manager
        {
        public:
            AITargetManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~AITargetManager();
            void Update(double p_dt) override;

        private:
            // Helper functions
            void FireAtEntity(const size_t& p_entityID, const size_t& p_enemyID, const float& p_distance);
        };
    }
}