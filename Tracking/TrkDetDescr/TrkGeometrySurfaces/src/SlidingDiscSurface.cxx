/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SlidingDiscSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometrySurfaces/SlidingDiscSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/DiscBounds.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

const Trk::NoBounds Trk::DiscSurface::s_boundless;

// default constructor
Trk::SlidingDiscSurface::SlidingDiscSurface()
  : Trk::DiscSurface()
  , m_depth()
  , m_etaBin()
  , m_align()
{}

// copy constructor
Trk::SlidingDiscSurface::SlidingDiscSurface(const SlidingDiscSurface& dsf)
  : Trk::DiscSurface(dsf)
  , m_depth(new std::vector<float>(*(dsf.m_depth)))
  , m_etaBin(dsf.m_etaBin->clone())
  , m_align(dsf.m_align ? new Amg::Transform3D(*dsf.m_align) : nullptr)
{}

// copy constructor with shift
Trk::SlidingDiscSurface::SlidingDiscSurface(const SlidingDiscSurface& dsf, const Amg::Transform3D& transf)
  : Trk::DiscSurface(dsf, transf)
  , m_depth(new std::vector<float>(*(dsf.m_depth)))
  , m_etaBin(dsf.m_etaBin->clone())
  , m_align(dsf.m_align ? new Amg::Transform3D(*dsf.m_align) : nullptr)
{}

// constructor
Trk::SlidingDiscSurface::SlidingDiscSurface(const Trk::DiscSurface& dsf,
                                            Trk::BinUtility* bu,
                                            const std::vector<float>* offset,
                                            Amg::Transform3D* align)
  : Trk::DiscSurface(dsf)
  , m_depth(offset)
  , m_etaBin(bu)
  , m_align(align)
{}

// destructor (will call destructor from base class which deletes objects)
Trk::SlidingDiscSurface::~SlidingDiscSurface()
{
  delete m_depth;
  delete m_etaBin;
  delete m_align;
}

Trk::SlidingDiscSurface&
Trk::SlidingDiscSurface::operator=(const SlidingDiscSurface& dsf)
{
  if (this != &dsf) {
    Trk::DiscSurface::operator=(dsf);
    delete m_depth;
    m_depth = new std::vector<float>(*(dsf.m_depth));
    delete m_etaBin;
    m_etaBin = dsf.m_etaBin->clone();
    delete m_align;
    m_align = dsf.m_align ? new Amg::Transform3D(*dsf.m_align) : nullptr;
  }
  return *this;
}

bool
Trk::SlidingDiscSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::SlidingDiscSurface* dsf = dynamic_cast<const Trk::SlidingDiscSurface*>(&sf);
  if (!dsf)
    return false;
  if (this == dsf)
    return true;
  bool transfEqual(transform().isApprox(dsf->transform(), 10e-8));
  bool centerEqual = (transfEqual) ? (center() == dsf->center()) : false;
  bool boundsEqual = (centerEqual) ? (bounds() == dsf->bounds()) : false;
  return boundsEqual;
}

void
Trk::SlidingDiscSurface::localToGlobal(const Amg::Vector2D& locpos, const Amg::Vector3D&, Amg::Vector3D& glopos) const
{
  // create the position in the local 3d frame
  Amg::Vector3D loc3D0(locpos[Trk::locR] * cos(locpos[Trk::locPhi]), locpos[Trk::locR] * sin(locpos[Trk::locPhi]), 0.);
  // correct for alignment, retrieve offset correction
  Amg::Transform3D t0 = m_align ? m_align->inverse() * transform() : transform();
  float offset = m_depth ? (*m_depth)[m_etaBin->bin(t0 * loc3D0)] : 0.;
  Amg::Vector3D loc3Dframe(
    locpos[Trk::locR] * cos(locpos[Trk::locPhi]), locpos[Trk::locR] * sin(locpos[Trk::locPhi]), offset);
  // transport it to the globalframe
  glopos = Trk::Surface::transform() * loc3Dframe;
}

/** local<->global transformation in case of polar local coordinates */
bool
Trk::SlidingDiscSurface::globalToLocal(const Amg::Vector3D& glopos, const Amg::Vector3D&, Amg::Vector2D& locpos) const
{
  Amg::Vector3D loc3D0 = m_align ? m_align->inverse() * glopos : glopos; // used to retrieve localEta bin
  Amg::Vector3D loc3Dframe(Trk::Surface::transform().inverse() * glopos);
  locpos = Amg::Vector2D(loc3Dframe.perp(), loc3Dframe.phi());
  return (fabs(loc3Dframe.z() - (*m_depth)[m_etaBin->bin(loc3D0)]) <= s_onSurfaceTolerance);
}

bool
Trk::SlidingDiscSurface::isOnSurface(const Amg::Vector3D& glopo,
                                     Trk::BoundaryCheck bchk,
                                     double tol1,
                                     double tol2) const
{
  Amg::Vector3D loc3D0 = m_align ? m_align->inverse() * glopo : glopo; // used to retrieve localEta bin
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopo;
  float offset = (*m_depth)[m_etaBin->bin(loc3D0)];
  if (fabs(loc3Dframe.z() - offset) > (s_onSurfaceTolerance + tol1))
    return false;
  return (bchk ? bounds().inside(Amg::Vector2D(loc3Dframe.perp(), loc3Dframe.phi()), tol1, tol2) : true);
}

/** distance to surface, calculated within localEta bin only */
Trk::DistanceSolution
Trk::SlidingDiscSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  double tol = 0.001;

  // retrieve localEta bin using current position
  Amg::Vector3D loc3D0 = m_align ? m_align->inverse() * pos : pos; // used to retrieve localEta bin
  float offset = (*m_depth)[m_etaBin->bin(loc3D0)];

  // slide surface
  Amg::Vector3D N = normal();
  Amg::Vector3D C = center() + offset * N;

  double S = C.dot(N);
  double b = S < 0. ? -1 : 1;
  double A = b * dir.dot(N);
  double d = (pos - C).dot(N); // distance to surface

  if (A == 0.) { // direction parallel to surface
    if (fabs(d) < tol) {
      return Trk::DistanceSolution(1, 0., true, 0.);
    } 
      return Trk::DistanceSolution(0, d, true, 0.);
    
  }

  double D = b * (S - (pos.dot(N))) / A;

  return Trk::DistanceSolution(1, d, true, D);
}

Trk::DistanceSolution
Trk::SlidingDiscSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos,
                                                      const Amg::Vector3D& dir,
                                                      bool /*bound*/) const
{
  return straightLineDistanceEstimate(pos, dir);
}
