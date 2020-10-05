/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DiscSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/DiscSurface.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/DiscTrapezoidalBounds.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
//#include <iostream>
//#include <iomanip>

// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

const Trk::NoBounds Trk::DiscSurface::s_boundless;

// default constructor
Trk::DiscSurface::DiscSurface()
  : Trk::Surface()
  , m_bounds()
  , m_referencePoint(nullptr)
{}

// copy constructor
Trk::DiscSurface::DiscSurface(const DiscSurface& dsf)
  : Trk::Surface(dsf)
  , m_bounds(dsf.m_bounds)
  , m_referencePoint(nullptr)
{}

// copy constructor with shift
Trk::DiscSurface::DiscSurface(const DiscSurface& dsf, const Amg::Transform3D& transf)
  : Trk::Surface(dsf, transf)
  , m_bounds(dsf.m_bounds)
  , m_referencePoint(nullptr)
{}

// construct a disc with full phi coverage
Trk::DiscSurface::DiscSurface(Amg::Transform3D* htrans, double rmin, double rmax)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::DiscBounds(rmin, rmax))
  , m_referencePoint(nullptr)
{}

// construct a disc with given phi coverage
Trk::DiscSurface::DiscSurface(Amg::Transform3D* htrans, double rmin, double rmax, double hphisec)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::DiscBounds(rmin, rmax, hphisec))
  , m_referencePoint(nullptr)
{}

Trk::DiscSurface::DiscSurface(Amg::Transform3D* htrans,
                              double minhalfx,
                              double maxhalfx,
                              double maxR,
                              double minR,
                              double avephi,
                              double stereo)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::DiscTrapezoidalBounds(minhalfx, maxhalfx, maxR, minR, avephi, stereo))
  , m_referencePoint(nullptr)
{}

// construct a disc with given bounds
Trk::DiscSurface::DiscSurface(Amg::Transform3D* htrans, Trk::DiscBounds* dbounds)
  : Trk::Surface(htrans)
  , m_bounds(dbounds)
  , m_referencePoint(nullptr)
{}

// construct a disc with given bounds
Trk::DiscSurface::DiscSurface(Amg::Transform3D* htrans, Trk::DiscTrapezoidalBounds* dbounds)
  : Trk::Surface(htrans)
  , m_bounds(dbounds)
  , m_referencePoint(nullptr)
{}

// construct a disc from a transform, bounds is not set.
Trk::DiscSurface::DiscSurface(std::unique_ptr<Amg::Transform3D> htrans)
  : Trk::Surface(std::move(htrans))
  , m_bounds(nullptr)
  , m_referencePoint(nullptr)
{}

// construct form TrkDetElementBase
Trk::DiscSurface::DiscSurface(const Trk::TrkDetElementBase& detelement)
  : Trk::Surface(detelement)
  , m_bounds()
  , m_referencePoint(nullptr)
{}

Trk::DiscSurface&
Trk::DiscSurface::operator=(const DiscSurface& dsf)
{
  if (this != &dsf) {
    Trk::Surface::operator=(dsf);
    m_bounds = dsf.m_bounds;
    m_referencePoint.store(nullptr);
  }
  return *this;
}

bool
Trk::DiscSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::DiscSurface* dsf = dynamic_cast<const Trk::DiscSurface*>(&sf);
  if (!dsf)
    return false;
  if (this == dsf)
    return true;
  bool transfEqual(transform().isApprox(dsf->transform(), 10e-8));
  bool centerEqual = (transfEqual) ? (center() == dsf->center()) : false;
  bool boundsEqual = (centerEqual) ? (bounds() == dsf->bounds()) : false;
  return boundsEqual;
}

const Amg::Vector3D&
Trk::DiscSurface::globalReferencePoint() const
{
  if (!m_referencePoint) {
    const Trk::DiscBounds* dbo = dynamic_cast<const Trk::DiscBounds*>(&(bounds()));
    if (dbo) {
      double rMedium = bounds().r();
      double phi = dbo->averagePhi();
      Amg::Vector3D gp(rMedium * cos(phi), rMedium * sin(phi), 0.);
      m_referencePoint.set(std::make_unique<Amg::Vector3D>(transform() * gp));
    } else {
      const Trk::DiscTrapezoidalBounds* dtbo = dynamic_cast<const Trk::DiscTrapezoidalBounds*>(&(bounds()));
      // double rMedium = dtbo ? bounds().r() : dtbo->rCenter() ; //nonsense, or logic inverted?
      double rMedium = bounds().r();
      double phi = dtbo ? dtbo->averagePhi() : 0.;
      Amg::Vector3D gp(rMedium * cos(phi), rMedium * sin(phi), 0.);
      m_referencePoint.set(std::make_unique<Amg::Vector3D>(transform() * gp));
    }
  }
  return (*m_referencePoint);
}

void
Trk::DiscSurface::localToGlobal(const Amg::Vector2D& locpos, const Amg::Vector3D&, Amg::Vector3D& glopos) const
{
  // create the position in the local 3d frame
  Amg::Vector3D loc3Dframe(
    locpos[Trk::locR] * cos(locpos[Trk::locPhi]), locpos[Trk::locR] * sin(locpos[Trk::locPhi]), 0.);
  // transport it to the globalframe
  glopos = transform() * loc3Dframe;
}

/** local<->global transformation in case of polar local coordinates */
bool
Trk::DiscSurface::globalToLocal(const Amg::Vector3D& glopos, const Amg::Vector3D&, Amg::Vector2D& locpos) const
{
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopos;
  locpos = Amg::Vector2D(loc3Dframe.perp(), loc3Dframe.phi());
  return (fabs(loc3Dframe.z()) <= s_onSurfaceTolerance);
}

const Amg::Vector2D*
Trk::DiscSurface::localPolarToLocalCartesian(const Amg::Vector2D& locpol) const
{
  const Trk::DiscTrapezoidalBounds* dtbo = dynamic_cast<const Trk::DiscTrapezoidalBounds*>(&(bounds()));
  if (dtbo) {
    double rMedium = dtbo->rCenter();
    double phi = dtbo->averagePhi();

    Amg::Vector2D polarCenter(rMedium, phi);
    const Amg::Vector2D* cartCenter = localPolarToCartesian(polarCenter);
    const Amg::Vector2D* cartPos = localPolarToCartesian(locpol);
    Amg::Vector2D Pos = *cartPos - *cartCenter;

    delete cartCenter;
    delete cartPos;

    Amg::Vector2D locPos(Pos[Trk::locX] * sin(phi) - Pos[Trk::locY] * cos(phi),
                         Pos[Trk::locY] * sin(phi) + Pos[Trk::locX] * cos(phi));

    return (new Amg::Vector2D(locPos[Trk::locX], locPos[Trk::locY]));
  }

  return (
    new Amg::Vector2D(locpol[Trk::locR] * cos(locpol[Trk::locPhi]), locpol[Trk::locR] * sin(locpol[Trk::locPhi])));
}

/** local<->global transformation in case of polar local coordinates */
const Amg::Vector3D*
Trk::DiscSurface::localCartesianToGlobal(const Amg::Vector2D& locpos) const
{
  Amg::Vector3D loc3Dframe(locpos[Trk::locX], locpos[Trk::locY], 0.);
  return new Amg::Vector3D(transform() * loc3Dframe);
}

/** local<->global transformation in case of polar local coordinates */
const Amg::Vector2D*
Trk::DiscSurface::globalToLocalCartesian(const Amg::Vector3D& glopos, double tol) const
{
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopos;
  if (fabs(loc3Dframe.z()) > s_onSurfaceTolerance + tol)
    return nullptr;
  return new Amg::Vector2D(loc3Dframe.x(), loc3Dframe.y());
}

bool
Trk::DiscSurface::isOnSurface(const Amg::Vector3D& glopo, Trk::BoundaryCheck bchk, double tol1, double tol2) const
{
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopo;
  if (fabs(loc3Dframe.z()) > (s_onSurfaceTolerance + tol1))
    return false;
  return (bchk ? bounds().inside(Amg::Vector2D(loc3Dframe.perp(), loc3Dframe.phi()), tol1, tol2) : true);
}

/** distance to surface */
Trk::DistanceSolution
Trk::DiscSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  double tol = 0.001;

  const Amg::Vector3D& C = center();
  const Amg::Vector3D& N = normal();

  double S = C.dot(N);
  double b = S < 0. ? -1 : 1;
  double d = (pos - C).dot(N); // distance to surface

  double A = b * dir.dot(N);
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
Trk::DiscSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, bool bound) const
{
  const Amg::Transform3D& T = transform();
  double Az[3] = { T(0, 2), T(1, 2), T(2, 2) };

  // Transformation to cylinder system coordinates
  //
  double dx = pos[0] - T(0, 3);
  double dy = pos[1] - T(1, 3);
  double dz = pos[2] - T(2, 3);
  double z = dx * Az[0] + dy * Az[1] + dz * Az[2];
  double az = dir[0] * Az[0] + dir[1] * Az[1] + dir[2] * Az[2];

  // Step to surface
  //
  int ns = 0;
  double s = 0.;
  if (az != 0.) {
    s = -z / az;
    ns = 1;
  }
  double dist = fabs(z);
  if (!bound)
    return Trk::DistanceSolution(ns, dist, true, s);

  // Min distance to surface
  //
  double x = dx * T(0, 0) + dy * T(1, 0) + dz * T(2, 0);
  double y = dx * T(0, 1) + dy * T(1, 1) + dz * T(2, 1);

  Amg::Vector2D lp(sqrt(x * x + y * y), atan2(y, x));

  double d = bounds().minDistance(lp);
  if (d > 0.)
    dist = sqrt(dist * dist + d * d);

  return Trk::DistanceSolution(ns, dist, true, s);
}
