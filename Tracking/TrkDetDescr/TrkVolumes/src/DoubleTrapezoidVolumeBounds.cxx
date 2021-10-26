/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DoubleTrapezoidVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
// TrkSurfaces
#include "TrkSurfaces/DiamondBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// STD
#include <cmath>
#include <iomanip>
#include <iostream>

Trk::DoubleTrapezoidVolumeBounds::DoubleTrapezoidVolumeBounds()
  : VolumeBounds()
  , m_minHalfX(0.)
  , m_medHalfX(0.)
  , m_maxHalfX(0.)
  , m_halfY1(0.)
  , m_halfY2(0.)
  , m_halfZ(0.)
  , m_alpha1(0.)
  , m_alpha2(0.)
  , m_objectAccessor()
{}

Trk::DoubleTrapezoidVolumeBounds::DoubleTrapezoidVolumeBounds(
  double minhalex,
  double medhalex,
  double maxhalex,
  double haley1,
  double haley2,
  double halez)
  : VolumeBounds()
  , m_minHalfX(minhalex)
  , m_medHalfX(medhalex)
  , m_maxHalfX(maxhalex)
  , m_halfY1(haley1)
  , m_halfY2(haley2)
  , m_halfZ(halez)
  , m_alpha1(0.)
  , m_alpha2(0.)
  , m_objectAccessor()
{
  m_alpha1 = atan2(m_medHalfX - m_minHalfX, 2. * m_halfY1);
  m_alpha2 = atan2(m_medHalfX - m_maxHalfX, 2. * m_halfY2);
}

Trk::DoubleTrapezoidVolumeBounds::DoubleTrapezoidVolumeBounds(
  const Trk::DoubleTrapezoidVolumeBounds& trabo)
  : VolumeBounds()
  , m_minHalfX(trabo.m_minHalfX)
  , m_medHalfX(trabo.m_medHalfX)
  , m_maxHalfX(trabo.m_maxHalfX)
  , m_halfY1(trabo.m_halfY1)
  , m_halfY2(trabo.m_halfY2)
  , m_halfZ(trabo.m_halfZ)
  , m_alpha1(trabo.m_alpha1)
  , m_alpha2(trabo.m_alpha2)
  , m_objectAccessor(trabo.m_objectAccessor)
{}

Trk::DoubleTrapezoidVolumeBounds::~DoubleTrapezoidVolumeBounds() {}

Trk::DoubleTrapezoidVolumeBounds&
Trk::DoubleTrapezoidVolumeBounds::operator=(
  const Trk::DoubleTrapezoidVolumeBounds& trabo)
{
  if (this != &trabo) {
    m_minHalfX = trabo.m_minHalfX;
    m_medHalfX = trabo.m_medHalfX;
    m_maxHalfX = trabo.m_maxHalfX;
    m_halfY1 = trabo.m_halfY1;
    m_halfY2 = trabo.m_halfY2;
    m_halfZ = trabo.m_halfZ;
    m_alpha1 = trabo.m_alpha1;
    m_alpha2 = trabo.m_alpha2;
    m_objectAccessor = trabo.m_objectAccessor;
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::DoubleTrapezoidVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transform) 
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;

  // face surfaces xy
  Amg::RotationMatrix3D diamondRotation(transform.rotation());
  Amg::Vector3D diamondX(diamondRotation.col(0));
  Amg::Vector3D diamondY(diamondRotation.col(1));
  Amg::Vector3D diamondZ(diamondRotation.col(2));
  Amg::Vector3D diamondCenter(transform.translation());

  //   (1) - at negative local z
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::AngleAxis3D(180 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
      Amg::Translation3D(Amg::Vector3D(0., 0., this->halflengthZ()))),
    this->faceXYDiamondBounds()));
  //   (2) - at positive local z
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::Translation3D(Amg::Vector3D(0., 0., this->halflengthZ()))),
    this->faceXYDiamondBounds()));
  // face surfaces yz
  // transmute cyclical
  //   (3) - at point A, attached to alpha opening angle
  Amg::Vector3D A(
    diamondCenter - this->minHalflengthX() * diamondX -
    2 * this->halflengthY1() * diamondY);
  Amg::AngleAxis3D alpha1ZRotation(this->alpha1(), Amg::Vector3D(0., 0., 1.));
  Amg::RotationMatrix3D alpha1Rotation(
    diamondRotation * alpha1ZRotation *
    Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
    Amg::AngleAxis3D(90 * Gaudi::Units::deg, Amg::Vector3D(0., 0., 1.)));
  RectangleBounds* faceAlpha1Bounds = this->faceAlpha1RectangleBounds();
  Amg::Vector3D faceAlpha1Position(
    A + alpha1Rotation.col(0) * faceAlpha1Bounds->halflengthX());
  retsf->push_back(new Trk::PlaneSurface(
   Amg::Transform3D(
      alpha1Rotation * Amg::Translation3D(faceAlpha1Position)),
    faceAlpha1Bounds));
  //   (4) - at point B, attached to beta opening angle
  Amg::Vector3D B(
    diamondCenter + this->minHalflengthX() * diamondX -
    2 * this->halflengthY1() * diamondY);
  Amg::AngleAxis3D beta1ZRotation(-this->alpha1(), Amg::Vector3D(0., 0., 1.));
  Amg::RotationMatrix3D beta1Rotation(
    diamondRotation * beta1ZRotation *
    Amg::AngleAxis3D(90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
    Amg::AngleAxis3D(90 * Gaudi::Units::deg, Amg::Vector3D(0., 0., 1.)));
  RectangleBounds* faceBeta1Bounds = this->faceBeta1RectangleBounds();
  Amg::Vector3D faceBeta1Position(
    B + beta1Rotation.col(0) * faceBeta1Bounds->halflengthX());
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(beta1Rotation * Amg::Translation3D(faceBeta1Position)),
    faceBeta1Bounds));
  // face surfaces yz
  // transmute cyclical
  //   (5) - at point A', attached to alpha opening angle
  Amg::Vector3D AA(
    diamondCenter - this->maxHalflengthX() * diamondX +
    2 * this->halflengthY2() * diamondY);
  Amg::AngleAxis3D alpha2ZRotation(-this->alpha2(), Amg::Vector3D(0., 0., 1.));
  Amg::RotationMatrix3D alpha2Rotation(
    diamondRotation * alpha2ZRotation *
    Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
    Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 0., 1.)));
  RectangleBounds* faceAlpha2Bounds = this->faceAlpha2RectangleBounds();
  Amg::Vector3D faceAlpha2Position(
    AA + alpha2Rotation.col(0) * faceAlpha2Bounds->halflengthX());
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      alpha2Rotation * Amg::Translation3D(faceAlpha2Position)),
    faceAlpha2Bounds));
  //   (6) - at point B', attached to beta opening angle
  Amg::Vector3D BB(
    diamondCenter + this->maxHalflengthX() * diamondX +
    2 * this->halflengthY2() * diamondY);
  Amg::AngleAxis3D beta2ZRotation(this->alpha2(), Amg::Vector3D(0., 0., 1.));
  Amg::RotationMatrix3D beta2Rotation(
    diamondRotation * beta2ZRotation *
    Amg::AngleAxis3D(90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
    Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 0., 1.)));
  RectangleBounds* faceBeta2Bounds = this->faceBeta2RectangleBounds();
  Amg::Vector3D faceBeta2Position(
    BB + beta2Rotation.col(0) * faceBeta2Bounds->halflengthX());
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(beta2Rotation * Amg::Translation3D(faceBeta2Position)),
    faceBeta2Bounds));
  // face surfaces zx
  //   (7) - at negative local y
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::AngleAxis3D(180. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.)) *
      Amg::Translation3D(Amg::Vector3D(0., 2 * this->halflengthY1(), 0.)) *
      Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
      Amg::AngleAxis3D(-90. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
    this->faceZXRectangleBoundsBottom()));
  //   (8) - at positive local y
  retsf->push_back(new Trk::PlaneSurface(
    Amg::Transform3D(
      transform *
      Amg::Translation3D(Amg::Vector3D(0., this->halflengthY2(), 0.)) *
      Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 1., 0.)) *
      Amg::AngleAxis3D(-90. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
    this->faceZXRectangleBoundsTop()));

  return retsf;
}

// faces in xy
Trk::DiamondBounds*
Trk::DoubleTrapezoidVolumeBounds::faceXYDiamondBounds() const
{
  return new Trk::DiamondBounds(
    m_minHalfX, m_medHalfX, m_maxHalfX, m_halfY1, m_halfY2);
}

Trk::RectangleBounds*
Trk::DoubleTrapezoidVolumeBounds::faceAlpha1RectangleBounds() const
{
  return new Trk::RectangleBounds(m_halfY1 / cos(m_alpha1), m_halfZ);
}

Trk::RectangleBounds*
Trk::DoubleTrapezoidVolumeBounds::faceAlpha2RectangleBounds() const
{
  return new Trk::RectangleBounds(m_halfY2 / cos(m_alpha2), m_halfZ);
}

Trk::RectangleBounds*
Trk::DoubleTrapezoidVolumeBounds::faceBeta1RectangleBounds() const
{
  return new Trk::RectangleBounds(m_halfY1 / cos(m_alpha1), m_halfZ);
}

Trk::RectangleBounds*
Trk::DoubleTrapezoidVolumeBounds::faceBeta2RectangleBounds() const
{
  return new Trk::RectangleBounds(m_halfY2 / cos(m_alpha2), m_halfZ);
}

Trk::RectangleBounds*
Trk::DoubleTrapezoidVolumeBounds::faceZXRectangleBoundsBottom() const
{
  return new Trk::RectangleBounds(m_halfZ, m_minHalfX);
}

Trk::RectangleBounds*
Trk::DoubleTrapezoidVolumeBounds::faceZXRectangleBoundsTop() const
{
  return new Trk::RectangleBounds(m_halfZ, m_maxHalfX);
}

// Trk::RectangleBounds*
// Trk::DoubleTrapezoidVolumeBounds::faceZXRectangleBoundsTop() const
//{
//    double delta = (m_alpha < m_beta) ? m_alpha - M_PI/2. : m_beta - M_PI/2.;
//    return new Trk::RectangleBounds(m_halfZ,
//    0.5*(m_minHalfX+m_minHalfX+2.*m_halfY/cos(delta)));
//}

bool
Trk::DoubleTrapezoidVolumeBounds::inside(const Amg::Vector3D& pos, double tol)
  const
{
  if (std::abs(pos.z()) > m_halfZ + tol)
    return false;
  if (pos.y() < -2 * m_halfY1 - tol)
    return false;
  if (pos.y() > 2 * m_halfY2 - tol)
    return false;
  Trk::DiamondBounds* faceXYBounds = this->faceXYDiamondBounds();
  Amg::Vector2D locp(pos.x(), pos.y());
  bool inside(faceXYBounds->inside(locp, tol));
  delete faceXYBounds;
  return inside;
}

// ostream operator overload
MsgStream&
Trk::DoubleTrapezoidVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::DoubleTrapezoidVolumeBounds: (minhalfX, medhalfX, maxhalfX, "
             "halfY1, halfY2, halfZ) = ";
  temp_sl << "(" << m_minHalfX << ", " << m_medHalfX << ", " << m_maxHalfX;
  temp_sl << ", " << m_halfY1 << ", " << m_halfY2 << ", " << m_halfZ << ")";
  sl << temp_sl.str();
  return sl;
}

std::ostream&
Trk::DoubleTrapezoidVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(7);
  temp_sl << "Trk::DoubleTrapezoidVolumeBounds: ) =(minhalfX, medhalfX, "
             "maxhalfX, halfY1, halfY2, halfZ) ";
  temp_sl << "(" << m_minHalfX << ", " << m_medHalfX << ", " << m_maxHalfX;
  temp_sl << ", " << m_halfY1 << ", " << m_halfY2 << ", " << m_halfZ << ")";
  sl << temp_sl.str();
  return sl;
}

