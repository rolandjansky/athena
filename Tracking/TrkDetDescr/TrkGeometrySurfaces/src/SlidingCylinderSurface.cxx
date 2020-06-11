/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SlidingCylinderSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometrySurfaces/SlidingCylinderSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/CylinderBounds.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

// default constructor
Trk::SlidingCylinderSurface::SlidingCylinderSurface()
  : Trk::CylinderSurface()
  , m_depth()
  , m_etaBin()
  , m_align()
{}

// copy constructor
Trk::SlidingCylinderSurface::SlidingCylinderSurface(const SlidingCylinderSurface& dsf)
  : Trk::CylinderSurface(dsf)
  , m_depth(new std::vector<float>(*(dsf.m_depth)))
  , m_etaBin(dsf.m_etaBin->clone())
  , m_align(dsf.m_align ? new Amg::Transform3D(*dsf.m_align) : nullptr)
{}

// copy constructor with shift
Trk::SlidingCylinderSurface::SlidingCylinderSurface(const SlidingCylinderSurface& dsf, const Amg::Transform3D& transf)
  : Trk::CylinderSurface(dsf, transf)
  , m_depth(new std::vector<float>(*(dsf.m_depth)))
  , m_etaBin(dsf.m_etaBin->clone())
  , m_align(dsf.m_align ? new Amg::Transform3D(*dsf.m_align) : nullptr)
{}

// constructor
Trk::SlidingCylinderSurface::SlidingCylinderSurface(const Trk::CylinderSurface& dsf,
                                                    Trk::BinUtility* bu,
                                                    const std::vector<float>* offset,
                                                    Amg::Transform3D* align)
  : Trk::CylinderSurface(dsf)
  , m_depth(offset)
  , m_etaBin(bu)
  , m_align(align)
{}

// destructor (will call destructor from base class which deletes objects)
Trk::SlidingCylinderSurface::~SlidingCylinderSurface()
{
  delete m_depth;
  delete m_etaBin;
  delete m_align;
}

Trk::SlidingCylinderSurface&
Trk::SlidingCylinderSurface::operator=(const SlidingCylinderSurface& dsf)
{
  if (this != &dsf) {
    Trk::CylinderSurface::operator=(dsf);
    delete m_depth;
    m_depth = new std::vector<float>(*(dsf.m_depth));
    delete m_etaBin;
    m_etaBin = dsf.m_etaBin->clone();
    delete m_align;
    m_align = (dsf.m_align ? new Amg::Transform3D(*dsf.m_align) : nullptr);
  }
  return *this;
}

bool
Trk::SlidingCylinderSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::SlidingCylinderSurface* dsf = dynamic_cast<const Trk::SlidingCylinderSurface*>(&sf);
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
Trk::SlidingCylinderSurface::localToGlobal(const Amg::Vector2D& locpos,
                                           const Amg::Vector3D&,
                                           Amg::Vector3D& glopos) const
{
  // create the position in the local 3d frame
  double r0 = bounds().r();
  double phi0 = locpos[Trk::locRPhi] / r0;
  Amg::Vector3D loc3D0(r0 * cos(phi0), r0 * sin(phi0), locpos[Trk::locZ]);
  // correct for alignment, retrieve offset correction
  Amg::Transform3D t0 = m_align ? m_align->inverse() * transform() : transform();
  float offset = m_depth ? (*m_depth)[m_etaBin->bin(t0 * loc3D0)] : 0.;
  double r = r0 + offset;
  double phi = locpos[Trk::locRPhi] / r;
  Amg::Vector3D loc3Dframe(r * cos(phi), r * sin(phi), locpos[Trk::locZ]);
  // transport it to the globalframe
  glopos = Trk::Surface::transform() * loc3Dframe;
}

/** local<->global transformation in case of polar local coordinates */
bool
Trk::SlidingCylinderSurface::globalToLocal(const Amg::Vector3D& glopos,
                                           const Amg::Vector3D&,
                                           Amg::Vector2D& locpos) const
{
  // get the transform & transform global position into cylinder frame
  // transform it to the globalframe: CylinderSurfaces are allowed to have 0 pointer transform
  double radius = 0.;
  double inttol = bounds().r() * 0.0001;
  if (inttol < 0.01)
    inttol = 0.01;
  // realign to find local eta bin
  Amg::Vector3D loc3D0 = m_align ? (m_align->inverse() * glopos) : glopos;
  float offset = (*m_depth)[m_etaBin->bin(loc3D0)];
  // do the transformation or not
  if (Trk::Surface::m_transform) {
    const Amg::Transform3D& surfaceTrans = transform();
    Amg::Transform3D inverseTrans(surfaceTrans.inverse());
    Amg::Vector3D loc3Dframe(inverseTrans * glopos);
    locpos = Amg::Vector2D((bounds().r() + offset) * loc3Dframe.phi(), loc3Dframe.z());
    radius = loc3Dframe.perp();
  } else {
    locpos = Amg::Vector2D((bounds().r() + offset) * glopos.phi(), glopos.z());
    radius = glopos.perp();
  }
  // return true or false
  return (fabs(radius - bounds().r() - offset) <= inttol);
}

bool
Trk::SlidingCylinderSurface::isOnSurface(const Amg::Vector3D& glopo,
                                         Trk::BoundaryCheck bchk,
                                         double tol1,
                                         double tol2) const
{
  Amg::Vector3D loc3D0 = m_align ? m_align->inverse() * glopo : glopo;
  Amg::Vector3D loc3Dframe = m_transform ? (transform().inverse()) * glopo : glopo;
  float offset = (*m_depth)[m_etaBin->bin(loc3D0)];
  // recalculate r to match bounds
  Amg::Vector3D loc3Dbase((loc3Dframe.perp() - offset) * cos(loc3Dframe.phi()),
                          (loc3Dframe.perp() - offset) * sin(loc3Dframe.phi()),
                          loc3Dframe.z());

  return (bchk ? bounds().inside3D(loc3Dbase, tol1, tol2) : true);
}

/** distance to surface */
Trk::DistanceSolution
Trk::SlidingCylinderSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  double tol = 0.001;

  // retrieve localEta bin using current position
  Amg::Vector3D loc3D0 = m_align ? m_align->inverse() * pos : pos; // used to retrieve localEta bin
  float offset = m_depth ? (*m_depth)[m_etaBin->bin(loc3D0)] : 0.;

  const Amg::Vector3D& X = center(); // point
  const Amg::Vector3D& S = normal(); // vector

  double radius = bounds().r() + offset;
  double sp = pos.dot(S);
  double sc = X.dot(S);
  double dp = dir.dot(S);
  Amg::Vector3D dx = X - pos - (sc - sp) * S; // vector
  Amg::Vector3D ax = dir - dp * S;            // vector

  double A = ax.dot(ax); // size of projected direction (squared)
  double B = ax.dot(dx); // dot product (->cos angle)
  double C = dx.dot(dx); // distance to axis (squared)
  double currDist = radius - sqrt(C);

  if (A == 0.) { // direction parallel to cylinder axis
    if (fabs(currDist) < tol) {
      return Trk::DistanceSolution(1, 0., true, 0.); // solution at surface
    } 
      return Trk::DistanceSolution(0, currDist, true, 0.); // point of closest approach without intersection
    
  }

  // minimal distance to cylinder axis
  double rmin = C - B * B / A < 0. ? 0. : sqrt(C - B * B / A);

  if (rmin > radius) { // no intersection
    double first = B / A;
    return Trk::DistanceSolution(0, currDist, true, first); // point of closest approach without intersection
  } 
    if (fabs(rmin - radius) < tol) { // tangential 'intersection' - return double solution
      double first = B / A;
      return Trk::DistanceSolution(2, currDist, true, first, first);
    } 
      double first = B / A - sqrt((radius - rmin) * (radius + rmin) / A);
      double second = B / A + sqrt((radius - rmin) * (radius + rmin) / A);
      if (first >= 0.) {
        return Trk::DistanceSolution(2, currDist, true, first, second);
      } if (second <= 0.) {
        return Trk::DistanceSolution(2, currDist, true, second, first);
      } // inside cylinder
        return Trk::DistanceSolution(2, currDist, true, second, first);
      
    
  
}

Trk::DistanceSolution
Trk::SlidingCylinderSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos,
                                                          const Amg::Vector3D& dir,
                                                          bool /*bound*/) const
{
  return straightLineDistanceEstimate(pos, dir);
}
