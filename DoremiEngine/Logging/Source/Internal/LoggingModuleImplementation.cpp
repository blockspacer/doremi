#include <Internal/LoggingModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Internal/SubmoduleManagerImpl.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        LoggingModuleImplementation::LoggingModuleImplementation(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_submoduleManager(nullptr), m_sharedContext(p_sharedContext)
        {
        }

        LoggingModuleImplementation::~LoggingModuleImplementation()
        {
            if(m_submoduleManager != nullptr)
            {
                delete m_submoduleManager;
            }
        }

        void LoggingModuleImplementation::Startup()
        {
#ifdef NO_LOGGER
            printf("Logger system haas been disabled by preprocessor entry 'NO_LOGGER'\n");
#else
            printf("Logger system is now online, all other logging will be directed to the logfile.\n");
#endif
            m_submoduleManager = new SubmoduleManagerImpl();
            static_cast<SubmoduleManagerImpl*>(m_submoduleManager)->Initialize(*this);
        }

        void LoggingModuleImplementation::Shutdown() {}

        SubmoduleManager& LoggingModuleImplementation::GetSubModuleManager() const { return *m_submoduleManager; }

        const DoremiEngine::Core::SharedContext& LoggingModuleImplementation::GetEngineSharedContext() const { return m_sharedContext; }
    }
}

DoremiEngine::Logging::LoggingModule* CreateLoggingModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Logging::LoggingModule* logger = new DoremiEngine::Logging::LoggingModuleImplementation(p_sharedContext);
    return logger;
}