/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BevelledCylinderVolumeBounds.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Trk
#include "TrkVolumes/BevelledCylinderVolumeBounds.h"
// TrkSurfaces
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
//#include "TrkGeometrySurfaces/BevelledCylinderBounds.h"
#include "TrkGeometrySurfaces/SubtractedDiscSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/EllipseBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/VolumeExcluder.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SystemOfUnits.h"
// STD
#include <cmath>
#include <iostream>

const double Trk::BevelledCylinderVolumeBounds::s_numericalStable =
  10e-2 * Gaudi::Units::millimeter;

Trk::BevelledCylinderVolumeBounds::BevelledCylinderVolumeBounds()
  : VolumeBounds()
  , m_innerRadius(0.)
  , m_outerRadius(0.)
  , m_halfPhiSector(0.)
  , m_halfZ(0.)
  , m_thetaMinus(0.)
  , m_thetaPlus(0.)
  , m_type(-1)
  , m_boundaryAccessors()
  , m_subtractedVolume(nullptr)
{}

Trk::BevelledCylinderVolumeBounds::BevelledCylinderVolumeBounds(
  double rinner,
  double router,
  double haphi,
  double halez,
  int type)
  : VolumeBounds()
  , m_innerRadius(std::abs(rinner))
  , m_outerRadius(std::abs(router))
  , m_halfPhiSector(haphi)
  , m_halfZ(std::abs(halez))
  , m_thetaMinus(0.)
  , m_thetaPlus(0.)
  , m_type(type)
  , m_boundaryAccessors()
  , m_subtractedVolume(nullptr)
{}

Trk::BevelledCylinderVolumeBounds::BevelledCylinderVolumeBounds(
  const Trk::BevelledCylinderVolumeBounds& cylbo)
  : VolumeBounds()
  , m_innerRadius(cylbo.m_innerRadius)
  , m_outerRadius(cylbo.m_outerRadius)
  , m_halfPhiSector(cylbo.m_halfPhiSector)
  , m_halfZ(cylbo.m_halfZ)
  , m_thetaMinus(cylbo.m_thetaMinus)
  , m_thetaPlus(cylbo.m_thetaPlus)
  , m_type(cylbo.m_type)
  , m_boundaryAccessors()
  , m_subtractedVolume(nullptr)
{}

Trk::BevelledCylinderVolumeBounds::~BevelledCylinderVolumeBounds() {}

Trk::BevelledCylinderVolumeBounds&
Trk::BevelledCylinderVolumeBounds::operator=(
  const Trk::BevelledCylinderVolumeBounds& cylbo)
{
  if (this != &cylbo) {
    m_innerRadius = cylbo.m_innerRadius;
    m_outerRadius = cylbo.m_outerRadius;
    m_halfPhiSector = cylbo.m_halfPhiSector;
    m_halfZ = cylbo.m_halfZ;
    m_thetaMinus = cylbo.m_thetaMinus;
    m_thetaPlus = cylbo.m_thetaPlus;
    m_type = cylbo.m_type;
    m_subtractedVolume.store(nullptr);
  }
  return *this;
}

const std::vector<const Trk::Surface*>*
  Trk::BevelledCylinderVolumeBounds::decomposeToSurfaces
  (const Amg::Transform3D& transform)
{
  std::vector<const Trk::Surface*>* retsf =
    new std::vector<const Trk::Surface*>;
  // memory optimisation (reserve a save number of 20)
  retsf->reserve(6);

  Amg::RotationMatrix3D discRot(transform.rotation());
  Amg::Vector3D cylCenter(transform.translation());

  // Lazy init m_m_subtractedVolume
  if (m_type > -1 && !m_subtractedVolume) {
    m_subtractedVolume.set(std::unique_ptr<Trk::Volume>(subtractedVolume()));
  }

  // bottom Ellipse/Disc (negative z)
  if (m_type < 0)
    retsf->push_back(new Trk::PlaneSurface(
      Amg::Transform3D(
        (discRot *
         Amg::AngleAxis3D(-m_thetaMinus + M_PI, Amg::Vector3D(0., 1., 0.))) *
        Amg::Translation3D(
          cylCenter - (halflengthZ() - m_outerRadius * tan(m_thetaMinus)) *
                        discRot.col(2))),
      bottomEllipseBounds()));
  else {
    if (m_subtractedVolume) {
      Trk::Volume* subtrVol = new Trk::Volume(*m_subtractedVolume);
      Trk::DiscSurface bottomDisc(
        Amg::Transform3D(
          transform * Amg::AngleAxis3D(M_PI, Amg::Vector3D(1., 0., 0.)) *
          Amg::Translation3D(Amg::Vector3D(0., 0., halflengthZ()))),
        discBounds());
      retsf->push_back(new Trk::SubtractedDiscSurface(
        bottomDisc, new Trk::VolumeExcluder(subtrVol), false));
    } else
      retsf->push_back(new Trk::DiscSurface(
        Amg::Transform3D(
          transform * Amg::AngleAxis3D(M_PI, Amg::Vector3D(1., 0., 0.)) *
          Amg::Translation3D(Amg::Vector3D(0., 0., halflengthZ()))),
        discBounds()));
  }

  // top Ellipse/Disc (positive z)
  if (m_type < 0)
    retsf->push_back(new Trk::PlaneSurface(
      Amg::Transform3D(
        discRot * Amg::AngleAxis3D(m_thetaPlus, Amg::Vector3D(0., 1., 0.)) *
        Amg::Translation3D(
          cylCenter +
          (halflengthZ() - m_outerRadius * tan(m_thetaPlus)) * discRot.col(2))),
      topEllipseBounds()));
  else {
    if (m_subtractedVolume) {
      Trk::Volume* subtrVol = new Trk::Volume(*m_subtractedVolume);
      Trk::DiscSurface topDisc(
        Amg::Transform3D(
          transform * Amg::Translation3D(Amg::Vector3D(0., 0., halflengthZ()))),
        discBounds());
      retsf->push_back(new Trk::SubtractedDiscSurface(
        topDisc, new Trk::VolumeExcluder(subtrVol), false));
    } else
      retsf->push_back(new Trk::DiscSurface(
        Amg::Transform3D(
          transform * Amg::Translation3D(Amg::Vector3D(0., 0., halflengthZ()))),
        discBounds()));
  }

  // outer BevelledCylinder/Plane
  if (m_type < 0)
    retsf->push_back(new Trk::CylinderSurface(
      Amg::Transform3D(transform), outerBevelledCylinderBounds()));
  else if (m_type < 2)
    retsf->push_back(new Trk::CylinderSurface(
      Amg::Transform3D(transform), outerCylinderBounds()));
  else
    retsf->push_back(new Trk::PlaneSurface(
      Amg::Transform3D(
        transform *
        Amg::Translation3D(Amg::Vector3D(this->outerRadius(), 0., 0.)) *
        Amg::AngleAxis3D(-90 * Gaudi::Units::deg, Amg::Vector3D(0., 0., 1.)) *
        Amg::AngleAxis3D(-90. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
      outerBevelledPlaneBounds()));

  // inner BevelledCylinder/Plane
  if (innerRadius() > s_numericalStable) {
    if (m_type < 1)
      retsf->push_back(new Trk::CylinderSurface(
        Amg::Transform3D(transform), innerBevelledCylinderBounds()));
    else if (m_type == 2)
      retsf->push_back(new Trk::CylinderSurface(
        Amg::Transform3D(transform), innerCylinderBounds()));
    else
      retsf->push_back(new Trk::PlaneSurface(
        Amg::Transform3D(
          transform *
          Amg::Translation3D(Amg::Vector3D(this->innerRadius(), 0., 0.)) *
          Amg::AngleAxis3D(+90 * Gaudi::Units::deg, Amg::Vector3D(0., 0., 1.)) *
          Amg::AngleAxis3D(
            -90. * Gaudi::Units::deg, Amg::Vector3D(1., 0., 0.))),
        innerBevelledPlaneBounds()));
  }

  if (std::abs(halfPhiSector() - M_PI) > s_numericalStable) {
    if (m_type < 0) {
      // sectorPlane 1 (negative phi)
      retsf->push_back(new Trk::PlaneSurface(
        Amg::Transform3D(
          transform *
          Amg::AngleAxis3D(-halfPhiSector(), Amg::Vector3D(0., 0., 1.)) *
          Amg::Translation3D(Amg::Vector3D(mediumRadius(), 0., 0.)) *
          Amg::AngleAxis3D(M_PI / 2, Amg::Vector3D(1., 0., 0.))),
        sectorTrdBounds()));
      // sectorPlane 2 (positive phi)
      retsf->push_back(new Trk::PlaneSurface(
        Amg::Transform3D(
          transform *
          Amg::AngleAxis3D(halfPhiSector(), Amg::Vector3D(0., 0., 1.)) *
          Amg::Translation3D(Amg::Vector3D(mediumRadius(), 0., 0.)) *
          Amg::AngleAxis3D(-M_PI / 2, Amg::Vector3D(1., 0., 0.))),
        sectorTrdBounds()));
    } else {
      // sectorPlane 1 (negative phi)
      double ri = innerRadius();
      double ro = outerRadius();
      if (m_type == 1 || m_type == 3)
        ri *= 1. / cos(halfPhiSector());
      if (m_type > 1)
        ro *= 1. / cos(halfPhiSector());
      retsf->push_back(new Trk::PlaneSurface(
        Amg::Transform3D(
          transform *
          Amg::AngleAxis3D(-halfPhiSector(), Amg::Vector3D(0., 0., 1.)) *
          Amg::Translation3D(Amg::Vector3D(0.5 * (ri + ro), 0., 0.)) *
          Amg::AngleAxis3D(M_PI / 2, Amg::Vector3D(1., 0., 0.))),
        sectorPlaneBounds()));
      // sectorPlane 2 (positive phi)
      retsf->push_back(new Trk::PlaneSurface(
        Amg::Transform3D(
          transform *
          Amg::AngleAxis3D(halfPhiSector(), Amg::Vector3D(0., 0., 1.)) *
          Amg::Translation3D(Amg::Vector3D(0.5 * (ri + ro), 0., 0.)) *
          Amg::AngleAxis3D(-M_PI / 2, Amg::Vector3D(1., 0., 0.))),
        sectorPlaneBounds()));
    }
  }
  return retsf;
}

Trk::ObjectAccessor
Trk::BevelledCylinderVolumeBounds::boundarySurfaceAccessor(
  const Amg::Vector3D& gp,
  const Amg::Vector3D& dir,
  bool) const
{
  // the tube case - most likely
  if (m_innerRadius != 0. && std::abs(m_halfPhiSector - M_PI) < s_numericalStable) {
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
        // intersect the Rmax
        Trk::BevelledBoundaryIntersector intersectRmax(
          posR, posZ, deltaZ / deltaR, m_outerRadius);
        double zOfIntersect = intersectRmax.yOfX;
        // now check if the intersect is inside m_halfZ
        if (std::abs(zOfIntersect) <= m_halfZ)
          return Trk::ObjectAccessor(
            (choiceIndicator || zOfIntersect > 0.)
              ? m_boundaryAccessors.bevelledtubeAccessor(
                  Trk::BevelledTubeRincreaseZincrease)
              : m_boundaryAccessors.bevelledtubeAccessor(
                  Trk::BevelledTubeRincreaseZdecrease));
        // if the intersect is outside
        return Trk::ObjectAccessor(
          (choiceIndicator || zOfIntersect > 0.)
            ? m_boundaryAccessors.bevelledtubeAccessor(
                Trk::BevelledTubeZincreaseRincrease)
            : m_boundaryAccessors.bevelledtubeAccessor(
                Trk::BevelledTubeZdecreaseRincrease));
      }
      // intersect the Rmin
      Trk::BevelledBoundaryIntersector intersectRmin(
        posR, posZ, deltaZ / deltaR, m_innerRadius);
      double zOfIntersect = intersectRmin.yOfX;
      // now check if the intersect is inside m_halfZ
      if (std::abs(zOfIntersect) <= m_halfZ)
        return Trk::ObjectAccessor(
          (choiceIndicator || zOfIntersect > 0.)
            ? m_boundaryAccessors.bevelledtubeAccessor(
                Trk::BevelledTubeRdecreaseZincrease)
            : m_boundaryAccessors.bevelledtubeAccessor(
                Trk::BevelledTubeRdecreaseZdecrease));
      // if the intersect is outside
      return Trk::ObjectAccessor(
        (choiceIndicator || zOfIntersect > 0.)
          ? m_boundaryAccessors.bevelledtubeAccessor(
              Trk::BevelledTubeZincreaseRdecrease)
          : m_boundaryAccessors.bevelledtubeAccessor(
              Trk::BevelledTubeZdecreaseRdecrease));
    }
    // =================================================================================================

    // ======================= the inside/outside part remains
    // =========================================
    //  (a) outside cases
    if (posR < m_innerRadius && deltaR < 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
        Trk::BevelledTubeOutsideRminRdecrease));
    if (posR > m_outerRadius && deltaR > 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
        Trk::BevelledTubeOutsideRmaxRincrease));
    if (posZ < -m_halfZ && deltaZ < 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
        Trk::BevelledTubeOutsideZminZdecrease));
    if (posZ > m_halfZ && deltaZ > 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
        Trk::BevelledTubeOutsideZmaxZincrease));
    // (b) inside cases
    // the increase R case
    if (deltaR > 0.) {
      // solve the linear equation for the outer Radius
      Trk::BevelledBoundaryIntersector intersectRmax(
        posR, posZ, deltaZ / deltaR, m_outerRadius);
      double zOfIntersect = intersectRmax.yOfX;

      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeRincreaseZincrease));
      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect < 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeRincreaseZdecrease));
      if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect < 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeZdecreaseRincrease));
      if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeZincreaseRincrease));

    } else {
      // solve the linear equation for the inner Radius
      Trk::BevelledBoundaryIntersector intersectRmin(
        posR, posZ, deltaZ / deltaR, m_innerRadius);
      double zOfIntersect = intersectRmin.yOfX;

      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeRdecreaseZincrease));
      if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect < 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeRdecreaseZdecrease));
      if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect > 0.)
        return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
          Trk::BevelledTubeZincreaseRdecrease));
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledtubeAccessor(
        Trk::BevelledTubeZdecreaseRdecrease));
    }
  }
  // the cylinder case
  if (m_innerRadius == 0. && std::abs(m_halfPhiSector - M_PI) < 10e-3) {

    double posZ = gp.z();
    double posR = gp.perp();
    // difference Vector
    Amg::Vector3D diff(gp + dir);
    // differences
    double deltaZ = diff.z() - posZ;
    double deltaR = diff.perp() - posR;

    // solve the linear equation for the cylinder Radius
    Trk::BevelledBoundaryIntersector intersectR(
      posR, posZ, deltaZ / deltaR, m_outerRadius);
    //  the intersection point
    double zOfIntersect = intersectR.yOfX;

    if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect > 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledcylinderAccessor(
        Trk::BevelledCylinderZincrease));
    if (std::abs(zOfIntersect) <= m_halfZ && zOfIntersect < 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledcylinderAccessor(
        Trk::BevelledCylinderZdecrease));
    if (std::abs(zOfIntersect) > m_halfZ && zOfIntersect > 0.)
      return Trk::ObjectAccessor(m_boundaryAccessors.bevelledcylinderAccessor(
        Trk::BevelledCylinderPositiveFace));
    return Trk::ObjectAccessor(m_boundaryAccessors.bevelledcylinderAccessor(
      Trk::BevelledCylinderNegativeFace));
  }
  // the sectoral cylinder case
  if (m_innerRadius != 0. && std::abs(m_halfPhiSector - M_PI) > 10e-3)
    return Trk::ObjectAccessor(
      m_boundaryAccessors.sectoralBevelledCylinderAccessor(
        Trk::StandardSectoralBevelledCylinder));
  // it remains the sectoral tube case
  return Trk::ObjectAccessor(m_boundaryAccessors.sectoralBevelledTubeAccessor(
    Trk::StandardSectoralBevelledTube));
}

Trk::CylinderBounds*
Trk::BevelledCylinderVolumeBounds::innerBevelledCylinderBounds() const
{
  //    return new Trk::BevelledCylinderBounds(m_innerRadius, m_halfPhiSector,
  //    m_halfZ-(m_outerRadius-m_innerRadius)*tan(m_thetaPlus), m_thetaMinus,
  //    m_thetaPlus);
  return new Trk::CylinderBounds(m_innerRadius, m_halfPhiSector, m_halfZ);
}

Trk::CylinderBounds*
Trk::BevelledCylinderVolumeBounds::outerBevelledCylinderBounds() const
{
  //    return new Trk::BevelledCylinderBounds(m_outerRadius, m_halfPhiSector,
  //    m_halfZ, m_thetaMinus, m_thetaPlus);
  return new Trk::CylinderBounds(m_outerRadius, m_halfPhiSector, m_halfZ);
}

Trk::RectangleBounds*
Trk::BevelledCylinderVolumeBounds::outerBevelledPlaneBounds() const
{
  return new Trk::RectangleBounds(
    m_outerRadius * tan(m_halfPhiSector), m_halfZ);
}

Trk::RectangleBounds*
Trk::BevelledCylinderVolumeBounds::innerBevelledPlaneBounds() const
{
  return new Trk::RectangleBounds(
    m_innerRadius * tan(m_halfPhiSector), m_halfZ);
}

Trk::EllipseBounds*
Trk::BevelledCylinderVolumeBounds::bottomEllipseBounds() const
{
  //    return new Trk::EllipseBounds();
  return new Trk::EllipseBounds(
    m_innerRadius / cos(m_thetaMinus),
    m_innerRadius,
    m_outerRadius / cos(m_thetaMinus),
    m_outerRadius,
    m_halfPhiSector);
}

Trk::EllipseBounds*
Trk::BevelledCylinderVolumeBounds::topEllipseBounds() const
{
  //    return new Trk::EllipseBounds();
  return new Trk::EllipseBounds(
    m_innerRadius / cos(m_thetaPlus),
    m_innerRadius,
    m_outerRadius / cos(m_thetaPlus),
    m_outerRadius,
    m_halfPhiSector);
}

Trk::CylinderBounds*
Trk::BevelledCylinderVolumeBounds::innerCylinderBounds() const
{
  return new Trk::CylinderBounds(m_innerRadius, m_halfPhiSector, m_halfZ);
}

Trk::CylinderBounds*
Trk::BevelledCylinderVolumeBounds::outerCylinderBounds() const
{
  return new Trk::CylinderBounds(m_outerRadius, m_halfPhiSector, m_halfZ);
}

Trk::DiscBounds*
Trk::BevelledCylinderVolumeBounds::discBounds() const
{
  // adjust radius to make sure all surface covered
  double outerRadius =
    (m_type > 1) ? m_outerRadius / cos(m_halfPhiSector) : m_outerRadius;
  return new Trk::DiscBounds(m_innerRadius, outerRadius, m_halfPhiSector);
}

Trk::TrapezoidBounds*
Trk::BevelledCylinderVolumeBounds::sectorTrdBounds() const
{
  return new Trk::TrapezoidBounds(
    0.5 * (outerRadius() - innerRadius()), m_halfZ, m_thetaMinus, m_thetaPlus);
}

Trk::RectangleBounds*
Trk::BevelledCylinderVolumeBounds::sectorPlaneBounds() const
{
  double ri = innerRadius();
  double ro = outerRadius();
  if (m_type == 1 || m_type == 3)
    ri *= 1. / cos(halfPhiSector());
  if (m_type > 1)
    ro *= 1. / cos(halfPhiSector());
  return new Trk::RectangleBounds(0.5 * (ro - ri), m_halfZ);
}

Trk::Volume*
Trk::BevelledCylinderVolumeBounds::subtractedVolume() const
{
  if (m_type < 1)
    return nullptr;

  double tp = tan(m_halfPhiSector);
  Trk::Volume* volIn = nullptr;
  Trk::Volume* volOut = nullptr;
  if (m_type == 1 || m_type == 3) { // cut inner cylinder
    volIn = new Trk::Volume(
      nullptr,
      new Trk::CuboidVolumeBounds(
        m_innerRadius, m_innerRadius * tp + 0.1, m_halfZ + 0.1));
  }
  if (m_type > 1) {
    double hz = m_outerRadius * (1. / cos(m_halfPhiSector) - 1.);
    volOut = new Trk::Volume(
      new Amg::Transform3D(
        Amg::Translation3D(Amg::Vector3D(m_outerRadius + hz, 0., 0.))),
      new Trk::CuboidVolumeBounds(hz, m_outerRadius * tp + 0.1, m_halfZ + 0.1));
  }

  if (!volIn)
    return volOut;
  else if (!volOut)
    return volIn;
  return new Trk::Volume(
    nullptr, new Trk::CombinedVolumeBounds(volIn, volOut, false));
}

// ostream operator overload

MsgStream&
Trk::BevelledCylinderVolumeBounds::dump(MsgStream& sl) const
{
  std::stringstream sl_temp;
  sl_temp << std::setiosflags(std::ios::fixed);
  sl_temp << std::setprecision(7);
  sl_temp << "Trk::BevelledCylinderVolumeBounds: (innerR, outerR, "
             "halfPhiSector, halflengthInZ, thetaMinus, thetaPlus) = ";
  sl_temp << "(" << m_innerRadius << ", " << m_outerRadius << ", "
          << m_halfPhiSector << ", " << m_halfZ << ", " << m_thetaMinus << ", "
          << m_thetaPlus << ")";
  sl << sl_temp.str();
  return sl;
}

std::ostream&
Trk::BevelledCylinderVolumeBounds::dump(std::ostream& sl) const
{
  std::stringstream sl_temp;
  sl_temp << std::setiosflags(std::ios::fixed);
  sl_temp << std::setprecision(7);
  sl_temp << "Trk::BevelledCylinderVolumeBounds: (innerR, outerR, "
             "halfPhiSector, halflengthInZ, thetaMinus, thetaPlus) = ";
  sl_temp << "(" << m_innerRadius << ", " << m_outerRadius << ", "
          << m_halfPhiSector << ", " << m_halfZ << m_thetaMinus << ", "
          << m_thetaPlus << ")";
  sl << sl_temp.str();
  return sl;
}

