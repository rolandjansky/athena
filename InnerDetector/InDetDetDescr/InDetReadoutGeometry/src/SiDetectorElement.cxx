/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SiDetectorElement.cxx
 * Implementation file for class SiDetectorElement
 * @author Grant Gorfine
 * Based on version developed by David Calvet.
**/

#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "GeoModelKernel/GeoVFullPhysVol.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Units/PhysicalConstants.h" // for M_PI
#include "CLHEP/Units/SystemOfUnits.h" 

#include "ReadoutGeometryBase/SiCellId.h"
#include "ReadoutGeometryBase/SiReadoutCellId.h"


#include "ReadoutGeometryBase/SiCommonItems.h"

#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"

#include "InDetCondServices/ISiLorentzAngleSvc.h"
#include <cmath>
#include <cassert>
#include <limits>

namespace InDetDD {
using Trk::distPhi;
using Trk::distEta;
using Trk::distDepth;

// Constructor with parameters:
SiDetectorElement::SiDetectorElement(const Identifier &id,
				     const SiDetectorDesign *design,
				     const GeoVFullPhysVol *geophysvol,
				     SiCommonItems * commonItems) :
  SolidStateDetectorElementBase(id, design, geophysvol, commonItems),
  m_nextInEta(0),
  m_prevInEta(0),
  m_nextInPhi(0),
  m_prevInPhi(0),
  m_otherSide(0),
  m_cacheValid(false),
  m_firstTime(true),
  m_isStereo(false)
{
  // Set booleans for wether we are pixel/sct barrel/endcap
  m_isPixel = getIdHelper()->is_pixel(m_id);
  if (!m_isPixel && !getIdHelper()->is_sct(m_id)){
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element id is not for pixel or SCT" << endreq;
  }

  // Set IdHash. Also set m_isBarrel.
  m_isDBM=false; 
  m_isInnermostPixel=false;
  m_isNextToInnermostPixel=false;
  if (isPixel()) {
    const PixelID* pixelId = dynamic_cast<const PixelID *>(getIdHelper());
    if(pixelId) {
      m_isBarrel = pixelId->is_barrel(m_id);
      m_idHash = pixelId->wafer_hash(m_id);
      
      m_isInnermostPixel = pixelId->is_innermost(m_id);
      if (not m_isInnermostPixel)
        m_isNextToInnermostPixel = pixelId->is_nexttoinnermost(m_id);

      if(pixelId->is_dbm(m_id)){
        m_isBarrel=false; 
        m_isDBM=true; 
        m_isInnermostPixel=false;
        m_isNextToInnermostPixel=false;
      }       
    }    
  } else {
    const SCT_ID* sctId = dynamic_cast<const SCT_ID *>(getIdHelper()); 
    if(sctId){
      m_isBarrel = sctId->is_barrel(m_id);
      m_idHash = sctId->wafer_hash(m_id);
    }
  }
  
  if (!m_idHash.is_valid()) throw std::runtime_error("SiDetectorElement: Unable to set IdentifierHash");
}

// Destructor:
SiDetectorElement::~SiDetectorElement()
{
  delete m_surface;
}

void 
SiDetectorElement::updateCache() const
{
  SolidStateDetectorElementBase::updateCache();

  // handle warning about inconsistent orientation and identifier
  // m_barrelLike determined during first encounter of base class updateCache
  if(msgLvl(MSG::DEBUG)) {
    if (isBarrel() && !m_barrelLike) {
      msg(MSG::DEBUG) << "Element has endcap like orientation with barrel identifier. This is OK for Inclined modules or Barrel Disks in ITK pixel layouts, but is otherwise suspicious..." 
      << endreq;
    } else if (!isBarrel() && m_barrelLike) {
      msg(MSG::DEBUG) << "Element has barrel like orientation with endcap identifier."
      << endreq;
    }
  }

  m_cacheValid = true;

  bool firstTimeTmp = m_firstTime;
  m_firstTime = false;
  
  // Determine isStereo
  if (firstTimeTmp) {
    if (isSCT() && m_otherSide) {
      double sinStereoThis = std::abs(sinStereo());
      double sinStereoOther = std::abs(m_otherSide->sinStereo());
      if (std::fabs(sinStereoThis- sinStereoOther)<1.e-6) {
        // If they happend to be equal then set side0 as axial and side1 as stereo.
        const SCT_ID* sctId = dynamic_cast<const SCT_ID *>(getIdHelper()); 
        if(sctId){
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
  }
}

const Amg::Transform3D &
SiDetectorElement::moduleTransform() const
{
  return  (isModuleFrame()) ?  transform() : m_otherSide->transform();
}

  Amg::Transform3D
  SiDetectorElement::defModuleTransform() const
{
 return  (isModuleFrame()) ? defTransform() : m_otherSide->defTransform();
}  


// Take a transform in the local reconstruction and return it in the module frame
// For a given transform l in frame A. The equivalent transform in frame B is
//  B.inverse() * A * l * A.inverse() * B
// Here A is the local to global transform of the element and B is the local to global
// transform of the module.
// If we are already in the module frame then there is nothing to do, we just return the
// transform that is input. Otherwise we use the above formula.
Amg::Transform3D 
SiDetectorElement::localToModuleFrame(const Amg::Transform3D & localTransform) const
{
  if (isModuleFrame()) {
    return localTransform;
  } else {
    return m_otherSide->transform().inverse() * transform() * localTransform * transform().inverse() *  m_otherSide->transform();
  }
}

Amg::Transform3D 
SiDetectorElement::localToModuleTransform() const
{
  if (isModuleFrame()) {
    return Amg::Transform3D(); // Identity
  } else {
    return  m_otherSide->transform().inverse() * transform();
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












bool SiDetectorElement::isPixel() const
{
  return m_isPixel;
}

bool SiDetectorElement::isSCT() const
{
  return !m_isPixel;
}

bool SiDetectorElement::isBarrel() const
{
  return m_isBarrel;
}

bool SiDetectorElement::isDBM() const 
{ 
  return m_isDBM; 
} 

bool SiDetectorElement::isInnermostPixelLayer() const
{
  return m_isInnermostPixel;
}  

bool SiDetectorElement::isNextToInnermostPixelLayer() const
{
  return m_isNextToInnermostPixel;
}  

int SiDetectorElement::getPixelLayer() const {
  if( isPixel() ){
    const PixelID *p_pixelId = static_cast<const PixelID *>(getIdHelper());
    return p_pixelId->layer_disk(m_id);
  }else{
    return -1;
  }
}

bool SiDetectorElement::isInclined() const {

  if (isPixel()) {
    if (!m_cacheValid) updateCache();
    // Used for ITk Layout
    // allows to distinguish flat from inclined modules
    // In a perfect aligned detector
    // - The z component of the normal vector to the surface of flat barrel modules is 0 and of endcap rings is 1
    // - For inclined modules, it is >0 and <1
    // Since inclined modules have an inclination > 45 degrees wrt the beam line, 
    // it should be safe to ask for the z component of the normal vector 
    // to be bigger than cos(45) = sqrt(2)/2 = , considering that alignment 
    // constants wouldn't apply a big change

    // Temporary implementation, should be made a property of the design class in future geotags

    double myNormalZ = this->normal()[Amg::z];
    double exp_value_min = 0.7071; // sqrt(2)/2
    double exp_value_max = 0.99;

    return(fabs(myNormalZ) > exp_value_min && fabs(myNormalZ) < exp_value_max);
    
  } else {
    return false;
  }
}

bool SiDetectorElement::isBarrelRing() const {
  if (isPixel() && isBarrel()) {
    double myNormalZ = this->normal()[Amg::z];
    // Step 3 layouts contains also barrel rings. To identify we check if they are 
    // close to the perpendicular position
    double epsilon = 0.01;
   
    return((1.-fabs(myNormalZ)) < epsilon);
  }
  else {
    return false;
  }
}

// compute sin(tilt angle) at center:
double SiDetectorElement::sinTilt() const
{
  if (!m_cacheValid) updateCache();

  // Tilt is defined as the angle between a refVector and the sensor normal.
  // In barrel refVector = unit vector radial.
  // in endcap it is assumed there is no tilt.
  // sinTilt = (refVector cross normal) . z

  // tilt angle is not defined for the endcap
  if (isEndcap()) return 0;
  
  // Angle between normal and radial vector.
  // HepGeom::Vector3D<double> refVector(m_center.x(), m_center.y(), 0);
  // return (refVector.cross(m_normal)).z()/refVector.mag();
  // or the equivalent
  return (m_center.x() * m_normal.y() - m_center.y() * m_normal.x()) / m_center.perp();
}

  double SiDetectorElement::sinTilt(const Amg::Vector2D &localPos) const
{
  if (!m_cacheValid) updateCache();

  // tilt angle is not defined for the endcap
  if (isEndcap()) return 0;

  HepGeom::Point3D<double> point=globalPositionCLHEP(localPos);
  return sinTilt(point);
}

double SiDetectorElement::sinTilt(const HepGeom::Point3D<double> &globalPos) const
{ 
  if (!m_cacheValid) updateCache();

  // It is assumed that the global position is already in the plane of the element.

  // tilt angle is not defined for the endcap
  if (isEndcap()) return 0;
 
  // Angle between normal and radial vector.
  //HepGeom::Vector3D<double> refVector(globalPos.x(), globalPos.y(), 0);
  //return (refVector.cross(m_normal)).z()/refVector.mag();
  // or the equivalent
  return (globalPos.x() * m_normal.y() - globalPos.y() * m_normal.x()) / globalPos.perp();
}

double SiDetectorElement::sinStereo() const
{

  if (!m_cacheValid) updateCache();

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

  double sinStereo;
  if (isBarrel()) {
    sinStereo = m_phiAxis.z();
  } else { // endcap
    if (m_design->shape() == InDetDD::Annulus) { //built-in Stereo angle for Annulus shape sensor
      HepGeom::Point3D<double> sensorCenter = m_design->sensorCenter();
      //Below retrieved method will return -sin(m_Stereo), thus sinStereolocal = sin(m_Stereo)
      double sinStereoReco = - static_cast<const SiDetectorDesign *>(m_design)->sinStripAngleReco(sensorCenter[1], sensorCenter[0]);
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
    else{ //non built-in Stereo angle
      sinStereo = (m_center.y() * m_etaAxis.x() - m_center.x() * m_etaAxis.y()) / m_center.perp();
    }
  }      
  return sinStereo;
}

  double SiDetectorElement::sinStereo(const Amg::Vector2D &localPos) const
{
  if (!m_cacheValid) updateCache();

  HepGeom::Point3D<double> point=globalPositionCLHEP(localPos);
  return sinStereo(point);
}

double SiDetectorElement::sinStereo(const HepGeom::Point3D<double> &globalPos) const
{
  //
  // sinStereo =  (refVector cross stripAxis) . normal
  //

  if (!m_cacheValid) updateCache();

  double sinStereo;
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
      // double radius   = m_center.perp(); // Really want nominal r.
      assert (minWidth() != maxWidth());
      double radius = width() * length() / (maxWidth() - minWidth());
      // HepGeom::Vector3D<double> stripAxis = radius * m_etaAxis + globalPos - m_center;
      // Only need projection in xy plane.
      double stripAxisX  = globalPos.x() - m_center.x() + m_etaAxis.x()*radius;
      double stripAxisY  = globalPos.y() - m_center.y() + m_etaAxis.y()*radius;
      double norm  = 1./(radius*sqrt(stripAxisX*stripAxisX + stripAxisY*stripAxisY));
      sinStereo  = norm * (stripAxisX * globalPos.y() - stripAxisY * globalPos.x());
    }
  }
  return sinStereo;
}

bool 
SiDetectorElement::isStereo() const 
{
  if (m_firstTime) updateCache();
  return m_isStereo;
}


double 
SiDetectorElement::sinStereoLocal(const Amg::Vector2D &localPos) const
{
  return static_cast<const SiDetectorDesign *>(m_design)->sinStripAngleReco(localPos[0], localPos[1]);
}


double 
SiDetectorElement::sinStereoLocal(const HepGeom::Point3D<double> &globalPos) const
{
  return sinStereoLocal(localPosition(globalPos));
}

// Special method for SCT to retrieve the two ends of a "strip"
  std::pair<Amg::Vector3D,Amg::Vector3D> SiDetectorElement::endsOfStrip(const Amg::Vector2D &position) const
{
  const std::pair<Amg::Vector2D,Amg::Vector2D> localEnds=
    static_cast<const SiDetectorDesign *>(m_design)->endsOfStrip(position);
  return std::pair<Amg::Vector3D,Amg::Vector3D >(globalPosition(localEnds.first),
					  globalPosition(localEnds.second));
}

  
const std::vector<const Trk::Surface*>& SiDetectorElement::surfaces() const 
{
    if (!m_surfaces.size()){
        // get this surface
        m_surfaces.push_back(&surface());
        // get the other side surface
        if (otherSide()){
            m_surfaces.push_back(&(otherSide()->surface()));
        }
    }
    // return the surfaces
    return m_surfaces;
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
SiDetectorElement::nearBondGap(Amg::Vector2D localPosition, double etaTol) const
{
  return static_cast<const SiDetectorDesign *>(m_design)->nearBondGap(localPosition, etaTol);
}

bool
SiDetectorElement::nearBondGap(HepGeom::Point3D<double> globalPosition, double etaTol) const
{
  return static_cast<const SiDetectorDesign *>(m_design)->nearBondGap(localPosition(globalPosition), etaTol);
}

Identifier 
SiDetectorElement::identifierFromCellId(const SiCellId & cellId) const
{
  Identifier id; // Will be initialized in an invalid state.

  // If something fails it returns the id in an invalid state.

  if (cellId.isValid()) {

    if (isPixel()){
      const PixelID * pixelIdHelper = static_cast<const PixelID *>(getIdHelper());
      if (pixelIdHelper) {
	id = pixelIdHelper->pixel_id(m_id, cellId.phiIndex(), cellId.etaIndex());
      }
    } else {
      const SCT_ID * sctIdHelper = static_cast<const SCT_ID *>(getIdHelper());
      if (sctIdHelper) {
	id = sctIdHelper->strip_id(m_id, cellId.strip());
      }
    }
  }

  return id; 
}

SiCellId   
SiDetectorElement::cellIdFromIdentifier(const Identifier & identifier) const
{
  SiCellId cellId; // Initialized in invalid state.

  // If something fails it returns the cellId in an invalid state.

  if (identifier.is_valid()) {
  
    if (isPixel()){
      const PixelID * pixelIdHelper = static_cast<const PixelID *>(getIdHelper());
      if (pixelIdHelper) {
	cellId = SiCellId(pixelIdHelper->phi_index(identifier), pixelIdHelper->eta_index(identifier));
      }
    } else {
      const SCT_ID * sctIdHelper = static_cast<const SCT_ID *>(getIdHelper());
      if (sctIdHelper) {
        int row = sctIdHelper->row(identifier);
        int strip = sctIdHelper->strip(identifier);
        if (row < 0) { // This sensor does not have rows
          cellId = SiCellId(strip);
        }
        else {
          auto &sctDesign = *static_cast<const SiDetectorDesign *>(m_design);
          int strip1D = sctDesign.strip1Dim(strip, row);
          cellId = SiCellId(strip1D);
        }
      }
    }
  }
  
  return cellId;
}

} // namespace InDetDD
