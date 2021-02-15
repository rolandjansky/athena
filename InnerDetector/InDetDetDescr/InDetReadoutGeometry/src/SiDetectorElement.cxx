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
    SolidStateDetectorElementBase(id,design,geophysvol,commonItems,geoAlignStore),
    m_design(design)
  {
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

    Amg::Vector3D point = globalPosition(localPos);
    return sinTilt(point);
  }

  double
  SiDetectorElement::sinTilt(const Amg::Vector3D& globalPos) const
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

    Amg::Vector3D point=globalPosition(localPos);
    return sinStereoImpl(point);
  }

  double
  SiDetectorElement::sinStereo(const Amg::Vector3D& globalPos) const
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
  SiDetectorElement::sinStereoLocal(const Amg::Vector3D& globalPos) const
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
  

  bool
  SiDetectorElement::nearBondGap(const Amg::Vector2D& localPosition, double etaTol) const
  {
    //again, can we avoid casting here?
    return static_cast<const SiDetectorDesign *>(m_design)->nearBondGap(localPosition, etaTol);
  }

  bool
  SiDetectorElement::nearBondGap(const Amg::Vector3D& globalPosition, double etaTol) const
  {
    return static_cast<const SiDetectorDesign *>(m_design)->nearBondGap(localPosition(globalPosition), etaTol);
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

    //Since these are barrel, endcap, sensor-type, specific, might be better for these to be calculated in the design()
    //However, not clear how one could do that for the annulus calculation which uses global frame
    double sinStereo = 0.;
    if (isBarrel()) {
      sinStereo = m_phiAxis.z();
    } else { // endcap
      if (m_design->shape() == InDetDD::Annulus) { //built-in Stereo angle for Annulus shape sensor
	Amg::Vector3D sensorCenter = m_design->sensorCenter();
	//Below retrieved method will return -sin(m_Stereo), thus sinStereolocal = sin(m_Stereo)
	double sinStereoReco = - (m_design->sinStripAngleReco(sensorCenter[1], sensorCenter[0]));
	double cosStereoReco = sqrt(1-sinStereoReco*sinStereoReco); 
	double radialShift = sensorCenter[0]; 
	//The focus of all strips in the local reco frame
	Amg::Vector2D localfocus(-radialShift*sinStereoReco, radialShift - radialShift*cosStereoReco);
	//The focus of all strips in the global frame
	Amg::Vector3D globalfocus(globalPosition(localfocus));
	//The direction of x-axis of the Strip frame in the global frame
	Amg::Vector3D globalSFxAxis =(m_center - globalfocus)/radialShift;
	//Stereo angle is the angle between global radial direction and the x-axis of the Strip frame in the global frame 
	sinStereo = (m_center.y() * globalSFxAxis.x() - m_center.x() * globalSFxAxis.y()) / m_center.perp();   
      }
      else sinStereo = (m_center.y() * m_etaAxis.x() - m_center.x() * m_etaAxis.y()) / m_center.perp();
    }
    return sinStereo;
  }

  double
  SiDetectorElement::sinStereoImpl(const Amg::Vector3D& globalPos) const
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
	Amg::Vector3D stripAxis = radius * m_etaAxis + globalPos - m_center;
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
