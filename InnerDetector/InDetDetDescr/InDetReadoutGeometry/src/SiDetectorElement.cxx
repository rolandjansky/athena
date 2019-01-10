/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "InDetReadoutGeometry/SiCellId.h"
#include "InDetReadoutGeometry/SiReadoutCellId.h"


#include "InDetReadoutGeometry/SiCommonItems.h"

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
  TrkDetElementBase(geophysvol),
  m_id(id),
  m_design(design),
  m_commonItems(commonItems),
  m_nextInEta(0),
  m_prevInEta(0),
  m_nextInPhi(0),
  m_prevInPhi(0),
  m_otherSide(0),
  m_cacheValid(false),
  m_conditionsCacheValid(false),
  m_firstTime(true),
  m_isStereo(false),
  m_tanLorentzAnglePhi(0),
  m_tanLorentzAngleEta(0),
  m_lorentzCorrection(0),
  m_surface(0)
{
  //The following are fixes for coverity bug 11955, uninitialized scalars:
  const bool boolDefault(true);
  m_depthDirection=boolDefault;
  m_phiDirection=boolDefault;
  m_etaDirection=boolDefault;
  const double defaultMin(std::numeric_limits<double>::max());
  const double defaultMax(std::numeric_limits<double>::lowest());
  m_minZ=defaultMin;
  m_maxZ=defaultMax;
  m_minR=defaultMin;
  m_maxR=defaultMax;
  m_minPhi=defaultMin;
  m_maxPhi=defaultMax;

  m_hitEta = m_design->etaAxis();
  m_hitPhi = m_design->phiAxis();
  m_hitDepth = m_design->depthAxis();
  ///
  
  commonConstructor();
}

void
SiDetectorElement::commonConstructor() 
{
  if (!m_id.is_valid()) throw std::runtime_error("SiDetectorElement: Invalid identifier");

  // Set booleans for wether we are pixel/sct barrel/endcap
  m_isPixel = getIdHelper()->is_pixel(m_id);
  if (!m_isPixel && !getIdHelper()->is_sct(m_id)){
    if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element id is not for pixel or SCT" << endreq;
  }

  // Set IdHash. Also set m_isBarrel.
  m_isDBM=false; 
  if (isPixel()) {
    const PixelID* pixelId = dynamic_cast<const PixelID *>(getIdHelper());
    if(pixelId){
      m_isBarrel = pixelId->is_barrel(m_id);
      m_idHash = pixelId->wafer_hash(m_id);

      if(pixelId->is_dbm(m_id)){
	m_isBarrel=false; 
	m_isDBM=true; 
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

  // Increase the reference count of the SiDetectorDesign objects.
  m_design->ref();

  // Increase the reference count of the SiCommonItems objects.
  m_commonItems->ref();

  // Should we reference count the geophysvol as well?

}


// Destructor:
SiDetectorElement::~SiDetectorElement()
{
  delete m_surface;

  // The design is reference counted so that it will not be deleted until the last element is deleted.
  m_design->unref();

  m_commonItems->unref();

}

void 
SiDetectorElement::updateCache() const
{

  m_cacheValid = true;

  bool firstTimeTmp = m_firstTime;
  m_firstTime = false;
  
  const HepGeom::Transform3D &geoTransform = getMaterialGeom()->getAbsoluteTransform();
      
  m_centerCLHEP = geoTransform * m_design->sensorCenter();
  m_center = Amg::Vector3D(m_centerCLHEP[0],m_centerCLHEP[1],m_centerCLHEP[2]);
  
  HepGeom::Point3D<double> centerGeoModel(0., 0., 0.);
  m_originCLHEP = geoTransform * centerGeoModel;
  m_origin = Amg::Vector3D(m_originCLHEP[0],m_originCLHEP[1],m_originCLHEP[2]);
  //
  // Determine directions depth, eta and phi axis in reconstruction local frame
  // ie depth away from interaction point
  //    phi in direction of increasing phi
  //    eta in direction of increasing z in barrel, and increasing r in endcap
  //
    
  // depthAxis, phiAxis, and etaAxis are defined to be x,y,z respectively for all detectors for hit local frame.
  // depthAxis, phiAxis, and etaAxis are defined to be z,x,y respectively for all detectors for reco local frame.
  static const HepGeom::Vector3D<double> localAxes[3] = {
    HepGeom::Vector3D<double>(1,0,0),
    HepGeom::Vector3D<double>(0,1,0),
    HepGeom::Vector3D<double>(0,0,1)
  };

  
  static const HepGeom::Vector3D<double> & localRecoPhiAxis = localAxes[distPhi];     // Defined to be same as x axis
  static const HepGeom::Vector3D<double> & localRecoEtaAxis = localAxes[distEta];     // Defined to be same as y axis
  static const HepGeom::Vector3D<double> & localRecoDepthAxis = localAxes[distDepth]; // Defined to be same as z axis
  
  // We only need to calculate the rough orientation once.
  //For it to change would require extreme unrealistic misalignment changes.
  if (firstTimeTmp) {
    // Determine the unit vectors in global frame
   
    const HepGeom::Vector3D<double> &geoModelPhiAxis = localAxes[m_hitPhi];
    const HepGeom::Vector3D<double> &geoModelEtaAxis = localAxes[m_hitEta];
    const HepGeom::Vector3D<double> &geoModelDepthAxis = localAxes[m_hitDepth];
 
    HepGeom::Vector3D<double> globalDepthAxis(geoTransform * geoModelDepthAxis);
    HepGeom::Vector3D<double> globalPhiAxis(geoTransform * geoModelPhiAxis);
    HepGeom::Vector3D<double> globalEtaAxis(geoTransform * geoModelEtaAxis);

    // unit radial vector
    HepGeom::Vector3D<double> unitR(m_center.x(), m_center.y(), 0.);
    unitR.setMag(1.);

    HepGeom::Vector3D<double> nominalEta;
    HepGeom::Vector3D<double> nominalNormal;
    HepGeom::Vector3D<double> nominalPhi(-unitR.y(), unitR.x(), 0);
  
    // In Barrel like geometry, the etaAxis is along increasing z, and normal is in increasing radial direction.
    // In Endcap like geometry, the etaAxis is along increasing r, and normal is in decreasing z direction,
    // We base whether it is barrel like or endcap like by the orientation of the local z axis of the 
    // the element. This allows the use of endcap identifiers in a TB setup. A warning message is issued however if
    // the orientation and identifier are not consistent (ie a barrel like orientation with an endcap identifier).

    bool barrelLike = true;
    nominalEta.setZ(1);
    if (std::abs(globalEtaAxis.dot(nominalEta)) < 0.5) { // Check that it is in roughly the right direction.
      barrelLike = false;
    }   

    if (isBarrel() && !barrelLike) {      
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element has endcap like orientation with barrel identifier." 
						 << endreq;
    } else if (!isBarrel() && barrelLike) {
      if(msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Element has barrel like orientation with endcap identifier."
						 << endreq;
    }
    
    if (barrelLike) {
      nominalEta.setZ(1);
      nominalNormal =  unitR;
    } else { // endcap like
      nominalNormal.setZ(-1);
      nominalEta = unitR; 
    }

    // Determine if axes are to have there directions swapped.

    //
    // Depth axis.
    //
    double depthDir = globalDepthAxis.dot(nominalNormal);
    m_depthDirection = true;
    if (depthDir < 0) {
      if (m_design->depthSymmetric()) {
	m_depthDirection = false;
      } else {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local depth axis." << endreq;
      }
    }
    if (std::abs(depthDir) < 0.5) { // Check that it is in roughly the right direction.
      msg(MSG::ERROR) << "Orientation of local depth axis does not follow correct convention." << endreq;
      // throw std::runtime_error("Orientation of local depth axis does not follow correct convention.");
      m_depthDirection = true; // Don't swap.
    }
    
    //
    // Phi axis
    //
    double phiDir = globalPhiAxis.dot(nominalPhi);
    m_phiDirection = true;
    if (phiDir < 0) {
      if (m_design->phiSymmetric()) {
	m_phiDirection = false;
      } else {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local xPhi axis." << endreq;
      }
    }
    
    if (std::abs(phiDir) < 0.5) { // Check that it is in roughly the right direction.
      msg(MSG::ERROR) << "Orientation of local xPhi axis does not follow correct convention." << endreq;
      // throw std::runtime_error("Orientation of local xPhi axis does not follow correct convention.");
      m_phiDirection = true; // Don't swap.
    }
    
    //
    // Eta axis
    //
    double etaDir = globalEtaAxis.dot(nominalEta);
    m_etaDirection = true;
    if (etaDir < 0) {
      if (m_design->etaSymmetric()) {
	m_etaDirection = false;
      } else {
	if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Unable to swap local xEta axis." << endreq;
      }
    }
    if (std::abs(etaDir) < 0.5) { // Check that it is in roughly the right direction.
      msg(MSG::ERROR) << "Orientation of local xEta axis does not follow correct convention." << endreq;
      // throw std::runtime_error("Orientation of local xEta axis does not follow correct convention.");
      m_etaDirection = true; // Don't swap
    }   

  } // end if (m_firstTime)
  

  m_transformHit   = geoTransform * m_design->SiHitToGeoModel();
  m_transformCLHEP = geoTransform * recoToHitTransform();
  //m_transform = m_commonItems->solenoidFrame() * geoTransform * recoToHitTransform();
  m_transform = Amg::CLHEPTransformToEigen(m_transformCLHEP);

  #ifndef NDEBUG
  // Check that local frame is right-handed. (ie transform has no reflection)
  // This can be done by checking that the determinant is >0.
  if (firstTimeTmp) { // Only need to check this once.
    HepGeom::Transform3D & t = m_transformCLHEP;
    double det = t(0,0) * (t(1,1)*t(2,2) - t(1,2)*t(2,1)) -
                 t(0,1) * (t(1,0)*t(2,2) - t(1,2)*t(2,0)) +
                 t(0,2) * (t(1,0)*t(2,1) - t(1,1)*t(2,0));
    if (det < 0) {
      if(msgLvl(MSG::DEBUG)){ 
        msg(MSG::DEBUG) << "Local frame is left-handed. (hitEtaDirection, hitPhiDirection, hitDepthDirection) = ("
                        << m_etaDirection <<", "
                        << m_phiDirection <<", "
                        << m_depthDirection <<")"
                        << endreq;
      }
    }
  }
  #endif 

  // Initialize various cached members
  // The unit vectors
  m_normalCLHEP = m_transformCLHEP * localRecoDepthAxis;
  m_normal = Amg::Vector3D( m_normalCLHEP[0], m_normalCLHEP[1], m_normalCLHEP[2]);
  

  m_phiAxisCLHEP = m_transformCLHEP * localRecoPhiAxis;
  m_etaAxisCLHEP = m_transformCLHEP * localRecoEtaAxis;

  m_phiAxis = Amg::Vector3D(m_phiAxisCLHEP[0],m_phiAxisCLHEP[1],m_phiAxisCLHEP[2]);
  m_etaAxis = Amg::Vector3D(m_etaAxisCLHEP[0],m_etaAxisCLHEP[1],m_etaAxisCLHEP[2]);

  getExtent(m_minR, m_maxR, m_minZ, m_maxZ, m_minPhi, m_maxPhi);

  // Determin isStereo
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


void 
SiDetectorElement::updateConditionsCache() const
{
  m_conditionsCacheValid = true;

  // 
  // Lorentz Angle related stuff
  // 

  if (m_commonItems->lorentzAngleSvc()) {
    m_tanLorentzAnglePhi = m_commonItems->lorentzAngleSvc()->getTanLorentzAngle(m_idHash);
    m_tanLorentzAngleEta = m_commonItems->lorentzAngleSvc()->getTanLorentzAngleEta(m_idHash);
    m_lorentzCorrection = m_commonItems->lorentzAngleSvc()->getLorentzShift(m_idHash);
  } else {
    // Set to zero
    m_tanLorentzAnglePhi = 0;
    m_tanLorentzAngleEta = 0;
    m_lorentzCorrection =  0;
  }
}


const HepGeom::Transform3D &
SiDetectorElement::transformHit() const
{
  if (!m_cacheValid) {
      updateCache();
  }
  return m_transformHit;
}

const Amg::Transform3D &
SiDetectorElement::transform() const
{
  if (!m_cacheValid) updateCache();
  return m_transform;
}

const HepGeom::Transform3D &
SiDetectorElement::transformCLHEP() const
{
  //if (!m_cacheValid) updateCache();
  //return m_transform;
  //stuff to get the CLHEP version of the local to global transform.
  if (!m_cacheValid) updateCache();
  return m_transformCLHEP;

}
const HepGeom::Transform3D 
SiDetectorElement::defTransformCLHEP() const
{
  return getMaterialGeom()->getDefAbsoluteTransform() * recoToHitTransform();
}  
   
const Amg::Transform3D 
SiDetectorElement::defTransform() const
{
  HepGeom::Transform3D tmpTransform =  getMaterialGeom()->getDefAbsoluteTransform() * recoToHitTransform();
  return Amg::CLHEPTransformToEigen(tmpTransform);
}

const HepGeom::Transform3D 
SiDetectorElement::recoToHitTransform() const
{

  // Determine the reconstruction local (LocalPosition) to global transform.

  if (m_firstTime) updateCache();

  // global = transform * recoLocal
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
    HepGeom::Vector3D<double>(1,0,0),
    HepGeom::Vector3D<double>(0,1,0),
    HepGeom::Vector3D<double>(0,0,1)
  };
  //static 

  int signPhi = m_phiDirection? +1: -1;
  int signEta = m_etaDirection? +1: -1;

  const HepGeom::Transform3D recoToHit(HepGeom::Point3D<double>(0, 0, 0),
  signPhi * localAxes[distPhi], signEta * localAxes[distEta],
  HepGeom::Point3D<double>(0, 0, 0), localAxes[m_hitPhi], localAxes[m_hitEta]);

  return recoToHit;
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


const Amg::Vector3D & 
SiDetectorElement::center() const
{
  if (!m_cacheValid) updateCache();
  return m_center;
}

const Amg::Vector3D & 
SiDetectorElement::origin() const
{
  if (!m_cacheValid) updateCache();
  return m_origin;
}

const Amg::Vector3D & 
SiDetectorElement::normal() const
{
  if (!m_cacheValid) updateCache();
  return m_normal;
}

const HepGeom::Vector3D<double> & 
SiDetectorElement::etaAxisCLHEP() const
{
  if (!m_cacheValid) updateCache();
  return m_etaAxisCLHEP;
}

const HepGeom::Vector3D<double> & 
SiDetectorElement::phiAxisCLHEP() const
{
  if (!m_cacheValid) updateCache();
  return m_phiAxisCLHEP;
}

const Amg::Vector3D & 
SiDetectorElement::etaAxis() const
{
  if (!m_cacheValid) updateCache();
  return m_etaAxis;
}

const Amg::Vector3D & 
SiDetectorElement::phiAxis() const
{
  if (!m_cacheValid) updateCache();
  return m_phiAxis;
}

Amg::Vector2D
SiDetectorElement::hitLocalToLocal(double xEta, double xPhi) const  // Will change order to phi,eta
{
  if (!m_cacheValid) updateCache(); 
  if (!m_etaDirection) xEta = -xEta;
  if (!m_phiDirection) xPhi = -xPhi;
  return Amg::Vector2D(xPhi, xEta);
}

HepGeom::Point3D<double>
SiDetectorElement::hitLocalToLocal3D(const HepGeom::Point3D<double> & hitPosition) const
{
  // Equiv to transform().inverse * transformHit() * hitPosition
  if (!m_cacheValid) updateCache(); 
  double xDepth = hitPosition[m_hitDepth];
  double xPhi = hitPosition[m_hitPhi];
  double xEta = hitPosition[m_hitEta];
  if (!m_depthDirection) xDepth = -xDepth;
  if (!m_phiDirection) xPhi = -xPhi;
  if (!m_etaDirection) xEta = -xEta;
  return HepGeom::Point3D<double>(xPhi, xEta, xDepth);
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

bool SiDetectorElement::isBlayer() const
{
  if (isPixel() && isBarrel()) {
    const PixelID* p_pixelId = static_cast<const PixelID *>(getIdHelper());
    return (0==p_pixelId->layer_disk(m_id));
  } else {
    return false;
  }
} 

bool SiDetectorElement::isInnermostPixelLayer() const
{
  if (isPixel() && isBarrel()) {
    const PixelID* p_pixelId = static_cast<const PixelID *>(getIdHelper());
    return (0==p_pixelId->layer_disk(m_id));
  } else {
    return false;
  }
}  

bool SiDetectorElement::isNextToInnermostPixelLayer() const
{
  if (isPixel() && isBarrel()) {
    const PixelID* p_pixelId = static_cast<const PixelID *>(getIdHelper());
    return (1==p_pixelId->layer_disk(m_id));
  } else {
    return false;
  }
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
  if (isPixel() && isBarrel()) {
    double myNormalZ = this->normal()[Amg::z];
    // Inclined barrel modules (in the ITk Step 2.2 Inclined Duals layout)
    // have myNormalZ values of -0.965926 or -0.829044.
    // Flat barrel modules have myNormalZ = 0
    // in a perfectly-aligned detector
    // but once misalignment is added, perhaps the value of 0 below should be changed
    // to 0.1 or whatever is a reasonable value.
    double epsilon = 0.1;
   
    return(fabs(myNormalZ) > epsilon);
  }
  else {
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

Amg::Vector2D SiDetectorElement::correctLocalPosition(const Amg::Vector2D &position) const
{
  Amg::Vector2D correctedPosition(position);
  correctedPosition[distPhi] += getLorentzCorrection();
  return correctedPosition;
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
      double sinStereoReco = - m_design->sinStripAngleReco(sensorCenter[1], sensorCenter[0]);
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
  return m_design->sinStripAngleReco(localPos[0], localPos[1]);
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
    m_design->endsOfStrip(position);
  return std::pair<Amg::Vector3D,Amg::Vector3D >(globalPosition(localEnds.first),
					  globalPosition(localEnds.second));
}


const Trk::Surface & 
SiDetectorElement::surface() const
{
  if (!m_surface){
    m_surface = new Trk::PlaneSurface(*this);
  }
 return *m_surface;
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
  
const Trk::SurfaceBounds & 
SiDetectorElement::bounds() const
{
  return m_design->bounds();
}
  
// Get min/max or r, z,and phi
void SiDetectorElement::getExtent(double &rMin, double &rMax, 
	       double &zMin, double &zMax, 
	       double &phiMin, double &phiMax) const
{
  HepGeom::Point3D<double> corners[4];
  getCorners(corners);

  bool first = true; 

  double phiOffset = 0.;

  HepGeom::Point3D<double> sensorCenter = m_design->sensorCenter();
  double radialShift = sensorCenter[0];

  const HepGeom::Transform3D rShift = HepGeom::TranslateX3D(radialShift);//in local frame, radius is x

  for (int i = 0; i < 4; i++) {
    corners[i].transform(rShift);
    HepGeom::Point3D<double> globalPoint = globalPosition(corners[i]);

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
      } else if  (phiPoint > 0.5 * M_PI) {
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
  if (phiMin > M_PI)  phiMin -= 2. * M_PI; 
  if (phiMax < -M_PI) phiMax += 2. * M_PI; 
  if (phiMax > M_PI)  phiMax -= 2. * M_PI; 

}



// Get eta/phi extent. Returns min/max eta and phi and r (for barrel)
// or z (for endcap) Takes as input the vertex spread in z (+-deltaZ).
// Gets 4 corners of the sensor and calculates eta phi for each corner
// for both +/- vertex spread.  The returned phi is between -M_PI and M_PI
// with the direction minPhi to maxPhi always in the positive sense,
// so if the element extends across the -180/180 boundary then minPhi will
// be greater than maxPhi.
void SiDetectorElement::getEtaPhiRegion(double deltaZ, double &etaMin, double &etaMax, double &phiMin, 
					double &phiMax, double &rz) const
{
  if (!m_cacheValid) updateCache();

  HepGeom::Point3D<double> corners[4];
  getCorners(corners);

  bool first = true; 

  double phiOffset = 0.;

  for (int i = 0; i < 4; i++) {
    double etaMinPoint;
    double etaMaxPoint;
    double phiPoint;

    // Get the eta phi value for this corner.
    getEtaPhiPoint(corners[i], deltaZ, etaMinPoint, etaMaxPoint, phiPoint);

    if (first) { // Use the first point to initialize the min/max values.

      // Put phi in a range so that we are not near -180/+180 division.
      // Do this by adding an offset if phi > 90 CLHEP::deg or < -90 CLHEP::deg. 
      // This offset is later removed.
      if (phiPoint < -0.5 * M_PI) {
	phiOffset = -0.5 * M_PI;
      } else if  (phiPoint > 0.5 * M_PI) {
	phiOffset = 0.5 * M_PI;
      }
      phiMin = phiMax = phiPoint - phiOffset;
      etaMin = etaMinPoint;
      etaMax = etaMaxPoint;
    } else {
      phiPoint -= phiOffset;
      // put phi back in -M_PI < phi < +M_PI range
      if (phiPoint < -M_PI) phiPoint += 2. * M_PI; 
      if (phiPoint > M_PI)  phiPoint -= 2. * M_PI; 
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
  if (phiMin > M_PI)  phiMin -= 2. * M_PI; 
  if (phiMax < -M_PI) phiMax += 2. * M_PI; 
  if (phiMax > M_PI)  phiMax -= 2. * M_PI; 

  // Calculate rz = r (barrel) or z (endcap)
  // Use center of sensor ((0,0,0) in local coordinates) for determining this.
  //  HepGeom::Point3D<double> globalCenter = globalPosition(HepGeom::Point3D<double>(0,0,0));
  if (isBarrel()) {
    rz = center().perp(); // r
  } else {
    rz = center().z();  // z
  }

}	

// Gets eta phi for a point given in local coordinates. deltaZ is specified to
// account for the vertex spread. phi is independent of this vertex
// spread. etaMax will correspond to zMin (-deltaZ) and etaMin will
// correspond to zMax (+deltaZ).
void SiDetectorElement::getEtaPhiPoint(const HepGeom::Point3D<double> & point, double deltaZ, 
				       double &etaMin, double &etaMax, double &phi) const
{
  // Get the point in global coordinates.
  HepGeom::Point3D<double> globalPoint = globalPosition(point);

  double r = globalPoint.perp();
  double z = globalPoint.z();
  
  double thetaMin = atan2(r,(z + deltaZ));
  etaMax = -log(tan(0.5 * thetaMin));
  double thetaMax = atan2(r,(z - deltaZ));
  etaMin = -log(tan(0.5 * thetaMax));
  
  phi = globalPoint.phi();
}

void SiDetectorElement::getCorners(HepGeom::Point3D<double> *corners) const
{
  if (!m_cacheValid) updateCache();

  // This makes the assumption that the forward SCT detectors are orientated such that 
  // the positive etaAxis corresponds to the top of the detector where the width is largest.
  // This is currently always the case.
  // For the SCT barrel and pixel detectors minWidth and maxWidth are the same and so should 
  // work for all orientations.

  double minWidth = m_design->minWidth();
  double maxWidth = m_design->maxWidth();
  double length   = m_design->length();
  
  // Lower left
  corners[0][distPhi] = -0.5 * minWidth;
  corners[0][distEta] = -0.5 * length;
  corners[0][distDepth] = 0.;

  // Lower right
  corners[1][distPhi] =  0.5 * minWidth;
  corners[1][distEta] = -0.5 * length;
  corners[1][distDepth] = 0.;

  // Upper Right
  corners[2][distPhi] = 0.5 * maxWidth;
  corners[2][distEta] = 0.5 * length;
  corners[2][distDepth] = 0.;

  // Upper left
  corners[3][distPhi] = -0.5 * maxWidth;
  corners[3][distEta] =  0.5 * length;
  corners[3][distDepth] = 0.;
}

SiIntersect
SiDetectorElement::inDetector(const Amg::Vector2D & localPosition, 
					  double phiTol, double etaTol) const
{
  return m_design->inDetector(localPosition, phiTol, etaTol);
}

  
SiIntersect 
SiDetectorElement::inDetector(const HepGeom::Point3D<double> & globalPosition, double phiTol, double etaTol) const
{
  return m_design->inDetector(localPosition(globalPosition), phiTol, etaTol);
}

bool 
SiDetectorElement::nearBondGap(Amg::Vector2D localPosition, double etaTol) const
{
  return m_design->nearBondGap(localPosition, etaTol);
}

bool
SiDetectorElement::nearBondGap(HepGeom::Point3D<double> globalPosition, double etaTol) const
{
  return m_design->nearBondGap(localPosition(globalPosition), etaTol);
}  


  Amg::Vector2D
SiDetectorElement::localPositionOfCell(const SiCellId &cellId) const
{
  Amg::Vector2D pos(m_design->localPositionOfCell(cellId));
  return correctLocalPosition(pos);
}

Amg::Vector2D
SiDetectorElement::localPositionOfCell(const Identifier & id) const
{
  SiCellId cellId = cellIdFromIdentifier(id);
  SiLocalPosition silp = m_design->localPositionOfCell(cellId);
  Amg::Vector2D pos = hitLocalToLocal(silp.xEta(), silp.xPhi());
  return correctLocalPosition(pos);
}

Amg::Vector2D
SiDetectorElement::rawLocalPositionOfCell(const SiCellId &cellId) const
{
  return m_design->localPositionOfCell(cellId);
}

Amg::Vector2D
SiDetectorElement::rawLocalPositionOfCell(const Identifier & id) const
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


SiCellId 
SiDetectorElement::cellIdOfPosition(const Amg::Vector2D &localPosition) const
{
  return m_design->cellIdOfPosition(localPosition);
}

Identifier
SiDetectorElement::identifierOfPosition(const Amg::Vector2D &localPosition) const
{
  SiCellId cellId = m_design->cellIdOfPosition(localPosition);
  return identifierFromCellId(cellId);
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
          auto &sctDesign = *m_design; 
          int strip1D = sctDesign.strip1Dim(strip, row);
          cellId = SiCellId(strip1D);
        }
      }
    }
  }
  
  return cellId;
}

} // namespace InDetDD
