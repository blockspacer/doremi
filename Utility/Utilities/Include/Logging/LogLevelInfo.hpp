#pragma once
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            struct LogLevelInfo
            {
                explicit LogLevelInfo(const std::string& p_name) : name(p_name) {}
                LogLevelInfo() {}
                std::string name;
            };
        }
    }
}