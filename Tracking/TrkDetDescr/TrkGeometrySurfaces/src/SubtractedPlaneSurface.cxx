/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedPlaneSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>

// default constructor
Trk::SubtractedPlaneSurface::SubtractedPlaneSurface() :
  Trk::PlaneSurface(),
  m_subtrVol(),
  m_shared(true)
{}

// copy constructor
Trk::SubtractedPlaneSurface::SubtractedPlaneSurface(const SubtractedPlaneSurface& psf) :
  Trk::PlaneSurface(psf),
  m_subtrVol(psf.m_subtrVol),
  m_shared(psf.m_shared)
{}

// copy constructor with shift
Trk::SubtractedPlaneSurface::SubtractedPlaneSurface(const SubtractedPlaneSurface& psf, const Amg::Transform3D& transf) :
  Trk::PlaneSurface(psf, transf),
  m_subtrVol(psf.m_subtrVol),
  m_shared(psf.m_shared)
{}

// constructor
Trk::SubtractedPlaneSurface::SubtractedPlaneSurface(const Trk::PlaneSurface& ps, AreaExcluder* vol, bool shared) :
  Trk::PlaneSurface(ps),
  m_subtrVol(vol),
  m_shared(shared)
{}

// destructor (will call destructor from base class which deletes objects)
Trk::SubtractedPlaneSurface::~SubtractedPlaneSurface()
{}

Trk::SubtractedPlaneSurface& Trk::SubtractedPlaneSurface::operator=(const Trk::SubtractedPlaneSurface& psf){
  
  if (this!=&psf){
    Trk::PlaneSurface::operator=(psf);
    m_subtrVol = psf.m_subtrVol;
    m_shared = psf.m_shared;
  }
  return *this;

} 

bool Trk::SubtractedPlaneSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::SubtractedPlaneSurface* spsf = dynamic_cast<const Trk::SubtractedPlaneSurface*>(&sf);
  if (!spsf) return false;
    bool surfaceEqual = Trk::PlaneSurface::operator==(sf);
    bool sharedEqual = (surfaceEqual) ? (shared() == spsf->shared()) : false; 
    return sharedEqual;
}


