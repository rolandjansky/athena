/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrapezoidVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/TrapezoidVolumeBounds.h"

#include "TrkDetDescrUtils/GeometryStatics.h"

// TrkSurfaces
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

Trk::TrapezoidVolumeBounds::TrapezoidVolumeBounds()
  : VolumeBounds()
  , m_minHalfX(0.)
  , m_maxHalfX(0.)
  , m_halfY(0.)
  , m_halfZ(0.)
  , m_alpha(0.)
  , m_beta(0.)
  , m_objectAccessor()
{}

Trk::TrapezoidVolumeBounds::TrapezoidVolumeBounds(
  double minhalex,
  double maxhalex,
  double haley,
  double halez)
  : VolumeBounds()
  , m_minHalfX(minhalex)
  , m_maxHalfX(maxhalex)
  , m_halfY(haley)
  , m_halfZ(halez)
  , m_alpha(0.)
  , m_beta(0.)
  , m_objectAccessor()
{
  m_alpha = atan((m_maxHalfX - m_minHalfX) / 2 / m_halfY) + 0.5 * M_PI;
  m_beta = m_alpha;
}

Trk::TrapezoidVolumeBounds::TrapezoidVolumeBounds(
  double minhalex,
  double haley,
  double halez,
  double alpha,
  double beta)
  : VolumeBounds()
  , m_minHalfX(minhalex)
  , m_maxHalfX(0.)
  , m_halfY(haley)
  , m_halfZ(halez)
  , m_alpha(alpha)
  , m_beta(beta)
  , m_objectAccessor()
{
  double gamma = (alpha > beta) ? (alpha - 0.5 * M_PI) : (beta - 0.5 * M_PI);
  m_maxHalfX = m_minHalfX + (2. * m_halfY) * tan(gamma);
}

Trk::TrapezoidVolumeBounds::TrapezoidVolumeBounds(
  const Trk::TrapezoidVolumeBounds& trabo)
  : VolumeBounds()
  , m_minHalfX(trabo.m_minHalfX)
  , m_maxHalfX(trabo.m_maxHalfX)
  , m_halfY(trabo.m_halfY)
  , m_halfZ(trabo.m_halfZ)
  , m_alpha(trabo.m_alpha)
  , m_beta(trabo.m_beta)
  , m_objectAccessor(trabo.m_objectAccessor)
{}

Trk::TrapezoidVolumeBounds::~TrapezoidVolumeBounds() {}

Trk::TrapezoidVolumeBounds&
Trk::TrapezoidVolumeBounds::operator=(const Trk::TrapezoidVolumeBounds& trabo)
{
  if (this != &trabo) {
    m_minHalfX = trabo.m_minHalfX;
    m_maxHalfX = trabo.m_maxHalfX;
    m_halfY = trabo.m_halfY;
    m_halfZ = trabo.m_halfZ;
    m_alpha = trabo.m_alpha;
    m_beta = trabo.m_beta;
    m_objectAccessor = trabo.m_objectAccessor;
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::TrapezoidVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transform)
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;

  // face surfaces xy
  Amg::RotationMatrix3D trapezoidRotation(transform.rotation());
  Amg::Vector3D trapezoidX(trapezoidRotation.col(0));
  Amg::Vector3D trapezoidY(trapezoidRotation.col(1));
  Amg::Vector3D trapezoidZ(trapezoidRotation.col(2));
  Amg::Vector3D trapezoidCenter(transform.translation());

  //   (1) - at negative local z
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::AngleAxis3D(180 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
      Amg::Translation3D(Amg::Vector3D(0., 0., this->halflengthZ()))),
    this->faceXYTrapezoidBounds()));
  //   (2) - at positive local z
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::Translation3D(Amg::Vector3D(0., 0., this->halflengthZ()))),
    this->faceXYTrapezoidBounds()));
  // face surfaces yz
  // transmute cyclical
  //   (3) - at point A, attached to alpha opening angle
  Amg::Vector3D A(
    this->minHalflengthX(), this->halflengthY(), trapezoidCenter.z());
  Amg::RotationMatrix3D alphaZRotation =
    (s_idRotation *
     Amg::AngleAxis3D(this->alpha() - 0.5 * M_PI, Amg::Vector3D(0., 0., 1.)))
      .toRotationMatrix();
  // CLHEP::HepRotation  alphaRotation(alphaZRotation*trapezoidRotation);
  Amg::RotationMatrix3D faceAlphaRotation;
  faceAlphaRotation.col(0) = alphaZRotation.col(1);
  faceAlphaRotation.col(1) = -alphaZRotation.col(2);
  faceAlphaRotation.col(2) = -alphaZRotation.col(0);
  RectangleBounds* faceAlphaBounds = this->faceAlphaRectangleBounds();
  // Amg::Vector3D
  // faceAlphaPosition(A+faceAlphaRotation.colX()*faceAlphaBounds->halflengthX());
  Amg::Vector3D faceAlphaPosition0(
    -0.5 * (this->minHalflengthX() + this->maxHalflengthX()), 0., 0.);
  Amg::Vector3D faceAlphaPosition = transform * faceAlphaPosition0;
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      (trapezoidRotation * faceAlphaRotation) *
      Amg::Translation3D(faceAlphaPosition)),
    faceAlphaBounds));
  //   (4) - at point B, attached to beta opening angle
  Amg::Vector3D B(
    this->minHalflengthX(), -this->halflengthY(), trapezoidCenter.z());
  Amg::RotationMatrix3D betaZRotation =
    (s_idRotation *
     Amg::AngleAxis3D(-(this->beta() - 0.5 * M_PI), Amg::Vector3D(0., 0., 1.)))
      .toRotationMatrix();
  // CLHEP::HepRotation  betaRotation(betaZRotation*trapezoidRotation);
  Amg::RotationMatrix3D faceBetaRotation;
  faceBetaRotation.col(0) = betaZRotation.col(1);
  faceBetaRotation.col(1) = betaZRotation.col(2);
  faceBetaRotation.col(2) = betaZRotation.col(0);
  RectangleBounds* faceBetaBounds = this->faceBetaRectangleBounds();
  // Amg::Vector3D
  // faceBetaPosition(B+faceBetaRotation.colX()*faceBetaBounds->halflengthX());
  Amg::Vector3D faceBetaPosition0(
    0.5 * (this->minHalflengthX() + this->maxHalflengthX()), 0., 0.);
  Amg::Vector3D faceBetaPosition = transform * faceBetaPosition0;
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      trapezoidRotation * faceBetaRotation *
      Amg::Translation3D(faceBetaPosition)),
    faceBetaBounds));
  // face surfaces zx
  //   (5) - at negative local x
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::AngleAxis3D(180. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.)) *
      Amg::Translation3D(Amg::Vector3D(0., this->halflengthY(), 0.)) *
      Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
      Amg::AngleAxis3D(-90. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
    this->faceZXRectangleBoundsBottom()));
  //   (6) - at positive local x
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::Translation3D(Amg::Vector3D(0., this->halflengthY(), 0.)) *
      Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
      Amg::AngleAxis3D(-90. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
    this->faceZXRectangleBoundsTop()));

  return retsf;
}

// faces in xy
Trk::TrapezoidBounds*
Trk::TrapezoidVolumeBounds::faceXYTrapezoidBounds() const
{
  // return new Trk::TrapezoidBounds(m_minHalfX,m_halfY, m_alpha, m_beta);
  return new Trk::TrapezoidBounds(m_minHalfX, m_maxHalfX, m_halfY);
}

Trk::RectangleBounds*
Trk::TrapezoidVolumeBounds::faceAlphaRectangleBounds() const
{
  return new Trk::RectangleBounds(m_halfY / cos(m_alpha - 0.5 * M_PI), m_halfZ);
}

Trk::RectangleBounds*
Trk::TrapezoidVolumeBounds::faceBetaRectangleBounds() const
{
  return new Trk::RectangleBounds(m_halfY / cos(m_beta - 0.5 * M_PI), m_halfZ);
}

Trk::RectangleBounds*
Trk::TrapezoidVolumeBounds::faceZXRectangleBoundsBottom() const
{
  return new Trk::RectangleBounds(m_halfZ, m_minHalfX);
}

Trk::RectangleBounds*
Trk::TrapezoidVolumeBounds::faceZXRectangleBoundsTop() const
{
  // double delta = (m_alpha < m_beta) ? m_alpha - M_PI/2. : m_beta - M_PI/2.;
  // return new Trk::RectangleBounds(m_halfZ,
  // 0.5*(m_minHalfX+m_minHalfX+2.*m_halfY/cos(delta)));
  return new Trk::RectangleBounds(m_halfZ, m_maxHalfX);
}

bool
Trk::TrapezoidVolumeBounds::inside(const Amg::Vector3D& pos, double tol) const
{
  if (std::abs(pos.z()) > m_halfZ + tol)
    return false;
  if (std::abs(pos.y()) > m_halfY + tol)
    return false;
  Trk::TrapezoidBounds* faceXYBounds = this->faceXYTrapezoidBounds();
  Amg::Vector2D locp(pos.x(), pos.y());
  bool inside(faceXYBounds->inside(locp, tol, tol));
  delete faceXYBounds;
  return inside;
}

// ostream operator overload
MsgStream&
Trk::TrapezoidVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl
    << "Trk::TrapezoidVolumeBounds: (minhalfX, halfY, halfZ, alpha, beta) = ";
  temp_sl << "(" << m_minHalfX << ", " << m_halfY << ", " << m_halfZ;
  temp_sl << ", " << m_alpha << ", " << m_beta << ")";
  sl << temp_sl.str();
  return sl;
}

std::ostream&
Trk::TrapezoidVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl
    << "Trk::TrapezoidVolumeBounds: (minhalfX, halfY, halfZ, alpha, beta) = ";
  temp_sl << "(" << m_minHalfX << ", " << m_halfY << ", " << m_halfZ;
  temp_sl << ", " << m_alpha << ", " << m_beta << ")";
  sl << temp_sl.str();
  return sl;
}

