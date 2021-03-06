#pragma once
#include <Interface/SubModule/PotentialFieldSubModule.hpp>
#include <Internal/AIContext.hpp>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldSubModuleImpl : public PotentialFieldSubModule
        {
        public:
            PotentialFieldSubModuleImpl(AIContext& p_aiContext);
            virtual ~PotentialFieldSubModuleImpl();
            PotentialField* CreateNewField(const float& p_width, const float& p_height, const int& p_numberOfQuadsWidth,
                                           const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT3& p_center, const std::string& p_fieldName) override;
            PotentialField* CreateNewFieldFromFile(const std::string& p_fileName) override;
            bool SaveFieldToFile(const PotentialField& p_fieldToSave, const std::string& p_fileName) override;
            PotentialGroup* CreateNewPotentialGroup() override;
            PotentialFieldActor* CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge, const float& p_range,
                                                const bool& p_static, const AIActorType& p_actorType) override;
            void AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) override;
            void EraseActor(PotentialFieldActor* op_actor) override;
            void EraseActor(PotentialFieldActor* op_actor, PotentialField* op_field) override;
            PotentialField* FindBestPotentialField(const DirectX::XMFLOAT3& p_position) override;
            void AddActorToEveryPotentialField(PotentialFieldActor* p_actor) override;
            std::vector<PotentialField*>& GetAllActiveFields() override { return m_fields; };
        private:
            std::vector<PotentialField*> m_fields;
            AIContext& m_context;
        };
    }
}
