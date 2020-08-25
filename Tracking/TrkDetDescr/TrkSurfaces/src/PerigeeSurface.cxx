/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// PerigeeSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/PerigeeSurface.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>
// Eigen
#include "GeoPrimitives/GeoPrimitives.h"

const Trk::NoBounds Trk::PerigeeSurface::s_perigeeBounds;

Trk::PerigeeSurface::PerigeeSurface()
  : Surface()
  , m_lineDirection(nullptr)
{}

Trk::PerigeeSurface::PerigeeSurface(const Amg::Vector3D& gp)
  : Surface()
  , m_lineDirection(nullptr)
{
  Surface::m_center = std::make_unique<const Amg::Vector3D>(gp);
  Surface::m_transform = std::make_unique<Amg::Transform3D>();
  (*Surface::m_transform) = Amg::Translation3D(gp.x(), gp.y(), gp.z());
}

Trk::PerigeeSurface::PerigeeSurface(Amg::Transform3D* tTransform)
  : Surface()
  , m_lineDirection(nullptr)
{
  Surface::m_transform=std::unique_ptr<Amg::Transform3D>(tTransform);
}

Trk::PerigeeSurface::PerigeeSurface(std::unique_ptr<Amg::Transform3D> tTransform)
  : Surface(std::move(tTransform))
  , m_lineDirection(nullptr)
{}

Trk::PerigeeSurface::PerigeeSurface(const PerigeeSurface& pesf)
  : Surface(pesf)
  , m_lineDirection(nullptr)
{
  if (pesf.m_center)
    Surface::m_center = std::make_unique<const Amg::Vector3D>(*pesf.m_center);
  if (pesf.m_transform)
    Surface::m_transform = std::make_unique<Amg::Transform3D>(*pesf.m_transform);
}

Trk::PerigeeSurface::PerigeeSurface(const PerigeeSurface& pesf, const Amg::Transform3D& shift)
  : Surface()
  , m_lineDirection(nullptr)
{
  if (pesf.m_center)
    Surface::m_center = std::make_unique<const Amg::Vector3D>(shift * (*pesf.m_center));
  if (pesf.m_transform)
    Surface::m_transform = std::make_unique<Amg::Transform3D>(shift * (*pesf.m_transform));
}


// assignment operator
Trk::PerigeeSurface&
Trk::PerigeeSurface::operator=(const Trk::PerigeeSurface& pesf)
{
  if (this != &pesf) {
    Trk::Surface::operator=(pesf);
    m_lineDirection.store(std::make_unique<Amg::Vector3D>(lineDirection()));
  }
  return *this;
}

bool
Trk::PerigeeSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::PerigeeSurface* persf = dynamic_cast<const Trk::PerigeeSurface*>(&sf);
  if (!persf)
    return false;
  if (this == persf)
    return true;
  return (center() == persf->center());
}

// simple local to global - from LocalParameters /
const Amg::Vector3D*
Trk::PerigeeSurface::localToGlobal(const Trk::LocalParameters& locpars) const
{
  if (locpars.contains(Trk::phi0)) {
    Amg::Vector3D loc3Dframe(
      -locpars[Trk::d0] * sin(locpars[Trk::phi0]), locpars[Trk::d0] * cos(locpars[Trk::phi0]), locpars[Trk::z0]);
    return new Amg::Vector3D(transform() * loc3Dframe);
  }
    return new Amg::Vector3D(0., 0., locpars[Trk::z0] + (center().z()));
}

// true local to global method/
void
Trk::PerigeeSurface::localToGlobal(const Amg::Vector2D& locpos,
                                   const Amg::Vector3D& glomom,
                                   Amg::Vector3D& glopos) const
{
  // this is for a tilted perigee surface
  if (Surface::m_transform) {
    // get the vector perpendicular to the momentum and the straw axis
    Amg::Vector3D radiusAxisGlobal(lineDirection().cross(glomom));
    Amg::Vector3D locZinGlobal = transform() * Amg::Vector3D(0., 0., locpos[Trk::locZ]);
    // transform zPosition into global coordinates and add locR * radiusAxis
    glopos = Amg::Vector3D(locZinGlobal + locpos[Trk::locR] * radiusAxisGlobal.normalized());
  } else {
    double phi = glomom.phi();
    glopos[Amg::x] = -locpos[Trk::d0] * sin(phi);
    glopos[Amg::y] = locpos[Trk::d0] * cos(phi);
    glopos[Amg::z] = locpos[Trk::z0];
    glopos += center();
  }
}

// true local to global method - from LocalParameters /
const Amg::Vector3D*
Trk::PerigeeSurface::localToGlobal(const Trk::LocalParameters& locpars, const Amg::Vector3D& glomom) const
{
  if (Surface::m_transform) {
    Amg::Vector3D* glopos = new Amg::Vector3D(0., 0., 0.);
    localToGlobal(Amg::Vector2D(locpars[Trk::d0], locpars[Trk::z0]), glomom, *glopos);
    return glopos;
  }
  double phi = glomom.phi();
  double x = -locpars[Trk::d0] * sin(phi) + center().x();
  double y = locpars[Trk::d0] * cos(phi) + center().y();
  double z = locpars[Trk::z0] + center().z();
  return new Amg::Vector3D(x, y, z);
}

// true global to local method
bool
Trk::PerigeeSurface::globalToLocal(const Amg::Vector3D& glopos,
                                   const Amg::Vector3D& glomom,
                                   Amg::Vector2D& locpos) const
{
  Amg::Vector3D perPos = (transform().inverse()) * glopos;
  double d0 = perPos.perp();
  double z0 = perPos.z();
  // decide the sign of d0
  d0 *= ((lineDirection().cross(glomom)).dot(perPos) < 0.0) ? -1.0 : 1.0;
  locpos[Trk::d0] = d0;
  locpos[Trk::z0] = z0;
  return true;
}

// return the measurement frame - this is the frame where the covariance is defined
Amg::RotationMatrix3D
Trk::PerigeeSurface::measurementFrame(const Amg::Vector3D&, const Amg::Vector3D& glomom) const
{
  Amg::RotationMatrix3D mFrame;
  // construct the measurement frame
  const Amg::Vector3D& measY = lineDirection();
  Amg::Vector3D measX(measY.cross(glomom).unit());
  Amg::Vector3D measDepth(measX.cross(measY));
  // assign the columnes
  mFrame.col(0) = measX;
  mFrame.col(1) = measY;
  mFrame.col(2) = measDepth;
  // return the rotation matrix
  return mFrame;
}

/** distance to surface */
Trk::DistanceSolution
Trk::PerigeeSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  const Amg::Vector3D& C = center();
  Amg::Vector3D S(0., 0., 1.);

  double D = dir.dot(S);
  double A = (1. - D) * (1. + D);
  if (A == 0.) {
    return Trk::DistanceSolution(1, pos.perp(), false, 0.);
  }
  double sol = (pos - C).dot(D * S - dir) / A;
  return Trk::DistanceSolution(1, pos.perp(), false, sol);
}

Trk::DistanceSolution
Trk::PerigeeSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, bool) const
{
  const Amg::Transform3D& T = transform();

  double dx = pos[0] - T(3, 0);
  double dy = pos[1] - T(3, 1);
  double A = dir[0] * dir[0] + dir[1] * dir[1];

  // Step to surface
  //
  if (A > 0.) {
    return Trk::DistanceSolution(1, 0., false, -(dir[0] * dx + dir[1] * dy) / A);
  }
  return Trk::DistanceSolution(1, 0., false, 0.);
}

// overload of ostream operator
MsgStream&
Trk::PerigeeSurface::dump(MsgStream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::PerigeeSurface:" << std::endl;
  sl << "     Center position  (x, y, z) = (" << center().x() << ", " << center().y() << ", " << center().z() << ")";
  sl << std::setprecision(-1);
  return sl;
}

// overload of ostream operator
std::ostream&
Trk::PerigeeSurface::dump(std::ostream& sl) const
{
  sl << std::setiosflags(std::ios::fixed);
  sl << std::setprecision(7);
  sl << "Trk::PerigeeSurface:" << std::endl;
  sl << "     Center position  (x, y, z) = (" << center().x() << ", " << center().y() << ", " << center().z() << ")";
  sl << std::setprecision(-1);
  return sl;
}
