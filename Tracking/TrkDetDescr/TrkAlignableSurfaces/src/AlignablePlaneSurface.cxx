/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// AlignablePlaneSurface.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkAlignableSurfaces/AlignablePlaneSurface.h"
#include <iostream>

Trk::AlignablePlaneSurface::AlignablePlaneSurface()
  : Trk::PlaneSurface()
  , Trk::AlignableSurface()
  , m_nominalSurface(nullptr)
{}

Trk::AlignablePlaneSurface::AlignablePlaneSurface(const Trk::PlaneSurface& psf, Amg::Transform3D* htrans)
  : Trk::PlaneSurface(psf)
  , Trk::AlignableSurface()
  , m_nominalSurface(&psf)
{
  if (htrans) {
    Surface::m_transform = std::unique_ptr<Amg::Transform3D>(htrans);
    Trk::Surface::m_center =
      std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->translation());
    Trk::Surface::m_normal = std::make_unique<Amg::Vector3D>(
      Trk::Surface::m_transform->rotation().col(2));
  } else {
    Surface::m_transform =
      std::make_unique<Amg::Transform3D>(m_nominalSurface->transform());
    Trk::Surface::m_center =
      std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->translation());
    Trk::Surface::m_normal = std::make_unique<Amg::Vector3D>(
      Trk::Surface::m_transform->rotation().col(2));
  }
}

Trk::AlignablePlaneSurface::AlignablePlaneSurface(const Trk::AlignablePlaneSurface& apsf)
  : Trk::PlaneSurface(apsf)
  , Trk::AlignableSurface()
  , m_nominalSurface(apsf.m_nominalSurface)
{}

Trk::AlignablePlaneSurface::~AlignablePlaneSurface() {}

Trk::AlignablePlaneSurface&
Trk::AlignablePlaneSurface::operator=(const Trk::AlignablePlaneSurface& apsf)
{

  if (this != &apsf) {
    Trk::PlaneSurface::operator=(apsf);
    m_nominalSurface = apsf.m_nominalSurface;
  }
  return *this;
}

bool
Trk::AlignablePlaneSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::AlignablePlaneSurface* apsf = dynamic_cast<const Trk::AlignablePlaneSurface*>(&sf);
  if (!apsf)
    return false;
  bool transfEqual = transform().isApprox(apsf->transform());
  bool boundsEqual = (transfEqual) ? (bounds() == apsf->bounds()) : false;
  return boundsEqual;
}

void
Trk::AlignablePlaneSurface::addAlignmentCorrection(Amg::Transform3D& corr)
{
  Trk::Surface::m_transform = std::make_unique<Amg::Transform3D>((*Trk::Surface::m_transform) * corr);
  Trk::Surface::m_center = std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->translation());
  Trk::Surface::m_normal = std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->rotation().col(2));
}

void
Trk::AlignablePlaneSurface::setAlignmentCorrection(Amg::Transform3D& corr)
{
  Trk::Surface::m_transform = std::make_unique<Amg::Transform3D>((m_nominalSurface->transform()) * corr);
  Trk::Surface::m_center = std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->translation());
  Trk::Surface::m_normal = std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->rotation().col(2));
}

void
Trk::AlignablePlaneSurface::setAlignableTransform(Amg::Transform3D& trans)
{
  Trk::Surface::m_transform = std::make_unique<Amg::Transform3D>(trans);
  Trk::Surface::m_center = std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->translation());
  Trk::Surface::m_normal = std::make_unique<Amg::Vector3D>(Trk::Surface::m_transform->rotation().col(2));
}
