/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
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
    m_siDesign(design)
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
	  //This adds some extra code for supporting rows, 
	  //but this method is only used in validation-type code
	  //So should not add an overhead in normal running
	  //(although we perhaps still try to avoid this...)
	  int strip = sctIdHelper->strip(identifier);
	  int row = sctIdHelper->row(identifier);
	  if(row>0){
	    auto &sctDesign = *static_cast<const SiDetectorDesign *>(m_siDesign);
	    int strip1D = sctDesign.strip1Dim(strip, row);
	    cellId = SiCellId(strip1D);
	  }
	  else cellId =  SiCellId(strip);
	}
      }
    }

    return cellId;
  }

  const std::vector<const Trk::Surface*>&
  SiDetectorElement::surfaces() const
  {
    if (!m_surfaces.isValid()) {
      std::vector<const Trk::Surface*> s;
      s.push_back(&surface());
      if (otherSide()) {
        s.push_back(&(otherSide()->surface()));
      }
      m_surfaces.set (std::move (s));
    }

    // return the surfaces
    return *m_surfaces.ptr();
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
      return {}; // Identity
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
    const Amg::Vector3D& normal = this->normal();
    const Amg::Vector3D& center = this->center();
    return (center.x() * normal.y() - center.y() * normal.x()) / center.perp();
  }

  double
  SiDetectorElement::sinTilt(const Amg::Vector2D& localPos) const
  {
    // tilt angle is not defined for the endcap
    if (isEndcap()) return 0.;

    Amg::Vector3D point = globalPosition(localPos);
    return sinTilt(point);
  }

  double
  SiDetectorElement::sinTilt(const Amg::Vector3D& globalPos) const
  {
    // It is assumed that the global position is already in the plane of the element.

    // tilt angle is not defined for the endcap
    if (isEndcap()) return 0.;

    // Angle between normal and radial vector.
    //HepGeom::Vector3D<double> refVector(globalPos.x(), globalPos.y(), 0);
    //return (refVector.cross(m_normal)).z()/refVector.mag();
    // or the equivalent
    const Amg::Vector3D& normal = this->normal();
    return (globalPos.x() * normal.y() - globalPos.y() * normal.x()) / globalPos.perp();
  }

  double
  SiDetectorElement::sinStereo() const
  {
    return sinStereoImpl();
  }

  double
  SiDetectorElement::sinStereo(const Amg::Vector2D& localPos) const
  {
    Amg::Vector3D point=globalPosition(localPos);
    return sinStereoImpl(point);
  }

  double
  SiDetectorElement::sinStereo(const Amg::Vector3D& globalPos) const
  {
    return sinStereoImpl(globalPos);
  } 

  double
  SiDetectorElement::sinStereoLocal(const Amg::Vector2D& localPos) const
  {
    // The equation below will work for rectangle detectors as well in which 
    // case it will return 0. But we return zero immediately as there is no point doing the calculation.
    if (m_siDesign->shape() == InDetDD::Box) return 0.;
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
    if (!m_isStereo.isValid()) {
      m_isStereo.set (determineStereo());
    }

    return *m_isStereo.ptr();
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
    return static_cast<const SiDetectorDesign *>(m_siDesign)->nearBondGap(localPosition, etaTol);
  }

  bool
  SiDetectorElement::nearBondGap(const Amg::Vector3D& globalPosition, double etaTol) const
  {
    return static_cast<const SiDetectorDesign *>(m_siDesign)->nearBondGap(localPosition(globalPosition), etaTol);
  }



  // Special method for SCT to retrieve the two ends of a "strip"
  std::pair<Amg::Vector3D,Amg::Vector3D>
  SiDetectorElement::endsOfStrip(const Amg::Vector2D& position) const
  {
    //again with the casting...
    const std::pair<Amg::Vector2D,Amg::Vector2D> localEnds=
      static_cast<const SiDetectorDesign *>(m_siDesign)->endsOfStrip(position);
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
  // if (!m_cache.isValid()) updateCache();
  //
  void
  SiDetectorElement::updateCache() const
  {


    SolidStateDetectorElementBase::updateCache();

    //Similar to 21.9, but ... Do we actually need this? If not, we could just rely on the base-class implementation?
    if (isBarrel() && !m_axisDir.ptr()->m_barrelLike) {
      ATH_MSG_WARNING("Element has endcap like orientation with barrel identifier.");
    } else if (!isBarrel() && m_axisDir.ptr()->m_barrelLike && (m_siDesign->type())!=InDetDD::PixelInclined && (m_siDesign->type())!=InDetDD::PLR) {
      ATH_MSG_WARNING("Element has barrel like orientation with endcap identifier.");
    }
  }

  bool
  SiDetectorElement::determineStereo() const
  {
    if (isSCT() && m_otherSide) {
      double sinStereoThis = std::abs(sinStereoImpl()); // Call the private impl method
      double sinStereoOther = std::abs(m_otherSide->sinStereo());
      if (sinStereoThis == sinStereoOther) {
        // If they happen to be equal then set side0 as axial and side1 as stereo.
        const SCT_ID* sctId = dynamic_cast<const SCT_ID*>(getIdHelper());
        if (sctId) {
          int side = sctId->side(m_id);
         return (side == 1);
        }
      } else {
        // set the stereo side as the one with largest absolute sinStereo.
        return (sinStereoThis > sinStereoOther);
      }
    }

    return false;
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
    auto designShape = m_siDesign->shape();
    if (isBarrel()) {
      sinStereo = this->phiAxis().z();
    } else { // endcap
      if (designShape == InDetDD::Annulus) { //built-in Stereo angle for Annulus shape sensor
        Amg::Vector3D sensorCenter = m_siDesign->sensorCenter();
        //Below retrieved method will return -sin(m_Stereo), thus sinStereolocal = sin(m_Stereo)
        double sinStereoReco = - (m_siDesign->sinStripAngleReco(sensorCenter[1], sensorCenter[0]));
        double cosStereoReco = sqrt(1-sinStereoReco*sinStereoReco); 
        double radialShift = sensorCenter[0]; 
        //The focus of all strips in the local reco frame
        Amg::Vector2D localfocus(-radialShift*sinStereoReco, radialShift - radialShift*cosStereoReco);
        //The focus of all strips in the global frame
        Amg::Vector3D globalfocus(globalPosition(localfocus));
        //The direction of x-axis of the Strip frame in the global frame
              const Amg::Vector3D& center = this->center();
        Amg::Vector3D globalSFxAxis =(center - globalfocus)/radialShift;
        //Stereo angle is the angle between global radial direction and the x-axis of the Strip frame in the global frame 
        sinStereo = (center.y() * globalSFxAxis.x() - center.x() * globalSFxAxis.y()) / center.perp();   
      }
      // else if (designShape == InDetDD::PolarAnnulus) {} // Polar specialisation in future
      else { // barrel
        const Amg::Vector3D& etaAxis = this->etaAxis();
        const Amg::Vector3D& center = this->center();
        sinStereo = (center.y() * etaAxis.x() - center.x() * etaAxis.y()) / center.perp();
      }
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
      if (m_siDesign->shape() != InDetDD::Trapezoid) {
        sinStereo = this->phiAxis().z();
      } else { // trapezoid
        assert (minWidth() != maxWidth());
        double radius = width() * length() / (maxWidth() - minWidth());
        const Amg::Vector3D& etaAxis = this->etaAxis();
        const Amg::Vector3D& center = this->center();
        const Amg::Vector3D& normal = this->normal();
	Amg::Vector3D stripAxis = radius * etaAxis + globalPos - center;
        sinStereo = (stripAxis.x() * normal.y() - stripAxis.y() * normal.x()) / stripAxis.mag();
      }
    } else { // endcap
      if (m_siDesign->shape() != InDetDD::Trapezoid) {
        const Amg::Vector3D& etaAxis = this->etaAxis();
        sinStereo = (globalPos.y() * etaAxis.x() - globalPos.x() * etaAxis.y()) / globalPos.perp();
      } else { // trapezoid
        assert (minWidth() != maxWidth());
        const Amg::Vector3D& etaAxis = this->etaAxis();
        const Amg::Vector3D& center = this->center();
        double radius = width() * length() / (maxWidth() - minWidth());
        // Only need projection in xy plane.
        double stripAxisX  = globalPos.x() - center.x() + etaAxis.x()*radius;
        double stripAxisY  = globalPos.y() - center.y() + etaAxis.y()*radius;
        double norm = 1./(radius*sqrt(stripAxisX*stripAxisX + stripAxisY*stripAxisY));
        sinStereo = norm * (stripAxisX * globalPos.y() - stripAxisY * globalPos.x());
      }
    }
    return sinStereo;
  }



} // namespace InDetDD
