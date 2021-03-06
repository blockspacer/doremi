#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        /**
        This is used when creating particle emitters
        Ensure that ALL variables are filled. This
        particular particle system spawns a grid of
        particles which are emitted in a predictable
        and deterministic manner.
        NO default values are assigned. YOU HAVE BEEN
        WARNED!... Really, this is important. Set all
        variables.*/
        struct ParticleEmitterData
        {
            bool m_active;
            // Position of emitter
            XMFLOAT3 m_position;
            // Dimensions of emitter UNUSED TODOJB remove?
            XMFLOAT2 m_dimensions;
            // Direction in which the emitter fires particles
            XMFLOAT4 m_direction;
            // Pressure with which particles are launched (arbitrary unit)
            float m_launchPressure;
            // Time between particles
            float m_emissionRate;
            // How dense the particles are packed UNUSED TODOJB remove?
            float m_density;
            /* The number of particles spawned along the local X axis
            Currently even numbers are rounded up*/
            int m_numParticlesX;
            /* The number of particles spawned along the local Y axis
            Currently even numbers are rounded up*/
            int m_numParticlesY;
            // How wide and high the emission area is in radians
            XMFLOAT2 m_emissionAreaDimensions;
            // How big particles are (think of them as spheres) (This is actually not that truthful...)
            float m_size;
        };
        class FluidManager
        {
        public:
            // virtual void CreateFluid(int p_id) = 0;
            // virtual void CreateFluidParticles(int p_id, vector<XMFLOAT3>& p_positions, vector<XMFLOAT3>& p_velocities, vector<int>& p_indices) = 0;
            virtual void GetParticlePositions(int p_id, vector<XMFLOAT3>& o_positions) = 0;

            /**
            Returns a vector with positions of all recently removed particles*/
            virtual const vector<XMFLOAT3>& GetRemovedParticlesPositions(int p_id) = 0;

            /**
            Creates a new particle system. Particle system will be a pressure emitter ONLY*/
            virtual void CreateParticleEmitter(int p_id, ParticleEmitterData p_data) = 0;
            /**
            Used to update the entire particle system*/
            virtual void SetParticleEmitterData(int p_id, ParticleEmitterData p_data) = 0;

            /**
            Gets all drains hit by a particle of the specified particle system*/
            virtual vector<int> GetDrainsHit(int p_id) = 0;
        };
    }
}