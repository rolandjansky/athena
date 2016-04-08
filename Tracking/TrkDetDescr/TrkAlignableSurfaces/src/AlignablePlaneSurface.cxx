/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlignablePlaneSurface.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkAlignableSurfaces/AlignablePlaneSurface.h"
#include <iostream>

Trk::AlignablePlaneSurface::AlignablePlaneSurface() :
 Trk::PlaneSurface(),
 Trk::AlignableSurface(),
 m_nominalSurface(0)
{}

Trk::AlignablePlaneSurface::AlignablePlaneSurface(const Trk::PlaneSurface& psf, Amg::Transform3D* htrans) :
 Trk::PlaneSurface(psf),
 Trk::AlignableSurface(),
 m_nominalSurface(&psf)
{
  if (htrans) Surface::m_transform = htrans;
  else Surface::m_transform = new Amg::Transform3D(m_nominalSurface->transform());
}

Trk::AlignablePlaneSurface::AlignablePlaneSurface(const Trk::AlignablePlaneSurface& apsf) :
 Trk::PlaneSurface(apsf),
 Trk::AlignableSurface(),
 m_nominalSurface(apsf.m_nominalSurface)
{}

Trk::AlignablePlaneSurface::~AlignablePlaneSurface()
{}

Trk::AlignablePlaneSurface& Trk::AlignablePlaneSurface::operator=(const Trk::AlignablePlaneSurface& apsf)
{
  
  if (this!=&apsf){
    Trk::PlaneSurface::operator=(apsf);
    m_nominalSurface = apsf.m_nominalSurface; 
  }
  return *this;
} 

bool Trk::AlignablePlaneSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::AlignablePlaneSurface* apsf = dynamic_cast<const Trk::AlignablePlaneSurface*>(&sf);
  if (!apsf) return false;
    bool transfEqual = transform().isApprox(apsf->transform());
    bool boundsEqual = (transfEqual) ? (bounds() == apsf->bounds()) : false;
  return boundsEqual;
}

void Trk::AlignablePlaneSurface::addAlignmentCorrection(Amg::Transform3D& corr) const
{
    Amg::Transform3D* delTransform = Trk::Surface::m_transform ;
    Trk::Surface::m_transform = new Amg::Transform3D((*Trk::Surface::m_transform)*corr);
    delete delTransform;
    delete Trk::Surface::m_center;
    m_center = 0;
    delete Trk::Surface::m_normal;
    m_normal =0;
}

void Trk::AlignablePlaneSurface::setAlignmentCorrection(Amg::Transform3D& corr) const
{
  Amg::Transform3D* delTransform = Trk::Surface::m_transform ;
  Trk::Surface::m_transform = new Amg::Transform3D((m_nominalSurface->transform())*corr);
  delete delTransform;
  delete Trk::Surface::m_center;
  m_center = 0;
  delete Trk::Surface::m_normal;
  m_normal =0;
}

void Trk::AlignablePlaneSurface::setAlignableTransform(Amg::Transform3D& trans) const
{
  delete Trk::Surface::m_transform ;
  Trk::Surface::m_transform  = new Amg::Transform3D(trans);
  delete Trk::Surface::m_center;
  m_center = 0;
  delete Trk::Surface::m_normal;
  m_normal =0;
  
}

