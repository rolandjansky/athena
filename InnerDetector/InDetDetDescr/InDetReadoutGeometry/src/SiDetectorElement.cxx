/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.cxx
 * Implementation file for class SiDetectorElement
 * @author Grant Gorfine
 * Based on version developed by David Calvet.
 **/

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "AtlasDetDescr/AtlasDetectorID.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <cassert>
#include <cmath>

namespace InDetDD {
  using Trk::distPhi;
  using Trk::distEta;
  using Trk::distDepth;

  // Constructor with parameters:
  SiDetectorElement::SiDetectorElement(const Identifier& id,
                                       const SiDetectorDesign* design,
                                       const GeoVFullPhysVol* geophysvol,
                                       const SiCommonItems* commonItems,
                                       const GeoAlignmentStore* geoAlignStore) :
    SolidStateDetectorElementBase(id,design,geophysvol,commonItems,geoAlignStore) {
    commonConstructor();
  }

  // Destructor:
  SiDetectorElement::~SiDetectorElement()
  {
  }

  bool
  SiDetectorElement::isPixel() const
  {
    return m_isPixel;
  }

  bool
  SiDetectorElement::isSCT() const
  {
    return !m_isPixel;
  }

  bool
  SiDetectorElement::isDBM() const
  {
    return m_isDBM;
  }

  bool
  SiDetectorElement::isBarrel() const
  {
    return m_isBarrel;
  }

  bool
  SiDetectorElement::isBlayer() const
  {
    if (isPixel() && isBarrel()) {
      const PixelID* p_pixelId = dynamic_cast<const PixelID*>(getIdHelper());
      return (p_pixelId && 0==p_pixelId->layer_disk(m_id));
    } else {
      return false;
    }
  }

  bool
  SiDetectorElement::isInnermostPixelLayer() const
  {
    if (isPixel() && isBarrel()) {
      const PixelID* p_pixelId = dynamic_cast<const PixelID*>(getIdHelper());
      return (p_pixelId && 0==p_pixelId->layer_disk(m_id));
    } else {
      return false;
    }
  }

  bool
  SiDetectorElement::isNextToInnermostPixelLayer() const
  {
    if (isPixel() && isBarrel()) {
      const PixelID* p_pixelId = dynamic_cast<const PixelID*>(getIdHelper());
      return (p_pixelId && 1==p_pixelId->layer_disk(m_id));
    } else {
      return false;
    }
  }

  Identifier
  SiDetectorElement::identifierFromCellId(const SiCellId& cellId) const
  {
    Identifier id; // Will be initialized in an invalid state.

    // If something fails it returns the id in an invalid state.

    if (cellId.isValid()) {

      if (isPixel()) {
        const PixelID* pixelIdHelper = dynamic_cast<const PixelID*>(getIdHelper());
        if (pixelIdHelper) {
          id = pixelIdHelper->pixel_id(m_id, cellId.phiIndex(), cellId.etaIndex());
        }
      } else {
        const SCT_ID* sctIdHelper = dynamic_cast<const SCT_ID*>(getIdHelper());
        if (sctIdHelper) {
          id = sctIdHelper->strip_id(m_id, cellId.strip());
        }
      }
    }

    return id;
  }

  SiCellId
  SiDetectorElement::cellIdFromIdentifier(const Identifier& identifier) const
  {
    SiCellId cellId; // Initialized in invalid state.

    // If something fails it returns the cellId in an invalid state.

    if (identifier.is_valid()) {

      if (isPixel()) {
        const PixelID* pixelIdHelper = dynamic_cast<const PixelID*>(getIdHelper());
        if (pixelIdHelper) {
          cellId = SiCellId(pixelIdHelper->phi_index(identifier), pixelIdHelper->eta_index(identifier));
        }
      } else {
        const SCT_ID* sctIdHelper = dynamic_cast<const SCT_ID*>(getIdHelper());
        if (sctIdHelper) {
          cellId =  SiCellId(sctIdHelper->strip(identifier));
        }
      }
    }

    return cellId;
  }

  Trk::Surface&
  SiDetectorElement::surface()
  {
    return *m_surface;
  }

  const Trk::Surface&
  SiDetectorElement::surface() const
  {
    return *m_surface;
  }

  const std::vector<const Trk::Surface*>&
  SiDetectorElement::surfaces() const
  {
    if (!m_surfacesValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_surfacesValid) {
        // get this surface
        m_surfaces.push_back(&surface());
        // get the other side surface
        if (otherSide()) {
          m_surfaces.push_back(&(otherSide()->surface()));
        }
      }
      m_surfacesValid.store(true);
    }

    // return the surfaces
    return m_surfaces;
  }

  const GeoTrf::Transform3D&
  SiDetectorElement::transformHit() const
  {
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getAbsPosition(getMaterialGeom());
      if (ptrXf) return *ptrXf;
    }
    return getMaterialGeom()->getAbsoluteTransform();
  }

  const HepGeom::Transform3D&
  SiDetectorElement::transformCLHEP() const
  {
    //stuff to get the CLHEP version of the local to global transform.
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_transformCLHEP;
  }

  const Amg::Transform3D&
  SiDetectorElement::transform() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_transform;
  }

  const HepGeom::Transform3D
  SiDetectorElement::defTransformCLHEP() const
  {
    if (m_geoAlignStore) {
      const GeoTrf::Transform3D* ptrXf = m_geoAlignStore->getDefAbsPosition(getMaterialGeom());
      if (ptrXf) return Amg::EigenTransformToCLHEP(*ptrXf) * recoToHitTransform();
    }
    return Amg::EigenTransformToCLHEP(getMaterialGeom()->getDefAbsoluteTransform()) * recoToHitTransform();
  }  
   
  const Amg::Transform3D
  SiDetectorElement::defTransform() const
  {
    HepGeom::Transform3D tmpTransform = defTransformCLHEP();
    return Amg::CLHEPTransformToEigen(tmpTransform);
  }

  const Amg::Transform3D&
  SiDetectorElement::moduleTransform() const
  {
    return (isModuleFrame()) ? transform() : m_otherSide->transform();
  }

  Amg::Transform3D
  SiDetectorElement::defModuleTransform() const
  {
    return (isModuleFrame()) ? defTransform() : m_otherSide->defTransform();
  }

  // Take a transform in the local reconstruction and return it in the module frame
  // For a given transform l in frame A. The equivalent transform in frame B is
  //  B.inverse() * A * l * A.inverse() * B
  // Here A is the local to global transform of the element and B is the local to global
  // transform of the module.
  // If we are already in the module frame then there is nothing to do, we just return the
  // transform that is input. Otherwise we use the above formula.
  Amg::Transform3D
  SiDetectorElement::localToModuleFrame(const Amg::Transform3D& localTransform) const
  {
    if (isModuleFrame()) {
      return localTransform;
    } else {
      return m_otherSide->transform().inverse() * transform() * localTransform * transform().inverse() * m_otherSide->transform();
    }
  }

  Amg::Transform3D
  SiDetectorElement::localToModuleTransform() const
  {
    if (isModuleFrame()) {
      return Amg::Transform3D(); // Identity
    } else {
      return m_otherSide->transform().inverse() * transform();
    }
  }

  bool
  SiDetectorElement::isModuleFrame() const
  {
    // The module frame is the axial side.
    // NB isStereo returns false for the pixel and so
    // isModuleFrame is always true for the pixel.

    return !isStereo();
  }

  const HepGeom::Vector3D<double>&
  SiDetectorElement::phiAxisCLHEP() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_phiAxisCLHEP;
  }

  const Amg::Vector3D&
  SiDetectorElement::phiAxis() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_phiAxis;
  }

  const HepGeom::Vector3D<double>&
  SiDetectorElement::etaAxisCLHEP() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_etaAxisCLHEP;
  }

  const Amg::Vector3D&
  SiDetectorElement::etaAxis() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_etaAxis;
  }

  const Amg::Vector3D&
  SiDetectorElement::normal() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_normal;
  }

  const Amg::Vector3D&
  SiDetectorElement::center() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return m_center;
  }

  Amg::Vector2D
  SiDetectorElement::hitLocalToLocal(double xEta, double xPhi) const  // Will change order to phi, eta
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    if (!m_etaDirection) xEta = -xEta;
    if (!m_phiDirection) xPhi = -xPhi;
    return Amg::Vector2D(xPhi, xEta);
  }

  HepGeom::Point3D<double>
  SiDetectorElement::hitLocalToLocal3D(const HepGeom::Point3D<double>& hitPosition) const
  {
    // Equiv to transform().inverse * transformHit() * hitPosition
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    double xDepth = hitPosition[m_hitDepth];
    double xPhi = hitPosition[m_hitPhi];
    double xEta = hitPosition[m_hitEta];
    if (!m_depthDirection) xDepth = -xDepth;
    if (!m_phiDirection) xPhi = -xPhi;
    if (!m_etaDirection) xEta = -xEta;
    return HepGeom::Point3D<double>(xPhi, xEta, xDepth);
  }

  // compute sin(tilt angle) at center:
  double
  SiDetectorElement::sinTilt() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    // Tilt is defined as the angle between a refVector and the sensor normal.
    // In barrel refVector = unit vector radial.
    // in endcap it is assumed there is no tilt.
    // sinTilt = (refVector cross normal) . z

    // tilt angle is not defined for the endcap
    if (isEndcap()) return 0.;

    // Angle between normal and radial vector.
    // HepGeom::Vector3D<double> refVector(m_center.x(), m_center.y(), 0);
    // return (refVector.cross(m_normal)).z()/refVector.mag();
    // or the equivalent
    return (m_center.x() * m_normal.y() - m_center.y() * m_normal.x()) / m_center.perp();
  }

  double
  SiDetectorElement::sinTilt(const Amg::Vector2D& localPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    // tilt angle is not defined for the endcap
    if (isEndcap()) return 0.;

    HepGeom::Point3D<double> point = globalPositionCLHEP(localPos);
    return sinTilt(point);
  }

  double
  SiDetectorElement::sinTilt(const HepGeom::Point3D<double>& globalPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    // It is assumed that the global position is already in the plane of the element.

    // tilt angle is not defined for the endcap
    if (isEndcap()) return 0.;

    // Angle between normal and radial vector.
    //HepGeom::Vector3D<double> refVector(globalPos.x(), globalPos.y(), 0);
    //return (refVector.cross(m_normal)).z()/refVector.mag();
    // or the equivalent
    return (globalPos.x() * m_normal.y() - globalPos.y() * m_normal.x()) / globalPos.perp();
  }

  double
  SiDetectorElement::sinStereo() const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return sinStereoImpl();
  }

  double
  SiDetectorElement::sinStereo(const Amg::Vector2D& localPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    HepGeom::Point3D<double> point=globalPositionCLHEP(localPos);
    return sinStereoImpl(point);
  }

  double
  SiDetectorElement::sinStereo(const HepGeom::Point3D<double>& globalPos) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    return sinStereoImpl(globalPos);
  } 

  double
  SiDetectorElement::sinStereoLocal(const Amg::Vector2D& localPos) const
  {
    // The equation below will work for rectangle detectors as well in which 
    // case it will return 0. But we return zero immediately as there is no point doing the calculation.
    if (m_design->shape() == InDetDD::Box) return 0.;
    double oneOverRadius = (maxWidth() - minWidth()) / (width() * length());
    double x = localPos[distPhi];
    double y = localPos[distEta];
    return -x*oneOverRadius / sqrt( (1.+y*oneOverRadius)*(1.+y*oneOverRadius) + x*oneOverRadius*x*oneOverRadius );
  }

  double
  SiDetectorElement::sinStereoLocal(const HepGeom::Point3D<double>& globalPos) const
  {
    return sinStereoLocal(localPosition(globalPos));
  }

  bool
  SiDetectorElement::isStereo() const
  {
    if (!m_stereoCacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_stereoCacheValid) {
        determineStereo();
      }
    }

    return m_isStereo;
  }

  // Get eta/phi extent. Returns min/max eta and phi and r (for barrel)
  // or z (for endcap) Takes as input the vertex spread in z (+-deltaZ).
  // Gets 4 corners of the sensor and calculates eta phi for each corner
  // for both +/- vertex spread.  The returned phi is between -M_PI and M_PI
  // with the direction minPhi to maxPhi always in the positive sense,
  // so if the element extends across the -180/180 boundary then minPhi will
  // be greater than maxPhi.
  void
  SiDetectorElement::getEtaPhiRegion(double deltaZ, double& etaMin, double& etaMax, double& phiMin,
                                     double& phiMax, double& rz) const
  {
    if (!m_cacheValid) {
      std::lock_guard<std::mutex> lock(m_mutex);
      if (!m_cacheValid) updateCache();
    }

    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true;

    double phiOffset = 0.;

    for (int i = 0; i < 4; i++) {
      double etaMinPoint = 0.;
      double etaMaxPoint = 0.;
      double phiPoint = 0.;

      // Get the eta phi value for this corner.
      getEtaPhiPoint(corners[i], deltaZ, etaMinPoint, etaMaxPoint, phiPoint);

      if (first) { // Use the first point to initialize the min/max values.

        // Put phi in a range so that we are not near -180/+180 division.
        // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
        // This offset is later removed.
        if (phiPoint < -0.5 * M_PI) {
          phiOffset = -0.5 * M_PI;
        } else if (phiPoint > 0.5 * M_PI) {
          phiOffset = 0.5 * M_PI;
        }
        phiMin = phiMax = phiPoint - phiOffset;
        etaMin = etaMinPoint;
        etaMax = etaMaxPoint;
      } else {
        phiPoint -= phiOffset;
        // put phi back in -M_PI < phi < +M_PI range
        if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
        if (phiPoint >  M_PI) phiPoint -= 2. * M_PI;
        phiMin = std::min(phiMin, phiPoint);
        phiMax = std::max(phiMax, phiPoint);
        etaMin = std::min(etaMin, etaMinPoint);
        etaMax = std::max(etaMax, etaMaxPoint);
      }
      first = false;
    }

    // put phi back in -M_PI < phi < +M_PI range
    phiMin += phiOffset;
    phiMax += phiOffset;
    if (phiMin < -M_PI) phiMin += 2. * M_PI;
    if (phiMin >  M_PI) phiMin -= 2. * M_PI;
    if (phiMax < -M_PI) phiMax += 2. * M_PI;
    if (phiMax >  M_PI) phiMax -= 2. * M_PI;

    rz = get_rz();

    }


  double SiDetectorElement::get_rz() const
  {
    // Calculate rz = r (barrel) or z (endcap)
    // Use center of sensor ((0,0,0) in local coordinates) for determining this.
    //  HepGeom::Point3D<double> globalCenter = globalPosition(HepGeom::Point3D<double>(0,0,0));
    if (isBarrel()) {
      return center().perp(); // r
    } else {
      return center().z();  // z
    }
  }
  
  const Trk::SurfaceBounds&
  SiDetectorElement::bounds() const
  {
    return m_design->bounds();
  }

  bool
  SiDetectorElement::nearBondGap(const Amg::Vector2D& localPosition, double etaTol) const
  {
    //again, can we avoid casting here?
    return static_cast<const SiDetectorDesign *>(m_design)->nearBondGap(localPosition, etaTol);
  }

  bool
  SiDetectorElement::nearBondGap(const HepGeom::Point3D<double>& globalPosition, double etaTol) const
  {
    return static_cast<const SiDetectorDesign *>(m_design)->nearBondGap(localPosition(globalPosition), etaTol);
  }

  SiIntersect
  SiDetectorElement::inDetector(const Amg::Vector2D& localPosition,
                                double phiTol, double etaTol) const
  {
    return m_design->inDetector(localPosition, phiTol, etaTol);
  }

  SiIntersect
  SiDetectorElement::inDetector(const HepGeom::Point3D<double>& globalPosition, double phiTol, double etaTol) const
  {
    return m_design->inDetector(localPosition(globalPosition), phiTol, etaTol);
  }

  Identifier
  SiDetectorElement::identifierOfPosition(const Amg::Vector2D& localPosition) const
  {
    SiCellId cellId = m_design->cellIdOfPosition(localPosition);
    return identifierFromCellId(cellId);
  }

  SiCellId
  SiDetectorElement::cellIdOfPosition(const Amg::Vector2D& localPosition) const
  {
    return m_design->cellIdOfPosition(localPosition);
  }

  Amg::Vector2D
  SiDetectorElement::rawLocalPositionOfCell(const SiCellId& cellId) const
  {
    return m_design->localPositionOfCell(cellId);
  }

  Amg::Vector2D
  SiDetectorElement::rawLocalPositionOfCell(const Identifier& id) const
  {
    SiCellId cellId = cellIdFromIdentifier(id);
    return m_design->localPositionOfCell(cellId);
  }

  int
  SiDetectorElement::numberOfConnectedCells(const SiCellId cellId) const
  {
    SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
    return m_design->numberOfConnectedCells(readoutId);
  }

  SiCellId
  SiDetectorElement::connectedCell(const SiCellId cellId, int number) const
  {
    SiReadoutCellId readoutId = m_design->readoutIdOfCell(cellId);
    return m_design->connectedCell(readoutId, number);
  }

  // Special method for SCT to retrieve the two ends of a "strip"
  std::pair<Amg::Vector3D,Amg::Vector3D>
  SiDetectorElement::endsOfStrip(const Amg::Vector2D& position) const
  {
    //again with the casting...
    const std::pair<Amg::Vector2D,Amg::Vector2D> localEnds=
      static_cast<const SiDetectorDesign *>(m_design)->endsOfStrip(position);
    return std::pair<Amg::Vector3D,Amg::Vector3D >(globalPosition(localEnds.first),
                                                   globalPosition(localEnds.second));
  }

  void
  SiDetectorElement::commonConstructor()
  {
    if (!m_id.is_valid()) throw std::runtime_error("SiDetectorElement: Invalid identifier");

    // Set booleans for wether we are pixel/sct barrel/endcap
    m_isPixel = getIdHelper()->is_pixel(m_id);
    if (!m_isPixel && !getIdHelper()->is_sct(m_id)) {
      ATH_MSG_WARNING("Element id is not for pixel or SCT");
    }

    // Set m_idHash. Also set m_isBarrel.
    if (isPixel()) {
      const PixelID* pixelId = dynamic_cast<const PixelID*>(getIdHelper());
      if (pixelId) {
        m_isBarrel = pixelId->is_barrel(m_id);
        m_idHash = pixelId->wafer_hash(m_id);

        if (pixelId->is_dbm(m_id)) {
          m_isBarrel = false;
          m_isDBM = true;
        }
      }
    } else {
      const SCT_ID* sctId = dynamic_cast<const SCT_ID*>(getIdHelper());
      if (sctId) {
        m_isBarrel = sctId->is_barrel(m_id);
        m_idHash = sctId->wafer_hash(m_id);
      }
    }

    if (!m_idHash.is_valid()) throw std::runtime_error("SiDetectorElement: Unable to set IdentifierHash");

    // Set surface
    m_surface = std::make_unique<Trk::PlaneSurface>(*this);
  }

  // update cache
  // This is supposed to be called inside a block like
  //
  // if (!m_cacheValid) {
  //   std::lock_guard<std::mutex> lock(m_mutex);
  //   if (!m_cacheValid) updateCache();
  // }
  //
  void
  SiDetectorElement::updateCache() const
  {


    SolidStateDetectorElementBase::updateCache();

    //Similar to 21.9, but ... Do we actually need this? If not, we could just rely on the base-class implementation?

      if (isBarrel() && !m_barrelLike) {
        ATH_MSG_WARNING("Element has endcap like orientation with barrel identifier.");
      } else if (!isBarrel() && m_barrelLike) {
        ATH_MSG_WARNING("Element has barrel like orientation with endcap identifier.");
      }

    m_cacheValid.store(true);
    if (m_firstTime) m_firstTime.store(false);
  }

  void
  SiDetectorElement::determineStereo() const
  {
    // Assume m_mutex is already locked.

    if (m_firstTime) updateCache();

    if (isSCT() && m_otherSide) {
      double sinStereoThis = std::abs(sinStereoImpl()); // Call the private impl method
      double sinStereoOther = std::abs(m_otherSide->sinStereo());
      if (sinStereoThis == sinStereoOther) {
        // If they happend to be equal then set side0 as axial and side1 as stereo.
        const SCT_ID* sctId = dynamic_cast<const SCT_ID*>(getIdHelper());
        if (sctId) {
          int side = sctId->side(m_id);
          m_isStereo = (side == 1);
        }
      } else {
        // set the stereo side as the one with largest absolute sinStereo.
        m_isStereo = (sinStereoThis > sinStereoOther);
      }
    } else {
      m_isStereo = false;
    }

    m_stereoCacheValid.store(true);
  }

  // Get min/max or r, z,and phi
  // helper method only to be used for the cache construction
  // i.e inside updateCache
  void
  SiDetectorElement::getExtent(double& rMin, double& rMax,
                               double& zMin, double& zMax,
                               double& phiMin, double& phiMax) const
  {

     double radialShift = 0.;
     /*
    Deprecated method for specialized ITk DetElement with different global frame - to be replaced!
    const InDetDD::StripStereoAnnulusDesign* testDesign = dynamic_cast<const InDetDD::StripStereoAnnulusDesign*>(m_design);
    if (testDesign) radialShift = testDesign->localModuleCentreRadius();//additional radial shift for sensors centred on beamline
     */

    HepGeom::Point3D<double> corners[4];
    getCorners(corners);

    bool first = true;

    double phiOffset = 0.;

   
    const HepGeom::Transform3D rShift = HepGeom::TranslateX3D(radialShift);//in local frame, radius is x

    for (int i = 0; i < 4; i++) {

      //if (testDesign) corners[i].transform(rShift); see comment re ITk...

      // m_tranform is already there as  part of the cache construction
      // This method seems to be used only as a helper for updateCache
      HepGeom::Point3D<double> globalPoint = m_transformCLHEP * corners[i];

      double rPoint = globalPoint.perp();
      double zPoint = globalPoint.z();
      double phiPoint = globalPoint.phi();

      // Use first point to initializa min/max values.
      if (first) {

        // Put phi in a range so that we are not near -180/+180 division.
        // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
        // This offset is later removed.
        if (phiPoint < -0.5 * M_PI) {
          phiOffset = -0.5 * M_PI;
        } else if (phiPoint > 0.5 * M_PI) {
          phiOffset = 0.5 * M_PI;
        }
        phiMin = phiMax = phiPoint - phiOffset;
        rMin = rMax = rPoint;
        zMin = zMax = zPoint;

      } else {
        phiPoint -= phiOffset;
        // put phi back in -M_PI < phi < +M_PI range
        if (phiPoint < -M_PI) phiPoint += 2. * M_PI;
        if (phiPoint > M_PI)  phiPoint -= 2. * M_PI;
        phiMin = std::min(phiMin, phiPoint);
        phiMax = std::max(phiMax, phiPoint);
        rMin = std::min(rMin, rPoint);
        rMax = std::max(rMax, rPoint);
        zMin = std::min(zMin, zPoint);
        zMax = std::max(zMax, zPoint);
      }
      first = false;
    }

    // put phi back in -M_PI < phi < +M_PI range
    phiMin += phiOffset;
    phiMax += phiOffset;
    if (phiMin < -M_PI) phiMin += 2. * M_PI;
    if (phiMin >  M_PI) phiMin -= 2. * M_PI;
    if (phiMax < -M_PI) phiMax += 2. * M_PI;
    if (phiMax >  M_PI) phiMax -= 2. * M_PI;

  }

  void
  SiDetectorElement::getCorners(HepGeom::Point3D<double>* corners) const
  {
    // This makes the assumption that the forward SCT detectors are orientated such that 
    // the positive etaAxis corresponds to the top of the detector where the width is largest.
    // This is currently always the case.
    // For the SCT barrel and pixel detectors minWidth and maxWidth are the same and so should 
    // work for all orientations.

    double tmpMinWidth = minWidth();
    double tmpMaxWidth = maxWidth();
    double tmpLength   = length();
  
    // Lower left
    corners[0][distPhi] = -0.5 * tmpMinWidth;
    corners[0][distEta] = -0.5 * tmpLength;
    corners[0][distDepth] = 0.;

    // Lower right
    corners[1][distPhi] =  0.5 * tmpMinWidth;
    corners[1][distEta] = -0.5 * tmpLength;
    corners[1][distDepth] = 0.;

    // Upper Right
    corners[2][distPhi] = 0.5 * tmpMaxWidth;
    corners[2][distEta] = 0.5 * tmpLength;
    corners[2][distDepth] = 0.;

    // Upper left
    corners[3][distPhi] = -0.5 * tmpMaxWidth;
    corners[3][distEta] =  0.5 * tmpLength;
    corners[3][distDepth] = 0.;
  }

  // Gets eta phi for a point given in local coordinates. deltaZ is specified to
  // account for the vertex spread. phi is independent of this vertex
  // spread. etaMax will correspond to zMin (-deltaZ) and etaMin will
  // correspond to zMax (+deltaZ).
  void
  SiDetectorElement::getEtaPhiPoint(const HepGeom::Point3D<double>& point, double deltaZ,
                                    double& etaMin, double& etaMax, double& phi) const
  {
    // Get the point in global coordinates.
    HepGeom::Point3D<double> globalPoint = globalPosition(point);

    double r = globalPoint.perp();
    double z = globalPoint.z();
  
    double thetaMin = std::atan2(r,(z + deltaZ));
    etaMax = -std::log(tan(0.5 * thetaMin));
    double thetaMax = std::atan2(r,(z - deltaZ));
    etaMin = -std::log(tan(0.5 * thetaMax));

    phi = globalPoint.phi();
  }
  /*
  const HepGeom::Transform3D
  SiDetectorElement::recoToHitTransformImpl() const
  {
    //        = transfromHit * hitLocal
    //        = transformHit * recoToHitTransform * recoLocal
    // recoToHitTransform takes recoLocal to hitLocal
    // x,y,z -> y,z,x
    // equiv to a rotation around Y of 90 deg followed by a rotation around X of 90deg
    //
    // recoToHit is static as it needs to be calculated once only.
    // We use the HepGeom::Transform3D constructor which takes one coordinates system to another where the
    // coordinate system is defined by it center and two axes.
    // distPhi, distEta are the reco local axes and hitPhi and hitEta are the hit local axes.
    // It assume phi, eta, depth makes a right handed system which is the case.
    static const HepGeom::Vector3D<double> localAxes[3] = {
      HepGeom::Vector3D<double>(1., 0., 0.),
      HepGeom::Vector3D<double>(0., 1., 0.),
      HepGeom::Vector3D<double>(0., 0., 1.)
    };

    const HepGeom::Transform3D recoToHit(HepGeom::Point3D<double>(0., 0., 0.),
                                         localAxes[distPhi],
                                         localAxes[distEta],
                                         HepGeom::Point3D<double>(0., 0., 0.),
                                         localAxes[m_hitPhi],
                                         localAxes[m_hitEta]);

    // Swap direction of axis as appropriate
    CLHEP::Hep3Vector scale(1., 1., 1.);
    if (!m_phiDirection)   scale[distPhi]   = -1.;
    if (!m_etaDirection)   scale[distEta]   = -1.;
    if (!m_depthDirection) scale[distDepth] = -1.;
    return recoToHit * HepGeom::Scale3D(scale[0], scale[1], scale[2]);
  }
  */
  double
  SiDetectorElement::sinStereoImpl() const
  {
    // Stereo is the angle between a refVector and a vector along the strip/pixel in eta direction.
    // I'm not sure how the sign should be defined. I've defined it here
    // with rotation sense respect to normal,
    // where normal is away from IP in barrel and in -ve z direction in endcap

    // In Barrel refVector = unit vector along z axis,
    // in endcap refVector = unit vector radial.
    //
    // sinStereo = (refVector cross stripAxis) . normal
    //           = (refVector cross etaAxis) . normal
    //           = refVector . (etaAxis cross normal)
    //           = refVector . phiAxis
    //
    // in Barrel we use
    // sinStereo = refVector . phiAxis
    //           = phiAxis.z()
    //
    // in endcap we use
    // sinStereo = (refVector cross etaAxis) . normal
    //           = -(center cross etaAxis) . zAxis
    //           = (etaAxis cross center). z()
    double sinStereo = 0.;
    if (isBarrel()) {
      sinStereo = m_phiAxis.z();
    } else { // endcap
      sinStereo = (m_center.y() * m_etaAxis.x() - m_center.x() * m_etaAxis.y()) / m_center.perp();
    }
    return sinStereo;
  }

  double
  SiDetectorElement::sinStereoImpl(const HepGeom::Point3D<double>& globalPos) const
  {
    //
    // sinStereo =  (refVector cross stripAxis) . normal
    //
    double sinStereo = 0.;
    if (isBarrel()) {
      if (m_design->shape() != InDetDD::Trapezoid) {
        sinStereo = m_phiAxis.z();
      } else { // trapezoid
        assert (minWidth() != maxWidth());
        double radius = width() * length() / (maxWidth() - minWidth());
        HepGeom::Vector3D<double> stripAxis = radius * m_etaAxisCLHEP + globalPos - m_centerCLHEP;
        sinStereo = (stripAxis.x() * m_normal.y() - stripAxis.y() * m_normal.x()) / stripAxis.mag();
      }
    } else { // endcap
      if (m_design->shape() != InDetDD::Trapezoid) {
        sinStereo = (globalPos.y() * m_etaAxis.x() - globalPos.x() * m_etaAxis.y()) / globalPos.perp();
      } else { // trapezoid
        assert (minWidth() != maxWidth());
        double radius = width() * length() / (maxWidth() - minWidth());
        // Only need projection in xy plane.
        double stripAxisX  = globalPos.x() - m_center.x() + m_etaAxis.x()*radius;
        double stripAxisY  = globalPos.y() - m_center.y() + m_etaAxis.y()*radius;
        double norm = 1./(radius*sqrt(stripAxisX*stripAxisX + stripAxisY*stripAxisY));
        sinStereo = norm * (stripAxisX * globalPos.y() - stripAxisY * globalPos.x());
      }
    }
    return sinStereo;
  }



} // namespace InDetDD
