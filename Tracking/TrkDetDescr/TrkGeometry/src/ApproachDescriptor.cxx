/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ApproachDescriptor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometry/ApproachDescriptor.h"

const Trk::ApproachSurfaces* Trk::ApproachDescriptor::approachSurfaces(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const 
{
  // return the single approach surfaces
  if (m_approachSurfaces)
    return (m_approachSurfaces.get());
  // there's new surface given
  if (m_approachSurfaceArraySurface){
    Trk::Intersection asInter = m_approachSurfaceArraySurface->straightLineIntersection(pos,dir);
    if (asInter.valid)
      return (m_approachSurfaceArray->object(asInter.position));
  }
  // get the bin
  return (m_approachSurfaceArray->object(pos));
}

