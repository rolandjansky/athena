/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedCylinderSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometrySurfaces/SubtractedCylinderSurface.h"
//STD
#include <iostream>
#include <iomanip>

// default constructor
Trk::SubtractedCylinderSurface::SubtractedCylinderSurface() :
  Trk::CylinderSurface(),
  m_subtrVol(),
  m_shared(true)
{}

// copy constructor
Trk::SubtractedCylinderSurface::SubtractedCylinderSurface(const SubtractedCylinderSurface& psf) :
  Trk::CylinderSurface(psf),
  m_subtrVol(psf.m_subtrVol),
  m_shared(psf.m_shared)
{}

// copy constructor with shift
Trk::SubtractedCylinderSurface::SubtractedCylinderSurface(const SubtractedCylinderSurface& psf, const Amg::Transform3D& transf) :
  Trk::CylinderSurface(psf, transf),
  m_subtrVol(psf.m_subtrVol),
  m_shared(psf.m_shared)
{}

// constructor
Trk::SubtractedCylinderSurface::SubtractedCylinderSurface(const Trk::CylinderSurface& ps, AreaExcluder* vol, bool shared) :
  Trk::CylinderSurface(ps),
  m_subtrVol(Trk::SharedObject<Trk::AreaExcluder>(vol)),
  m_shared(shared)
{}

// destructor (will call destructor from base class which deletes objects)
Trk::SubtractedCylinderSurface::~SubtractedCylinderSurface()
{}

Trk::SubtractedCylinderSurface& Trk::SubtractedCylinderSurface::operator=(const Trk::SubtractedCylinderSurface& psf){

  if (this!=&psf){
    Trk::CylinderSurface::operator=(psf);
    m_subtrVol = psf.m_subtrVol;
    m_shared = psf.m_shared;
  }
  return *this;

}

bool Trk::SubtractedCylinderSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::SubtractedCylinderSurface* scsf = dynamic_cast<const Trk::SubtractedCylinderSurface*>(&sf);
  if (!scsf) return false;
  bool surfaceEqual = Trk::CylinderSurface::operator==(sf);
  bool sharedEqual  = (surfaceEqual) ? (shared() == scsf->shared()) : false;
  return sharedEqual;
}

