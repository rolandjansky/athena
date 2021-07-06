/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrkEventPrimitivs/SurfaceType.h
 */

#ifndef TRKEVENTPRIMITIVES_SURFACETYPE_H
#define TRKEVENTPRIMITIVES_SURFACETYPE_H

namespace Trk {
/** @enum SurfaceType
    This enumerator simplifies the persistency & calculations,
  */
enum class SurfaceType
{
  Cone = 0,
  Cylinder = 1,
  Disc = 2,
  Perigee = 3,
  Plane = 4,
  Line = 5,
  Curvilinear = 6,
  Other = 7
};

} // namespace Trk

#endif
