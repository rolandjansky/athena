/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StraightLineSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/CylinderBounds.h"
// Identifier
#include "Identifier/Identifier.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
#include <iomanip>
#include <iostream>

const Trk::NoBounds Trk::StraightLineSurface::s_boundless;

// default constructor
Trk::StraightLineSurface::StraightLineSurface()
  : Surface()
  , m_lineDirection(nullptr)
  , m_bounds()
{}

// constructors by arguments: boundless surface
Trk::StraightLineSurface::StraightLineSurface(Amg::Transform3D* htrans)
  : Surface(htrans)
  , m_lineDirection(nullptr)
  , m_bounds()
{}

// constructors by arguments: boundless surface
Trk::StraightLineSurface::StraightLineSurface(std::unique_ptr<Amg::Transform3D> htrans)
  : Surface(std::move(htrans))
  , m_lineDirection(nullptr)
  , m_bounds()
{}

// constructors by arguments
Trk::StraightLineSurface::StraightLineSurface(Amg::Transform3D* htrans, double radius, double halez)
  : Surface(htrans)
  , m_lineDirection(nullptr)
  , m_bounds(new Trk::CylinderBounds(radius, halez))
{}

// dummy implementation
Trk::StraightLineSurface::StraightLineSurface(const Trk::TrkDetElementBase& detelement, const Identifier& id)
  : Surface(detelement, id)
  , m_lineDirection(nullptr)
  , m_bounds()
{}

// copy constructor
Trk::StraightLineSurface::StraightLineSurface(const Trk::StraightLineSurface& slsf)
  : Surface(slsf)
  , m_lineDirection(nullptr)
  , m_bounds(slsf.m_bounds)
{}

// copy constructor with shift
Trk::StraightLineSurface::StraightLineSurface(const StraightLineSurface& csf, const Amg::Transform3D& transf)
  : Surface(csf, transf)
  , m_lineDirection(nullptr)
  , m_bounds(csf.m_bounds)
{}


// assignment operator
Trk::StraightLineSurface&
Trk::StraightLineSurface::operator=(const Trk::StraightLineSurface& slsf)
{
  if (this != &slsf) {
    m_lineDirection.store(nullptr);
    Trk::Surface::operator=(slsf);
    m_bounds = slsf.m_bounds;
  }
  return *this;
}

bool
Trk::StraightLineSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::StraightLineSurface* slsf = dynamic_cast<const Trk::StraightLineSurface*>(&sf);
  if (!slsf)
    return false;
  bool transfEqual(transform().isApprox(slsf->transform(), 10e-8));
  bool centerEqual = (transfEqual) ? (center() == slsf->center()) : false;
  bool boundsEqual = (centerEqual) ? (bounds() == slsf->bounds()) : false;
  return boundsEqual;
}

// true local to global method - fully defined
void
Trk::StraightLineSurface::localToGlobal(const Amg::Vector2D& locpos,
                                        const Amg::Vector3D& glomom,
                                        Amg::Vector3D& glopos) const
{
  // get the vector perpenticular to the momentum and the straw axis
  Amg::Vector3D radiusAxisGlobal(lineDirection().cross(glomom));
  Amg::Vector3D locZinGlobal = transform() * Amg::Vector3D(0., 0., locpos[Trk::locZ]);
  // transform zPosition into global coordinates and add locR * radiusAxis
  glopos = Amg::Vector3D(locZinGlobal + locpos[Trk::locR] * radiusAxisGlobal.normalized());
}

// specialized version for providing different Z -  local to global method - from LocalParameters/
Amg::Vector3D*
Trk::StraightLineSurface::localToGlobal(const Trk::LocalParameters& locpars,
                                        const Amg::Vector3D& glomom,
                                        double locZ) const
{
  // create a local Position
  Amg::Vector2D locPos(locpars[Trk::driftRadius], locZ);
  return Surface::localToGlobal(locPos, glomom);
}

Amg::Vector3D
Trk::StraightLineSurface::localToGlobalPos(const Trk::LocalParameters& locpars,
                                           const Amg::Vector3D& glomom,
                                           double locZ) const
{
  // create a local Position
  Amg::Vector2D locPos(locpars[Trk::driftRadius], locZ);
  return Surface::localToGlobalPos(locPos, glomom);
}


// true global to local method - fully defined
bool
Trk::StraightLineSurface::globalToLocal(const Amg::Vector3D& glopos,
                                        const Amg::Vector3D& glomom,
                                        Amg::Vector2D& locpos) const
{
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopos;
  // construct localPosition with sign*candidate.perp() and z.()
  locpos = Amg::Vector2D(loc3Dframe.perp(), loc3Dframe.z());
  Amg::Vector3D decVec(glopos - center());
  // assign the right sign
  double sign = ((lineDirection().cross(glomom)).dot(decVec) < 0.) ? -1. : 1.;
  locpos[Trk::locR] *= sign;
  return true;
}

// isOnSurface check
bool
Trk::StraightLineSurface::isOnSurface(const Amg::Vector3D& glopo, BoundaryCheck bchk, double tol1, double tol2) const
{
  if (!bchk)
    return true;
  // check whether this is a boundless surface
  if (!(m_bounds.get()) && !Surface::m_associatedDetElement)
    return true;
  // get the standard bounds
  Amg::Vector3D loc3Dframe = (transform().inverse()) * glopo;
  Amg::Vector2D locCand(loc3Dframe.perp(), loc3Dframe.z());
  return (locCand[Trk::locR] < bounds().r() + tol1 && bounds().insideLoc2(locCand, tol2));
}

/** distance to surface */
Trk::DistanceSolution
Trk::StraightLineSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  const Amg::Vector3D& C = center();
  const Amg::Vector3D& S = lineDirection();

  double D = dir.dot(S);
  double A = (1. - D) * (1. + D);

  Amg::Vector3D dx = C - pos - (C.dot(S) - pos.dot(S)) * S;
  double currDist = sqrt(dx.dot(dx));

  if (A < 0.0001) {
    return Trk::DistanceSolution(1, currDist, false, 0.);
  }
  double sol = (pos - C).dot(D * S - dir) / A;
  return Trk::DistanceSolution(1, currDist, false, sol);
}

// return the measurement frame
Amg::RotationMatrix3D
Trk::StraightLineSurface::measurementFrame(const Amg::Vector3D&, const Amg::Vector3D& glomom) const
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

Trk::DistanceSolution
Trk::StraightLineSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos,
                                                       const Amg::Vector3D& dir,
                                                       bool bound) const
{
  const Amg::Transform3D& T = transform();
  Amg::Vector3D Az(T(0, 2), T(1, 2), T(2, 2));

  Amg::Vector3D dxyz = pos - T.translation();

  double D = dir.dot(Az);
  double Lz = dxyz.dot(Az);
  double A = (1. - D) * (1. + D);

  // Step to surface
  //
  double s = 0.;
  if (A > 0.)
    s = (D * Lz - (dir.dot(dxyz))) / A;
  if (!bound)
    return Trk::DistanceSolution(1, 0., false, s);

  // Min distance to surface
  //
  double Rm = 20.;
  double Lzm = 1.e+10;

  if (m_bounds.get()) {
    Rm = m_bounds.get()->r();
    Lzm = m_bounds.get()->halflengthZ();
  } else if (Surface::m_associatedDetElement) {

    const Trk::CylinderBounds* cb = nullptr;

    if (Surface::m_associatedDetElementId.is_valid()) {
      cb = dynamic_cast<const Trk::CylinderBounds*>(&m_associatedDetElement->bounds(Surface::m_associatedDetElementId));
    } else {
      cb = dynamic_cast<const Trk::CylinderBounds*>(&m_associatedDetElement->bounds());
    }
    if (cb) {
      Rm = cb->r();
      Lzm = cb->halflengthZ();
    }
  }

  double dist = dxyz.dot(dxyz) - Lz * Lz;
  dist = (dist > Rm * Rm) ? sqrt(dist) - Rm : 0.;
  double dL = fabs(Lz) - Lzm;
  if (dL > 0.)
    dist = sqrt(dist * dist + dL * dL);

  return Trk::DistanceSolution(1, dist, false, s);
}

Trk::Intersection
Trk::StraightLineSurface::straightLineIntersection(const Amg::Vector3D& pos,
                                                   const Amg::Vector3D& dir,
                                                   bool forceDir,
                                                   Trk::BoundaryCheck bchk) const
{
  // following nominclature found in header file and doxygen documentation
  // line one is the straight track
  const Amg::Vector3D& ma = pos;
  const Amg::Vector3D& ea = dir;
  // line two is the line surface
  const Amg::Vector3D& mb = center();
  const Amg::Vector3D& eb = lineDirection();
  // now go ahead and solve for the closest approach
  Amg::Vector3D mab(mb - ma);
  double eaTeb = ea.dot(eb);
  double denom = 1 - eaTeb * eaTeb;
  if (fabs(denom) > 10e-7) {
    double lambda0 = (mab.dot(ea) - mab.dot(eb) * eaTeb) / denom;
    // evaluate in terms of direction
    bool isValid = forceDir ? (lambda0 > 0.) : true;
    // evaluate validaty in terms of bounds
    Amg::Vector3D result = (ma + lambda0 * ea);
    isValid = bchk ? (isValid && isOnSurface(result)) : isValid;
    // return the result
    return Trk::Intersection(result, lambda0, isValid);
  }
  return Trk::Intersection(pos, 0., false);
}
