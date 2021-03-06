#pragma once

namespace Doremi
{
    namespace Core
    {
        /**
            TODOCM make sure to give cred for this func! and other stuff ofc
            http://gafferongames.com/networking-for-game-programmers/reliability-and-flow-control/
        */
        static bool sequence_more_recent(uint8_t s1, uint8_t s2, uint32_t max)
        {
            // If s1 is larger and the difference is less then half
            // meaning if they're not wrapped, the differnce should be less then half for it to be more recent
            // or
            //  If s2 is the larger one, and their difference is larger then half
            // meaning if they're wrapped, the difference should be more then half for it to be wrapped

            return (s1 > s2) && (s1 - s2 <= max / 2) || (s2 > s1) && (s2 - s1 > max / 2);
        }

        // TODOCM maybe change from uint8 to bigger value for bigger chains
        static int16_t sequence_difference(uint8_t s1, uint8_t s2, unsigned int max)
        {
            // If s1 is larger and the difference is less then half
            // meaning if they're not wrapped, the differnce should be less then half for it to be more recent
            // or
            //  If s2 is the larger one, and their difference is larger then half
            // meaning if they're wrapped, the difference should be more then half for it to be wrapped

            if(s1 > s2)
            {
                if(sequence_more_recent(s1, s2, max))
                {
                    return s1 - s2;
                }
                else
                {
                    return -(max - s1 + s2 + 1);
                }
            }
            else if(s2 > s1) // TODOCM check if this really is right
            {
                if(sequence_more_recent(s1, s2, max))
                {
                    return max + 1 - s2 + s1;
                }
                else
                {
                    return s1 - s2;
                }
            }
            else
            {
                // std::cout << "Snapshot sequence is same, something is wrong" << std::endl; // TODOCM remove maybe
                return 0;
            }
        }
    }
}
