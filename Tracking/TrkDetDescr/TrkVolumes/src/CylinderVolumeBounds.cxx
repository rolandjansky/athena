/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CylinderVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/CylinderIntersector.h"
// TrkSurfaces
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"

// STD
#include <cmath>
#include <iostream>

const double Trk::CylinderVolumeBounds::s_numericalStable =
  10e-2 * Gaudi::Units::mm;

Trk::CylinderVolumeBounds::CylinderVolumeBounds()
  : VolumeBounds()
  , m_innerRadius(0.)
  , m_outerRadius(0.)
  , m_halfPhiSector(0.)
  , m_halfZ(0.)
  , m_boundaryAccessors()
{}

Trk::CylinderVolumeBounds::CylinderVolumeBounds(double radius, double halez)
  : VolumeBounds()
  , m_innerRadius(0.)
  , m_outerRadius(std::abs(radius))
  , m_halfPhiSector(M_PI)
  , m_halfZ(std::abs(halez))
  , m_boundaryAccessors()
{}

Trk::CylinderVolumeBounds::CylinderVolumeBounds(
  double rinner,
  double router,
  double halez)
  : VolumeBounds()
  , m_innerRadius(std::abs(rinner))
  , m_outerRadius(std::abs(router))
  , m_halfPhiSector(M_PI)
  , m_halfZ(std::abs(halez))
  , m_boundaryAccessors()
{}

Trk::CylinderVolumeBounds::CylinderVolumeBounds(
  double rinner,
  double router,
  double haphi,
  double halez)
  : VolumeBounds()
  , m_innerRadius(std::abs(rinner))
  , m_outerRadius(std::abs(router))
  , m_halfPhiSector(haphi)
  , m_halfZ(std::abs(halez))
  , m_boundaryAccessors()
{}

Trk::CylinderVolumeBounds::CylinderVolumeBounds(
  const Trk::CylinderVolumeBounds& cylbo)
  : VolumeBounds()
  , m_innerRadius(cylbo.m_innerRadius)
  , m_outerRadius(cylbo.m_outerRadius)
  , m_halfPhiSector(cylbo.m_halfPhiSector)
  , m_halfZ(cylbo.m_halfZ)
  , m_boundaryAccessors()
{}

Trk::CylinderVolumeBounds::~CylinderVolumeBounds() {}

Trk::CylinderVolumeBounds&
Trk::CylinderVolumeBounds::operator=(const Trk::CylinderVolumeBounds& cylbo)
{
  if (this != &cylbo) {
    m_innerRadius = cylbo.m_innerRadius;
    m_outerRadius = cylbo.m_outerRadius;
    m_halfPhiSector = cylbo.m_halfPhiSector;
    m_halfZ = cylbo.m_halfZ;
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::CylinderVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transform)
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;
  // memory optimisation --- reserve the maximum
  retsf->reserve(6);

  // check if the transform is approximatively the identity
  bool isConcentric = transform.isApprox(Amg::Transform3D::Identity());

  Amg::RotationMatrix3D discRot(transform.rotation());
  Amg::Vector3D cylCenter(transform.translation());

  // bottom Disc (negative z)
  Amg::RotationMatrix3D bottomDiscRot;
  bottomDiscRot.col(0) = discRot.col(1);
  bottomDiscRot.col(1) = discRot.col(0);
  bottomDiscRot.col(2) = -discRot.col(2);
  retsf->push_back(new Trk::DiscSurface(
    Amg::Transform3D(
      transform * Amg::AngleAxis3D(M_PI, Amg::Vector3D(1., 0., 0.)) *
      Amg::Translation3D(Amg::Vector3D(0., 0., halflengthZ()))),
    bottomDiscBounds()));
  // top Disc (positive z)
  retsf->push_back(new Trk::DiscSurface(
    Amg::Transform3D(
      discRot * Amg::Translation3D(cylCenter + halflengthZ() * discRot.col(2))),
    topDiscBounds()));
  // outer Cylinder
  if (!isConcentric)
    retsf->push_back(new Trk::CylinderSurface(
      Amg::Transform3D(transform), outerCylinderBounds()));
  else
    retsf->push_back(new Trk::CylinderSurface(outerCylinderBounds()));

  // innermost Cylinder
  if (innerRadius() > s_numericalStable) {
    if (!isConcentric)
      retsf->push_back(new Trk::CylinderSurface(
        Amg::Transform3D(transform), innerCylinderBounds()));
    else
      retsf->push_back(new Trk::CylinderSurface(innerCylinderBounds()));
  }

  if (std::abs(halfPhiSector() - M_PI) > s_numericalStable) {
    // sectorPlane 1 (negative phi)
    retsf->push_back(new Trk::PlaneSurface(
      Amg::Transform3D(
        transform *
        Amg::AngleAxis3D(-halfPhiSector(), Amg::Vector3D(0., 0., 1.)) *
        Amg::Translation3D(Amg::Vector3D(mediumRadius(), 0., 0.)) *
        Amg::AngleAxis3D(M_PI / 2, Amg::Vector3D(1., 0., 0.))),
      sectorPlaneBounds()));
    // sectorPlane 2 (positive phi)
    retsf->push_back(new Trk::PlaneSurface(
      Amg::Transform3D(
        transform *
        Amg::AngleAxis3D(halfPhiSector(), Amg::Vector3D(0., 0., 1.)) *
        Amg::Translation3D(Amg::Vector3D(mediumRadius(), 0., 0.)) *
        Amg::AngleAxis3D(-M_PI / 2, Amg::Vector3D(1., 0., 0.))),
      sectorPlaneBounds()));
  }
  return retsf;
}

Trk::ObjectAccessor
Trk::CylinderVolumeBounds::boundarySurfaceAccessor(
  const Amg::Vector3D& gp,
  const Amg::Vector3D& dir,
  bool) const
{
  // the tube case - most likely
  if (
    m_innerRadius > s_numericalStable &&
    std::abs(m_halfPhiSector - M_PI) < s_numericalStable) {
    // prepare the data
    double posZ = gp.z();
    double posR = gp.perp();
    // difference Vector
    Amg::Vector3D diff(gp + dir);
    // differences
    double deltaZ = diff.z() - posZ;
    double deltaR = diff.perp() - posR;

    // the isOnSurface cases + switchers (that change with the cases)
    bool isOnFace = false;
    bool intersectionIndicator = (deltaR > 0.);
    bool choiceIndicator = false;

    // on surface or look-a-likes
    // on zMin or slightly outside
    if (
      std::abs(posZ + m_halfZ) < s_numericalStable ||
      (posZ < (-m_halfZ) && deltaZ > 0.)) {
      isOnFace = true;
    } else if (
      std::abs(posZ - m_halfZ) < s_numericalStable ||
      (posZ > m_halfZ && deltaZ < 0.)) {
      // on zMax or slighly outside
      isOnFace = true;
    } else if (
      std::abs(posR - m_innerRadius) < s_numericalStable ||
      (posR < m_innerRadius && deltaR > 0.)) {
      // on innerRadius or slightly outside
      isOnFace = true;
      choiceIndicator =
        (deltaZ > 0.); // the side choice indicator <========== WRONG ==========
    } else if (
      std::abs(posR - m_outerRadius) < s_numericalStable ||
      (posR > m_outerRadius && deltaR < 0.)) {
      // on outRadius or slightly outside
      isOnFace = true;
      choiceIndicator =
        (deltaZ > 0.); // the side choice indicator <========== WRONG ==========
    }

    // the onSurface case
    // =================================================================================
    if (isOnFace) {
      if (intersectionIndicator) {
        // intersect the Rmax / with floatint point exception check
        Trk::CylinderIntersector intersectRmax(
          posR, posZ, deltaR != 0 ? deltaZ / deltaR : 0, m_outerRadius);
        double zOfIntersect = intersectRmax.yOfX;
        // now check if the intersect is inside m_halfZ
        if (std::abs(zOfIntersect) <= m_halfZ)
          return Trk::ObjectAccessor(
            (choiceIndicator || zOfIntersect > 0.)
              ? m_boundaryAccessors.tubeAccessor(Trk::TubeRincreaseZincrease)
              : m_boundaryAccessors.tubeAccessor(Trk::TubeRincreaseZdecrease));
        // if the intersect is outside
        return Trk::ObjectAccessor(
          (choiceIndicator || zOfIntersect > 0.)
            ? m_boundaryAccessors.tubeAccessor(Trk::TubeZincreaseRincrease)
            : m_boundaryAccessors.tubeAccessor(Trk::TubeZdecreaseRincrease));
      }
      // intersect the Rmin
      Trk::CylinderIntersector intersectRmin(
        posR, posZ, deltaR != 0 ? deltaZ / deltaR : 0, m_innerRadius);
      double zOfIntersect = intersectRmin.yOfX;
      // now check if the intersect is inside m_halfZ
      if (std::abs(zOfIntersect) <= m_halfZ)
        return Trk::ObjectAccessor(
          (choiceIndicator || zOfIntersect > 0.)
            ? m_boundaryAccessors.tubeAccessor(Trk::TubeRdecreaseZincrease)
            : m_boundaryAccessors.tubeAccessor(Trk::TubeRdecreaseZdecrease));
      // if the intersect is outside
      return Trk::ObjectAccessor(
        (choiceIndicator || zOfIntersect > 0.)
          ? m_boundaryAccessors.tubeAccessor(Trk::TubeZincreaseRdecrease)
          : m_boundaryAccessors.tubeAccessor(Trk::TubeZdecreaseRdecrease));
    }
    // =================================================================================================

    // ======================= the inside/outside part remains
    // =========================================
    //  (a) outside cases
    if (posR < m_innerRadius && deltaR < 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.tubeAccessor(Trk::TubeOutsideRminRdecrease));
    if (posR > m_outerRadius && deltaR > 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.tubeAccessor(Trk::TubeOutsideRmaxRincrease));
    if (posZ < -m_halfZ && deltaZ < 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.tubeAccessor(Trk::TubeOutsideZminZdecrease));
    if (posZ > m_halfZ && deltaZ > 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.tubeAccessor(Trk::TubeOutsideZmaxZincrease));
    // (b) inside cases
    // the increase R case
    if (deltaR > 0.) {
      // solve the linear equation for the outer Radius - with floating point
      // exception check
      Trk::CylinderIntersector intersectRmax(
        posR, posZ, deltaR != 0 ? deltaZ / deltaR : 0, m_outerRadius);
      double zOfIntersect = intersectRmax.yOfX;

      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeRincreaseZincrease));
      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect < 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeRincreaseZdecrease));
      if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect < 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeZdecreaseRincrease));
      if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeZincreaseRincrease));

    } else {
      // solve the linear equation for the inner Radius with floating point
      // exceptin check
      Trk::CylinderIntersector intersectRmin(
        posR, posZ, deltaR != 0 ? deltaZ / deltaR : 0, m_innerRadius);
      double zOfIntersect = intersectRmin.yOfX;

      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeRdecreaseZincrease));
      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect < 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeRdecreaseZdecrease));
      if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(
          m_boundaryAccessors.tubeAccessor(Trk::TubeZincreaseRdecrease));
      return Trk::ObjectAccessor(
        m_boundaryAccessors.tubeAccessor(Trk::TubeZdecreaseRdecrease));
    }
  }
  // the cylinder case
  if (
    m_innerRadius < s_numericalStable &&
    std::abs(m_halfPhiSector - M_PI) < s_numericalStable) {

    // ----------------------------- z / r information
    double posZ = gp.z();
    double posR = gp.perp();

    // is on R case
    bool isOnCylinder = (std::abs(posR - m_outerRadius) < s_numericalStable);

    // difference Vector
    Amg::Vector3D diff(gp + dir);
    // differences
    double deltaZ = diff.z() - posZ;
    double deltaR = diff.perp() - posR;
    // take the sign of the intersect
    int radiusSign = deltaR > 0. ? 1 : -1;

    // solve the linear equation for the cylinder Radius / with floating point
    // exceptin check
    Trk::CylinderIntersector intersectR(
      posR,
      posZ,
      deltaR != 0 ? deltaZ / deltaR : 0,
      radiusSign * m_outerRadius);

    //  the intersection point - a little trick for the OnFace cases
    double zOfIntersect = intersectR.yOfX;
    // check if intersection exists
    bool intersectsCylinder =
      !isOnCylinder ? (zOfIntersect * zOfIntersect <= m_halfZ * m_halfZ)
                    : false;

    // return the cases for going through the cylinder
    if (intersectsCylinder && zOfIntersect > 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.cylinderAccessor(Trk::CylinderZincrease));
    if (intersectsCylinder && zOfIntersect <= 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.cylinderAccessor(Trk::CylinderZdecrease));
    if (deltaZ > 0.)
      return Trk::ObjectAccessor(
        m_boundaryAccessors.cylinderAccessor(Trk::CylinderPositiveFace));
    return Trk::ObjectAccessor(
      m_boundaryAccessors.cylinderAccessor(Trk::CylinderNegativeFace));
  }
  // the sectoral cylinder case
  if (m_innerRadius != 0. && std::abs(m_halfPhiSector - M_PI) > 10e-3)
    return Trk::ObjectAccessor(m_boundaryAccessors.sectoralCylinderAccessor(
      Trk::StandardSectoralCylinder));
  // it remains the sectoral tube case
  return Trk::ObjectAccessor(
    m_boundaryAccessors.sectoralTubeAccessor(Trk::StandardSectoralTube));
}

Trk::CylinderBounds*
Trk::CylinderVolumeBounds::innerCylinderBounds() const
{
  return new Trk::CylinderBounds(m_innerRadius, m_halfPhiSector, m_halfZ);
}

Trk::CylinderBounds*
Trk::CylinderVolumeBounds::outerCylinderBounds() const
{
  return new Trk::CylinderBounds(m_outerRadius, m_halfPhiSector, m_halfZ);
}

Trk::DiscBounds*
Trk::CylinderVolumeBounds::bottomDiscBounds() const
{
  return new Trk::DiscBounds(m_innerRadius, m_outerRadius, m_halfPhiSector);
}

Trk::RectangleBounds*
Trk::CylinderVolumeBounds::sectorPlaneBounds() const
{
  return new Trk::RectangleBounds(
    0.5 * (m_outerRadius - m_innerRadius), m_halfZ);
}

// ostream operator overload

MsgStream&
Trk::CylinderVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(2);
  temp_sl << "Trk::CylinderVolumeBounds: (rMin, rMax, halfPhi, halfZ) = ";
  temp_sl << m_innerRadius << ", " << m_outerRadius << ", " << m_halfPhiSector
          << ", " << m_halfZ;
  sl << temp_sl.str();
  return sl;
}

std::ostream&
Trk::CylinderVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream temp_sl;
  temp_sl << std::setiosflags(std::ios::fixed);
  temp_sl << std::setprecision(2);
  temp_sl << "Trk::CylinderVolumeBounds: (rMin, rMax, halfPhi, halfZ) = ";
  temp_sl << m_innerRadius << ", " << m_outerRadius << ", " << m_halfPhiSector
          << ", " << m_halfZ;
  sl << temp_sl.str();
  return sl;
}

