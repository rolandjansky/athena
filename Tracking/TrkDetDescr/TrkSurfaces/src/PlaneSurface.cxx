/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlaneSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkEventPrimitives/CurvilinearUVT.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkSurfaces/AnnulusBounds.h"
#include "TrkSurfaces/DiamondBounds.h"
#include "TrkSurfaces/EllipseBounds.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/TriangleBounds.h"
// Identifier
#include "Identifier/Identifier.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include "CxxUtils/sincos.h"
#include <iomanip>
#include <iostream>

const Trk::NoBounds Trk::PlaneSurface::s_boundless;

// default constructor
Trk::PlaneSurface::PlaneSurface()
  : Trk::Surface()
  , m_bounds()
{}


// copy constructor with shift
Trk::PlaneSurface::PlaneSurface(const PlaneSurface& psf, const Amg::Transform3D& transf)
  : Trk::Surface(psf, transf)
  , m_bounds(psf.m_bounds)
{}

// constructor from CurvilinearUVT
Trk::PlaneSurface::PlaneSurface(const Amg::Vector3D& position, const CurvilinearUVT& curvUVT)
  : Trk::Surface()
  , m_bounds(new Trk::NoBounds())
{
  Amg::Translation3D curvilinearTranslation(position.x(), position.y(), position.z());
  // create the rotation
  Amg::RotationMatrix3D curvilinearRotation;
  curvilinearRotation.col(0) = curvUVT.curvU();
  curvilinearRotation.col(1) = curvUVT.curvV();
  curvilinearRotation.col(2) = curvUVT.curvT();
  // curvilinear surfaces are boundless
  Trk::Surface::m_transform = std::make_unique<Amg::Transform3D>();
  (*Trk::Surface::m_transform) = curvilinearRotation;
  Trk::Surface::m_transform->pretranslate(position);
}

// construct form TrkDetElementBase
Trk::PlaneSurface::PlaneSurface(const Trk::TrkDetElementBase& detelement, Amg::Transform3D* transf)
  : Trk::Surface(detelement)
  , m_bounds()
{
  m_transform=std::unique_ptr<Amg::Transform3D>(transf);
}

// construct form SiDetectorElement
Trk::PlaneSurface::PlaneSurface(const Trk::TrkDetElementBase& detelement,
                                const Identifier& id,
                                Amg::Transform3D* transf)
  : Trk::Surface(detelement, id)
  , m_bounds()
{
  m_transform=std::unique_ptr<Amg::Transform3D>(transf);
}

// construct planar surface without bounds
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans)
  : Trk::Surface(htrans)
  , m_bounds()
{}

// construct planar surface without bounds
Trk::PlaneSurface::PlaneSurface(std::unique_ptr<Amg::Transform3D> htrans)
  : Trk::Surface(std::move(htrans))
  , m_bounds()
{}

// construct rectangle module
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, double halephi, double haleta)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::RectangleBounds(halephi, haleta))
{}

// construct trapezoidal module with parameters
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, double minhalephi, double maxhalephi, double haleta)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::TrapezoidBounds(minhalephi, maxhalephi, haleta))
{}

// construct annulus module with parameters
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::AnnulusBounds* tbounds)
  : Trk::Surface(htrans)
  , m_bounds(tbounds)
{}

// construct rectangle surface by giving RectangleBounds
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::RectangleBounds* rbounds)
  : Trk::Surface(htrans)
  , m_bounds(rbounds)
{}

// construct triangle surface by giving TriangleBounds
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::TriangleBounds* rbounds)
  : Trk::Surface(htrans)
  , m_bounds(rbounds)
{}

// construct trapezoidal module with parameters
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::TrapezoidBounds* tbounds)
  : Trk::Surface(htrans)
  , m_bounds(tbounds)
{}

// construct rotated trapezoidal module with parameters
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::RotatedTrapezoidBounds* tbounds)
  : Trk::Surface(htrans)
  , m_bounds(tbounds)
{}

// construct diamond module with parameters
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::DiamondBounds* tbounds)
  : Trk::Surface(htrans)
  , m_bounds(tbounds)
{}

// construct elliptic module with parameters
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::EllipseBounds* tbounds)
  : Trk::Surface(htrans)
  , m_bounds(tbounds)
{}

// construct module with shared boundaries - change to reference
Trk::PlaneSurface::PlaneSurface(Amg::Transform3D* htrans, Trk::SharedObject<const Trk::SurfaceBounds>& tbounds)
  : Trk::Surface(htrans)
  , m_bounds(tbounds)
{}

bool
Trk::PlaneSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::PlaneSurface* psf = dynamic_cast<const Trk::PlaneSurface*>(&sf);
  if (!psf)
    return false;
  if (psf == this)
    return true;
  bool transfEqual(transform().isApprox(psf->transform(), 10e-8));
  bool centerEqual = center() == psf->center();
  bool boundsEqual = bounds() == psf->bounds();
  return transfEqual && centerEqual && boundsEqual;
}

void
Trk::PlaneSurface::localToGlobal(const Amg::Vector2D& locpos, const Amg::Vector3D&, Amg::Vector3D& glopos) const
{
  Amg::Vector3D loc3Dframe(locpos[Trk::locX], locpos[Trk::locY], 0.);
  glopos = transform() * loc3Dframe;
}

bool
Trk::PlaneSurface::globalToLocal(const Amg::Vector3D& glopos, const Amg::Vector3D&, Amg::Vector2D& locpos) const
{
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopos;
  locpos = Amg::Vector2D(loc3Dframe.x(), loc3Dframe.y());
  return (loc3Dframe.z() * loc3Dframe.z() <= s_onSurfaceTolerance * s_onSurfaceTolerance);
}

void
Trk::PlaneSurface::localToGlobalDirection(const Trk::LocalDirection& locdir, Amg::Vector3D& globdir) const
{

  CxxUtils::sincos scXZ(locdir.angleXZ());
  CxxUtils::sincos scYZ(locdir.angleYZ());

  double norm = 1. / sqrt(scYZ.cs * scYZ.cs * scXZ.sn * scXZ.sn + scYZ.sn * scYZ.sn);

  // decide on the sign
  double sign = (scXZ.sn < 0.) ? -1. : 1.;

  // now calculate the GlobalDirection in the global frame
  globdir =
    transform().linear() *
    Amg::Vector3D(sign * scXZ.cs * scYZ.sn * norm, sign * scXZ.sn * scYZ.cs * norm, sign * scXZ.sn * scYZ.sn * norm);
}

void
Trk::PlaneSurface::globalToLocalDirection(const Amg::Vector3D& glodir, Trk::LocalDirection& ldir) const
{
  // bring the global direction into the surface frame
  Amg::Vector3D d(transform().inverse().linear() * glodir);
  ldir = Trk::LocalDirection(atan2(d.z(), d.x()), atan2(d.z(), d.y()));
}

bool
Trk::PlaneSurface::isOnSurface(const Amg::Vector3D& glopo, Trk::BoundaryCheck bchk, double tol1, double tol2) const
{
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopo;
  if (fabs(loc3Dframe(2)) > (s_onSurfaceTolerance + tol1))
    return false;
  return (bchk ? bounds().inside(Amg::Vector2D(loc3Dframe(0), loc3Dframe(1)), tol1, tol2) : true);
}

/** distance to surface */
Trk::DistanceSolution
Trk::PlaneSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  static const double tol = 0.001;

  const Amg::Vector3D& N = normal();

  const double d = (pos - center()).dot(N);

  const double A = dir.dot(N); // ignore sign
  if (A == 0.) {               // direction parallel to surface
    if (fabs(d) < tol) {
      return Trk::DistanceSolution(1, 0., true, 0.);
    }
      return Trk::DistanceSolution(0, d, true, 0.);

  }

  return Trk::DistanceSolution(1, d, true, -d / A);
}

Trk::DistanceSolution
Trk::PlaneSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, bool bound) const
{
  const Amg::Transform3D& T = transform();
  double Az[3] = { T(0, 2), T(1, 2), T(2, 2) };

  // Transformation to plane system coordinates
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
    return Trk::DistanceSolution(ns, fabs(z), true, s);

  // Min distance to surface
  //
  double x = dx * T(0, 0) + dy * T(1, 0) + dz * T(2, 0);
  double y = dx * T(0, 1) + dy * T(1, 1) + dz * T(2, 1);

  Amg::Vector2D lp(x, y);

  double d = bounds().minDistance(lp);
  if (d > 0.)
    dist = sqrt(dist * dist + d * d);

  return Trk::DistanceSolution(ns, dist, true, s);
}
