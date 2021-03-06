#pragma once
#include <Interface/PotentialField/PotentialFieldActor.hpp>
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActorImpl : public PotentialFieldActor
        {
        public:
            PotentialFieldActorImpl();
            virtual ~PotentialFieldActorImpl();

            void SetPosition(const DirectX::XMFLOAT3& p_position) override;
            void SetCharge(const float& p_charge) override { m_charge = p_charge; };
            void SetRange(const float& p_range) override { m_range = p_range; };
            void SetStatic(const bool& p_static) override { m_static = p_static; };
            void AddOccupiedQuad(const DirectX::XMINT2& p_quad) override;
            const DirectX::XMFLOAT3& GetPosition() const override { return m_position; };
            const float& GetCharge() const override { return m_charge; };
            const float& GetRange() const override { return m_range; };
            const AIActorType& GetActorType() const override { return m_actorType; };
            void SetActorType(const AIActorType& p_type) { m_actorType = p_type; };
            const std::vector<DirectX::XMINT2>& GetOccupiedQuads() const override { return m_occupiedQuads; };
            const bool& IsStatic() const override { return m_static; };
            const DirectX::XMINT2 GetClosestOccupied(const DirectX::XMINT2& p_quad);
            const std::vector<DirectX::XMINT2>& GetPhermoneTrail() const override { return m_phermoneTrail; };
            const DirectX::XMINT2 GetPrevGridPos() const override { return m_prevGridPos; };
            /*const DirectX::XMINT2 GetGridPos() const override { return m_gridPos; };*/
            void SetPrevGridPosition(const DirectX::XMINT2& p_prevGridPos) override { m_prevGridPos = p_prevGridPos; };
            void UpdatePhermoneTrail(const DirectX::XMINT2& p_gridPosToAdd) override;
            void EraseLatestAddedToPhermoneList() override;
            void AddPotentialVsOther(const PotentialChargeInformation& p_newPotential) override;
            virtual const std::vector<PotentialChargeInformation>& GetPotentialVsOthers() const { return m_potentialsVsOther; };
            void SetActivePotentialVsType(const AIActorType& p_type, bool p_active) override;
            bool GetUsePhermonetrail() const override { return m_usePhermonetrail; }
            void SetUsePhermonetrail(const bool& p_active) override { m_usePhermonetrail = p_active; }
            const DirectX::XMFLOAT3& GetWantedPosition() const override { return m_wantedPosition; };
            void SetWantedPosition(const DirectX::XMFLOAT3& p_wantedPosition) override { m_wantedPosition = p_wantedPosition; };
        private:
            std::vector<DirectX::XMINT2> m_phermoneTrail;
            std::vector<DirectX::XMINT2> m_occupiedQuads; // TODOKO review if it should be set to enable checking for duplicates
            std::vector<PotentialChargeInformation> m_potentialsVsOther;
            float m_range;
            float m_charge;
            DirectX::XMINT2 m_prevGridPos;
            DirectX::XMFLOAT3 m_position;
            DirectX::XMFLOAT3 m_wantedPosition; // This is so we dont have to check every time
            AIActorType m_actorType;
            bool m_static;
            bool m_usePhermonetrail;
        };
    }
}
