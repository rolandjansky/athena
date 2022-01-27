/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SlidingCylinderSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkGeometrySurfaces/SlidingCylinderSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/CylinderBounds.h"

// Eigen
#include "GeoPrimitives/GeoPrimitives.h"
// STD
#include <cmath>



// constructor
Trk::SlidingCylinderSurface::SlidingCylinderSurface(const Trk::CylinderSurface& dsf,
                                                    const Trk::BinUtility & bu,
                                                    const std::vector<float> & offset)
  : Trk::CylinderSurface(dsf)
  , m_depth(offset)
  , m_etaBin(bu)
{}


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
  Amg::Vector3D loc3D0(r0 * std::cos(phi0), r0 * std::sin(phi0), locpos[Trk::locZ]);
  // correct for alignment, retrieve offset correction
  Amg::Transform3D t0 = transform();
  float offset = m_depth[m_etaBin.bin(t0 * loc3D0)];
  double r = r0 + offset;
  double phi = locpos[Trk::locRPhi] / r;
  Amg::Vector3D loc3Dframe(r * std::cos(phi), r * std::sin(phi), locpos[Trk::locZ]);
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
  const double inttol = std::max(bounds().r() * 0.0001, 0.01);
  // realign to find local eta bin
  const Amg::Vector3D& loc3D0 = glopos;
  float offset = m_depth[m_etaBin.bin(loc3D0)];
  // do the transformation or not
  if (Trk::Surface::m_transforms) {
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
  return (std::abs(radius - bounds().r() - offset) <= inttol);
}

bool
Trk::SlidingCylinderSurface::isOnSurface(const Amg::Vector3D& glopo,
                                         const Trk::BoundaryCheck& bchk,
                                         double tol1,
                                         double tol2) const
{
  const Amg::Vector3D& loc3D0 = glopo;
  Amg::Vector3D loc3Dframe = m_transforms ? (transform().inverse()) * glopo : glopo;
  float offset = m_depth[m_etaBin.bin(loc3D0)];
  // recalculate r to match bounds
  Amg::Vector3D loc3Dbase((loc3Dframe.perp() - offset) * std::cos(loc3Dframe.phi()),
                          (loc3Dframe.perp() - offset) * std::sin(loc3Dframe.phi()),
                          loc3Dframe.z());

  return (bchk ? bounds().inside3D(loc3Dbase, tol1, tol2) : true);
}

/** distance to surface */
Trk::DistanceSolution
Trk::SlidingCylinderSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  double tol = 0.001;

  // retrieve localEta bin using current position
  const Amg::Vector3D& loc3D0 = pos; // used to retrieve localEta bin
  float offset = m_depth[m_etaBin.bin(loc3D0)];

  const Amg::Vector3D& X = Trk::Surface::center(); // point
  const Amg::Vector3D& S = Trk::Surface::normal(); // vector

  double radius = bounds().r() + offset;
  double sp = pos.dot(S);
  double sc = X.dot(S);
  double dp = dir.dot(S);
  Amg::Vector3D dx = X - pos - (sc - sp) * S; // vector
  Amg::Vector3D ax = dir - dp * S;            // vector

  double A = ax.dot(ax); // size of projected direction (squared)
  double B = ax.dot(dx); // dot product (->cos angle)
  double C = dx.dot(dx); // distance to axis (squared)
  double currDist = radius - std::sqrt(C);
  const double bOverA{B/A};
  if (A == 0.) { // direction parallel to cylinder axis
    if (std::abs(currDist) < tol) {
      return {1, 0., true, 0.}; // solution at surface
    }
      return {0, currDist, true, 0.}; // point of closest approach without intersection

  }
  // minimal distance to cylinder axis
  double rmin = C - B * bOverA < 0. ? 0. : std::sqrt(C - B * bOverA);
  if (rmin > radius) { // no intersection
    return {0, currDist, true, bOverA}; // point of closest approach without intersection
  }
  if (std::abs(rmin - radius) < tol) { // tangential 'intersection' - return double solution
    return {2, currDist, true, bOverA, bOverA};
  }
  const double sqrtTerm = std::sqrt((radius - rmin) * (radius + rmin)/A) ;
  double first = bOverA - sqrtTerm;
  double second = bOverA + sqrtTerm;
  if (first >= 0.) {
    return {2, currDist, true, first, second};
  } if (second <= 0.) {
    return {2, currDist, true, second, first};
  } // inside cylinder
  return {2, currDist, true, second, first};
}

Trk::DistanceSolution
Trk::SlidingCylinderSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos,
                                                          const Amg::Vector3D& dir,
                                                          bool /*bound*/) const
{
  return straightLineDistanceEstimate(pos, dir);
}
