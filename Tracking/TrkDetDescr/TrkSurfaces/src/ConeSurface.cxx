/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ConeSurface.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkSurfaces/ConeSurface.h"
#include "TrkSurfaces/RealQuadraticEquation.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// STD
//#include <iostream>
//#include <iomanip>
#include <cassert>

// default constructor
Trk::ConeSurface::ConeSurface()
  : Trk::Surface()
  , m_bounds()
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{}

// copy constructor
Trk::ConeSurface::ConeSurface(const ConeSurface& csf)
  : Trk::Surface(csf)
  , m_bounds(csf.m_bounds)
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{}

// copy constructor with shift
Trk::ConeSurface::ConeSurface(const ConeSurface& csf, const Amg::Transform3D& transf)
  : Trk::Surface(csf, transf)
  , m_bounds(csf.m_bounds)
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{}

// constructor by opening angle and whether its symmetric or a single cone
Trk::ConeSurface::ConeSurface(Amg::Transform3D* htrans, double alpha, bool symmetric)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::ConeBounds(alpha, symmetric))
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{}

// constructor by opening angle and its z values
Trk::ConeSurface::ConeSurface(Amg::Transform3D* htrans, double alpha, double zmin, double zmax, double halfPhi)
  : Trk::Surface(htrans)
  , m_bounds(new Trk::ConeBounds(alpha, zmin, zmax, halfPhi))
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{}

// constructor by ConeBounds
Trk::ConeSurface::ConeSurface(Amg::Transform3D* htrans, Trk::ConeBounds* cbounds)
  : Trk::Surface(htrans)
  , m_bounds(cbounds)
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{
  assert(cbounds);
}

// constructor from transform, bounds not set.
Trk::ConeSurface::ConeSurface(std::unique_ptr<Amg::Transform3D> htrans)
  : Trk::Surface(std::move(htrans))
  , m_bounds(nullptr)
  , m_referencePoint(nullptr)
  , m_rotSymmetryAxis(nullptr)
{}

// destructor (will call destructor from base class which deletes objects)
Trk::ConeSurface::~ConeSurface() = default;

Trk::ConeSurface&
Trk::ConeSurface::operator=(const ConeSurface& csf)
{
  if (this != &csf) {
    Trk::Surface::operator=(csf);
    m_bounds = csf.m_bounds;
    m_referencePoint.store(nullptr);
    m_rotSymmetryAxis.store(nullptr);
  }
  return *this;
}

// TODO: is the 0 always the cone center?
const Amg::Vector3D&
Trk::ConeSurface::globalReferencePoint() const
{
  if (!m_referencePoint) {
    // this is what was in cylinder
    // double rMedium = bounds().r();
    // double phi     = bounds().averagePhi();
    // Trk::GlobalPosition gp(rMedium*cos(phi), rMedium*sin(phi), 0.);
    Amg::Vector3D gp(0., 0., 0.);
    m_referencePoint.set(std::make_unique<Amg::Vector3D>(transform() * gp));
  }
  return (*m_referencePoint);
}

bool
Trk::ConeSurface::operator==(const Trk::Surface& sf) const
{
  // first check the type not to compare apples with oranges
  const Trk::ConeSurface* csf = dynamic_cast<const Trk::ConeSurface*>(&sf);
  if (!csf)
    return false;
  if (this == csf)
    return true;
  bool transfEqual(transform().isApprox(csf->transform(), 10e-8));
  bool centerEqual = (transfEqual) ? (center() == csf->center()) : false;
  bool boundsEqual = (centerEqual) ? (bounds() == csf->bounds()) : false;
  return boundsEqual;
}

const Amg::Vector3D&
Trk::ConeSurface::rotSymmetryAxis() const
{
  if (!m_rotSymmetryAxis) {
    Amg::Vector3D zAxis(transform().rotation().col(2));
    m_rotSymmetryAxis.set(std::make_unique<Amg::Vector3D>(zAxis));
  }
  return (*m_rotSymmetryAxis);
}

// return the measurement frame: it's the tangential plane
Amg::RotationMatrix3D
Trk::ConeSurface::measurementFrame(const Amg::Vector3D& pos, const Amg::Vector3D&) const
{
  Amg::RotationMatrix3D mFrame;
  // construct the measurement frame
  Amg::Vector3D measY(transform().rotation().col(2)); // measured Y is the z axis
  Amg::Vector3D measDepth =
    Amg::Vector3D(pos.x(), pos.y(), 0.).unit();       // measured z is the position transverse normalized
  Amg::Vector3D measX(measY.cross(measDepth).unit()); // measured X is what comoes out of it
  // the columnes
  mFrame.col(0) = measX;
  mFrame.col(1) = measY;
  mFrame.col(2) = measDepth;
  // return the rotation matrix
  //!< @todo fold in alpha
  // return it
  return mFrame;
}

void
Trk::ConeSurface::localToGlobal(const Amg::Vector2D& locpos, const Amg::Vector3D&, Amg::Vector3D& glopos) const
{
  // create the position in the local 3d frame
  double r = locpos[Trk::locZ] * bounds().tanAlpha();
  double phi = locpos[Trk::locRPhi] / r;
  Amg::Vector3D loc3Dframe(r * cos(phi), r * sin(phi), locpos[Trk::locZ]);
  // transport it to the globalframe
  glopos = transform() * loc3Dframe;
}

bool
Trk::ConeSurface::globalToLocal(const Amg::Vector3D& glopos, const Amg::Vector3D&, Amg::Vector2D& locpos) const
{
  const Amg::Transform3D& surfaceTrans = transform();
  Amg::Transform3D inverseTrans(surfaceTrans.inverse());
  Amg::Vector3D loc3Dframe(inverseTrans * glopos);
  double r = loc3Dframe.z() * bounds().tanAlpha();
  locpos = Amg::Vector2D(r * atan2(loc3Dframe.y(), loc3Dframe.x()), loc3Dframe.z());
  // now decide on the quility of the transformation
  // double inttol = r*0.0001;
  // inttol = (inttol<0.01) ? 0.01 : 0.01; // ?
  double inttol = 0.01;
  return ((loc3Dframe.perp() - r) <= inttol);
}

Trk::Intersection
Trk::ConeSurface::straightLineIntersection(const Amg::Vector3D& pos,
                                           const Amg::Vector3D& dir,
                                           bool forceDir,
                                           Trk::BoundaryCheck bchk) const
{
  // transform to a frame with the cone along z, with the tip at 0
  Amg::Vector3D tpos1 = transform().inverse() * pos;
  Amg::Vector3D tdir = transform().inverse().linear() * dir;
  // see the header for the formula derivation
  double tan2Alpha = bounds().tanAlpha() * bounds().tanAlpha();
  double A = tdir.x() * tdir.x() + tdir.y() * tdir.y() - tan2Alpha * tdir.z() * tdir.z();
  double B = 2 * (tdir.x() * tpos1.x() + tdir.y() * tpos1.y() - tan2Alpha * dir.z() * tpos1.z());
  double C = tpos1.x() * tpos1.x() + tpos1.y() * tpos1.y() - tan2Alpha * tpos1.z() * tpos1.z();
  if (A == 0.)
    A += 1e-16; // avoid div by zero

  // use Andreas' quad solver, much more stable than what I wrote
  Trk::RealQuadraticEquation solns(A, B, C);

  Amg::Vector3D solution(0., 0., 0.);
  double path = 0.;
  bool isValid = false;
  if (solns.solutions != Trk::none) {
    double t1 = solns.first;
    Amg::Vector3D soln1Loc(tpos1 + t1 * dir);
    isValid = forceDir ? (t1 > 0.) : true;
    // there's only one solution
    if (solns.solutions == Trk::one) {
      solution = soln1Loc;
      path = t1;
    } else {
      double t2 = solns.second;
      Amg::Vector3D soln2Loc(tpos1 + t2 * dir);
      // both solutions have the same sign
      if (t1 * t2 > 0. || !forceDir) {
        if (t1 * t1 < t2 * t2) {
          solution = soln1Loc;
          path = t1;
        } else {
          solution = soln2Loc;
          path = t2;
        }
      } else {
        if (t1 > 0.) {
          solution = soln1Loc;
          path = t1;
        } else {
          solution = soln2Loc;
          path = t2;
        }
      }
    }
  }
  solution = transform() * solution;

  isValid = bchk ? (isValid && isOnSurface(solution)) : isValid;
  return Trk::Intersection(solution, path, isValid);
}

/** distance to surface */

// TODO understand what is going on here (copied from cylinder,
// because i really don't see what the idea is here vs. the other
// straightLineDistanceEstimate, and this is harder to see whats
// happening.
Trk::DistanceSolution
Trk::ConeSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir) const
{
  return straightLineDistanceEstimate(pos, dir, false);
}

Trk::DistanceSolution
Trk::ConeSurface::straightLineDistanceEstimate(const Amg::Vector3D& pos, const Amg::Vector3D& dir, bool bound) const
{
  double tol = 0.001;

  Amg::Vector3D Cntr = center(); // tip of the cone (i.e. join between halves)
  Amg::Vector3D N = normal();    // this is the z-direction of the cone in
                                 // global coordiantes i believe

  Amg::Vector3D dPos = pos - Cntr; // pos w.r.t. cone tip
  double posLength = sqrt(dPos.dot(dPos));
  if (posLength < tol) // at origin of cone => on cone (avoid div by zero)
    return Trk::DistanceSolution(1, 0., true, 0.);
  double posProj = dPos.dot(N);
  double posProjAngle = acos(posProj / posLength);
  double currDist = posLength * sin(posProjAngle - atan(bounds().tanAlpha()));
  // solution on the surface
  if (fabs(currDist) < tol)
    return Trk::DistanceSolution(1, currDist, true, 0.);

  // transform to a frame with the cone along z, with the tip a 0
  Amg::Vector3D locFramePos = transform().inverse() * pos;
  Amg::Vector3D locFrameDir = transform().rotation().inverse() * dir.normalized();

  // solutions are in the form of a solution to a quadratic eqn.
  double tan2Alpha = bounds().tanAlpha() * bounds().tanAlpha();
  double A = locFrameDir.x() * locFrameDir.x() + locFrameDir.y() * locFrameDir.y() -
             tan2Alpha * locFrameDir.z() * locFrameDir.z();
  double B = 2 * (locFrameDir.x() * locFramePos.x() + locFrameDir.y() * locFramePos.y() -
                  tan2Alpha * locFrameDir.z() * locFramePos.z());
  double C = locFramePos.x() * locFramePos.x() + locFramePos.y() * locFramePos.y() -
             tan2Alpha * locFramePos.z() * locFramePos.z();
  if (A == 0.)
    A += 1e-16; // avoid div by zero
  // use Andreas' quad solver, much more stable than what I wrote
  Trk::RealQuadraticEquation solns(A, B, C);

  double d2bound = 0.;
  if (bound && solns.solutions != Trk::none) {
    const Amg::Vector2D* p = nullptr;
    if (fabs(solns.first) < fabs(solns.second))
      p = Surface::globalToLocal(locFramePos + solns.first * locFrameDir);
    else
      p = Surface::globalToLocal(locFramePos + solns.second * locFrameDir);
    if (p) {
      d2bound = bounds().minDistance(*p);
      delete p;
    }
    if (d2bound < 0)
      d2bound = 0;
  }
  double totDist = d2bound > 0. ? sqrt(d2bound * d2bound + currDist * currDist) : currDist;

  switch (solns.solutions) {
  case Trk::none:
    return Trk::DistanceSolution(0, totDist, true, 0., 0.);
  case Trk::one:
    return Trk::DistanceSolution(1, totDist, true, solns.first);
  case Trk::two:
    if (fabs(solns.first) < fabs(solns.second))
      return Trk::DistanceSolution(2, totDist, true, solns.first, solns.second);
    return Trk::DistanceSolution(2, totDist, true, solns.second, solns.first);
  default:
    return Trk::DistanceSolution(0, totDist, true, 0., 0.);
  };
}

double
Trk::ConeSurface::pathCorrection(const Amg::Vector3D& pos, const Amg::Vector3D& mom) const
{
  // (cos phi cos alpha, sin phi cos alpha, sgn z sin alpha)
  bool applyTransform = !(transform().isApprox(Amg::Transform3D::Identity()));
  Amg::Vector3D posLocal = applyTransform ? transform().inverse() * pos : pos;
  double phi = posLocal.phi();
  double sgn = posLocal.z() > 0. ? -1. : +1.;
  Amg::Vector3D normalC(cos(phi) * bounds().cosAlpha(), sin(phi) * bounds().cosAlpha(), sgn * bounds().sinAlpha());
  if (applyTransform)
    normalC = transform() * normalC;
  // back in global frame
  double cAlpha = normalC.dot(mom.unit());
  return (cAlpha != 0.) ? fabs(1. / cAlpha) : 1.; // ST undefined for cAlpha=0
}
