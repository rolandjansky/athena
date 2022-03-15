/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHTTOBJECTS_HTTTYPES_H
#define TRIGHTTOBJECTS_HTTTYPES_H

#include <iostream>

typedef int32_t pid_t;      // pattern id
typedef int32_t module_t;   // module id
typedef int32_t sector_t;   // sector number
typedef uint32_t layer_bitmask_t; // bitmask that stores at least as many bits as layers

//To label the origin of HTTHits
enum class SiliconTech { strip, pixel, nTechs, undefined };

//The detector zone that the HTTHit was in
enum class DetectorZone { barrel, posEndcap, negEndcap, nZones, undefined };

// Enum to hold the different types of hits in HTTOfflineHit
enum class OfflineHitType { Measurement, InertMaterial, BremPoint, Scatterer, Perigee, Outlier, Hole, Other };

// An Enum to hold the stage of the track
enum class TrackStage { FIRST, SECOND };

// Enum to hold the type of correction for hits on track
enum class TrackCorrType { None = 0, First = 1, Second = 2 };


std::ostream& operator<<(std::ostream& os, SiliconTech t);
std::ostream& operator<<(std::ostream& os, DetectorZone t);

#endif