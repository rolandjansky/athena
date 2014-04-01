/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SubtractedDiscSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometrySurfaces/SubtractedDiscSurface.h"
//Gaudi
#include "GaudiKernel/MsgStream.h"
//STD
#include <iostream>
#include <iomanip>

// default constructor
Trk::SubtractedDiscSurface::SubtractedDiscSurface() :
  Trk::DiscSurface(),
  m_subtrVol(),
  m_shared(true)
{}

// copy constructor
Trk::SubtractedDiscSurface::SubtractedDiscSurface(const SubtractedDiscSurface& psf) :
  Trk::DiscSurface(psf),
  m_subtrVol(psf.m_subtrVol),
  m_shared(psf.m_shared)
{}

// copy constructor with shift
Trk::SubtractedDiscSurface::SubtractedDiscSurface(const SubtractedDiscSurface& psf, const Amg::Transform3D& shift) :
  Trk::DiscSurface(psf,shift),
  m_subtrVol(psf.m_subtrVol),
  m_shared(psf.m_shared)
{}

// constructor
Trk::SubtractedDiscSurface::SubtractedDiscSurface(const Trk::DiscSurface& ps, AreaExcluder* vol, bool shared) :
  Trk::DiscSurface(ps),
  m_subtrVol(Trk::SharedObject<AreaExcluder>(vol)),
  m_shared(shared)
{}

// destructor (will call destructor from base class which deletes objects)
Trk::SubtractedDiscSurface::~SubtractedDiscSurface()
{}

Trk::SubtractedDiscSurface& Trk::SubtractedDiscSurface::operator=(const Trk::SubtractedDiscSurface& psf){
  
  if (this!=&psf){
    Trk::DiscSurface::operator=(psf);
    m_subtrVol = psf.m_subtrVol;
    m_shared = psf.m_shared;
  }
  return *this;

} 

bool Trk::SubtractedDiscSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::SubtractedDiscSurface* sdsf = dynamic_cast<const Trk::SubtractedDiscSurface*>(&sf);
  if (!sdsf) return false;
  bool surfaceEqual = Trk::DiscSurface::operator==(sf);
  bool sharedEqual = (surfaceEqual) ? (shared() == sdsf->shared()) : false; 
  return sharedEqual;
}


