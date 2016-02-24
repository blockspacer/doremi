#include <CameraClasses/ThirdPersonCamera.hpp>
#include <PlayerHandlerClient.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <InputHandlerClient.hpp>
// Engine
#include <DoremiEngine/Graphic/Include/Interface/Camera/Camera.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/CameraManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/SubModuleManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>

namespace Doremi
{
    namespace Core
    {
        ThirdPersonCamera::ThirdPersonCamera(DoremiEngine::Graphic::Camera* p_camera, const float& p_distanceFromPlayer, const float& p_minAngle, const float& p_maxAngle)
            : m_camera(p_camera), m_distanceFromPlayer(p_distanceFromPlayer), m_maxAngle(p_maxAngle), m_minAngle(p_minAngle)
        {
            m_angle = (p_minAngle + p_maxAngle) * 0.5f;
        }

        ThirdPersonCamera::~ThirdPersonCamera() {}

        void ThirdPersonCamera::Update()
        {
            using namespace DirectX;

            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            // If player exists
            if(!t_playerHandler->PlayerExists())
            {
                return;
            }

            EntityID playerID = t_playerHandler->GetPlayerEntityID();
            // m_angle = 0;
            TransformComponent* playerTransform = EntityHandler::GetInstance().GetComponentFromStorage<TransformComponent>(playerID);
            XMFLOAT4 orientation = playerTransform->rotation;
            XMVECTOR position = XMLoadFloat3(&playerTransform->position);
            XMVECTOR quater = XMLoadFloat4(&orientation);
            XMVECTOR up = XMLoadFloat3(&XMFLOAT3(0, 1, 0)); // The upvector of the character should always be this
            XMVECTOR forward = XMLoadFloat3(&XMFLOAT3(0, 0, 1)); // Standard forward vector
            forward = XMVector3Rotate(forward, quater); // Rotate forward vector with player orientation TODOKO Should maybe disregard some rotations?
            forward = XMVector3Normalize(forward);
            XMVECTOR right = XMVector3Cross(forward, up);

            XMVECTOR specialRot = XMQuaternionRotationAxis(right, m_angle);
            // XMQuaternionMultiply(quater, specialRot) is total rotation in one quternion
            forward = XMVector3Rotate(forward, specialRot); // Rotate forward vector with angle around local x vector
            forward = XMVector3Normalize(forward);
            XMVECTOR cameraPosition = position - forward * m_distanceFromPlayer;
            // float offsetY = 5 * cosf(m_angle); // Get offset in Y
            XMVECTOR realUp =
                XMVector3Cross(right, forward); // Get the real upvector to have the camera focus on a point a bit above the players focus
            realUp = XMVector3Normalize(realUp);
            cameraPosition += realUp * 3; // Set offset in Y TODOCONFIG
            XMMATRIX worldMatrix = XMMatrixTranspose(XMMatrixLookAtLH(cameraPosition, position + realUp * 3, up)); // TODOCONFIG
            // XMVECTOR forwardQuater = XMQuaternionRotationNormal(forward,0);
            XMFLOAT4X4 viewMat;
            XMStoreFloat4x4(&viewMat, worldMatrix);
            m_camera->SetViewMatrix(viewMat);
            XMFLOAT3 t_camPos;
            XMStoreFloat3(&t_camPos, cameraPosition);

            m_camera->SetCameraPosition(t_camPos);
        }

        void ThirdPersonCamera::UpdateInput(const double& p_dt)
        {
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            InputHandlerClient* t_inputHandler = t_playerHandler->GetInputHandler();

            float wantedAngle = m_angle; // the angle we want to reach
            wantedAngle -= t_inputHandler->GetMouseMovementY() * 0.001;
            if(wantedAngle < m_maxAngle && wantedAngle > m_minAngle)
            {
                m_angle = wantedAngle;
            }
        }
    }
}