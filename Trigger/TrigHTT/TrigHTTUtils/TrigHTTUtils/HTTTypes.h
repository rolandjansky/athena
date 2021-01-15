/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTTYPES_H
#define HTTTYPES_H

/*
 * HTTTypes.h: This file declares several typedefs and enums used throughout HTTSim
 *
 * Created: June 12th, 2019
 * Author: Riley Xu
 * Email: rixu@cern.ch
 */


#include <stdint.h>
#include <vector>
#include <ostream>

/*****************************************************************************/
/* Typedefs                                                                  */
/*****************************************************************************/

typedef int32_t pid_t;      // pattern id
typedef int32_t ssid_t;     // super-strip id (TSP or DC)
typedef int32_t module_t;   // module id
typedef int32_t sector_t;   // sector number
    // Use a custom type to enforce consistency and easily switch to int64_t or other if needed.
    // Objects like map<sector_t, module_t> are also self-documenting instead of map<int, int>.
    // Use signed integers to easily allow wildcards or error codes using negative values, without
    // need of casting or confusion.
    //
    // TODO: FTK-inherited code need to be updated to use these types

typedef uint16_t layer_bitmask_t; // bitmask that stores at least as many bits as layers

typedef std::vector<ssid_t> pattern_t; // Stores one ssid per layer



/*****************************************************************************/
/* Enums                                                                     */
/*****************************************************************************/

//To label the origin of HTTHits
enum class SiliconTech { strip, pixel, nTechs, undefined };
//The detector zone that the HTTHit was in
enum class DetectorZone { barrel, endcap, nZones, undefined };
// An Enum to hold the stage of the track
enum class TrackStage{FIRST,SECOND};


inline std::ostream & operator<<(std::ostream & os, SiliconTech t)
{
    switch (t)
    {
        case SiliconTech::strip: return (os << "strip");
        case SiliconTech::pixel: return (os << "pixel");
        default: return (os << "undefined");
    }
}

inline std::ostream & operator<<(std::ostream & os, DetectorZone t)
{
    switch (t)
    {
        case DetectorZone::barrel: return (os << "barrel");
        case DetectorZone::endcap: return (os << "endcap");
        default: return (os << "undefined");
    }
}


#endif // HTTTYPES_H
