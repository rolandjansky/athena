/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SiLorentzAngleSvc/SiLorentzAngleSvc.h"

#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/IdentifierHash.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "StoreGate/StoreGate.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"

#include <algorithm>
#include "AthenaPoolUtilities/AthenaAttributeList.h"

SiLorentzAngleSvc::SiLorentzAngleSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_pixelDefaults(false),
  m_sctDefaults(false),
  m_siConditionsSvc("PixelSiliconConditionsSvc", name),
  m_magFieldSvc("AtlasFieldSvc", name),
  m_detStore("StoreGateSvc/DetectorStore", name),
  m_geoModelSvc("GeoModelSvc", name),
  m_isPixel(true),
  m_siPropertiesSvc("SiPropertiesSvc",name),
  m_detManager(0)
 
{

  m_bfieldFolders.push_back("/GLOBAL/BField/Map");
  m_bfieldFolders.push_back("/EXT/DCS/MAGNETS/SENSORDATA");
    
	declareProperty("SiPropertiesSvc",m_siPropertiesSvc,"SiPropertiesSvc");
  declareProperty("SiConditionsServices", m_siConditionsSvc);
  declareProperty("DetectorName", m_detectorName="Pixel", "Detector name (Pixel or SCT)");
  // Temperature and voltages from job options only used if SiConditionsServices is None or
  // if value read from database is out of range.
  declareProperty("Temperature",m_temperature = -7., "Default temperature in Celcius.");
  declareProperty("TemperaturePixels",m_temperaturePix = -13., "Default temperature in Celcius for Pixels (incl. IBL).");
  declareProperty("BiasVoltage", m_biasVoltage = 150., "Default bias voltage in Volt.");
  declareProperty("BiasVoltageIBLPl", m_biasVoltageIBLPl = 80., "Default bias voltage for IBL SiPl in Volt.");
  declareProperty("BiasVoltageIBL3D", m_biasVoltageIBL3D = 20., "Default bias voltage for IBL Si3D in Volt.");
  declareProperty("DepletionVoltage", m_deplVoltage = 70., "Default depletion voltage in Volt.");
  declareProperty("CorrectionFactor", m_correctionFactor = 1.);
  declareProperty("UseMagFieldSvc", m_useMagFieldSvc = true);
  declareProperty("NominalField", m_nominalField =  2.0834*Gaudi::Units::tesla);
  declareProperty("IgnoreLocalPos", m_ignoreLocalPos = false, "Treat methods that take a local position as if one "
                                                  "called the methods without a local position" );
  declareProperty("CalcEtaComponent", m_calcEta = false);
  declareProperty("DetStore", m_detStore);
  declareProperty("MagFieldSvc", m_magFieldSvc);
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("BFieldFolders", m_bfieldFolders,"Magnetic field folders which may trigger  ");

  declareProperty("CorrDBFolder",m_corrDBFolder="");
  declareProperty("usePixelDefaults",m_pixelDefaults);
  declareProperty("useSctDefaults",m_sctDefaults);

}

SiLorentzAngleSvc::~SiLorentzAngleSvc() {
  //nop
}

StatusCode SiLorentzAngleSvc::initialize() { 

  ATH_MSG_INFO( "SiLorentzAngleSvc Initialized" );

  CHECK(m_siPropertiesSvc.retrieve());

  // Detector store
  CHECK(m_detStore.retrieve());

  // GeoModelSvc
  CHECK(m_geoModelSvc.retrieve()); 

  if (m_geoModelSvc->geoInitialized()) {
    ATH_MSG_INFO("Geometry already initialized. Call geoInitialize.");
    CHECK(geoInitialize());
  } 
  else {
    ATH_MSG_INFO( "Geometry not yet initialized. Registering callback.");
    // Register callback to check when TagInfo has changed
    CHECK(m_detStore->regFcn(&IGeoModelSvc::geoInit,&*m_geoModelSvc,&SiLorentzAngleSvc::geoInitCallback,this));
    ATH_MSG_DEBUG("Registered geoInitCallback callback.");
  }

  // MagneticFieldSvc handles updates itself
  if (!m_useMagFieldSvc) {
    ATH_MSG_DEBUG("Not using MagneticFieldSvc - Will be using Nominal Field!");
  }
  else if (m_magFieldSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING("Could not retrieve MagneticFieldSvc - Will be using Nominal Field!");
    m_useMagFieldSvc = false;
    //return StatusCode::FAILURE;
  }
  
  // DB for corrections
  if (m_corrDBFolder.size()>0) {
    ATH_MSG_INFO("Loading lorentz angle correction value from database folder " << m_corrDBFolder);
    CHECK(m_detStore->regFcn(&SiLorentzAngleSvc::corrFolderCallBack,this,m_dbData,m_corrDBFolder));
  }
  else {
    ATH_MSG_INFO("No database folder set for lorentz angle correction. Use value from jobOptions");
  }

  return StatusCode::SUCCESS;
}    

StatusCode SiLorentzAngleSvc::geoInitCallback(IOVSVC_CALLBACK_ARGS) {
  ATH_MSG_INFO("geoInitCallback is called by callback."); 
  return geoInitialize();
}

StatusCode SiLorentzAngleSvc::geoInitialize() {
  ATH_MSG_INFO("SiLorentzAngleSvc geoInitialize");

  if (m_detectorName != "Pixel" && m_detectorName != "SCT") {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be Pixel or SCT." );
    return StatusCode::FAILURE;
  }
  m_isPixel = (m_detectorName == "Pixel");
 
  // Get conditions summary service.
  if (!m_siConditionsSvc.empty()) {
    ATH_MSG_DEBUG("Conditions Summary Service not empty --> attempting to retrieve.");
    CHECK(m_siConditionsSvc.retrieve());
  }
  else {
    ATH_MSG_DEBUG("Conditions Summary Service not requested.");
  }

  // Get the detector manager
  CHECK(m_detStore->retrieve(m_detManager, m_detectorName));

  if (!m_siConditionsSvc.empty()) {
    ATH_MSG_DEBUG("Conditions Summary Service not empty --> checking if has callback.");
    if (m_siConditionsSvc->hasCallBack()) {
      //Register callback. To be triggered after SiConditionsSvc's callback,
      ATH_MSG_INFO("Registering callback." );
      CHECK(m_detStore->regFcn(&ISiliconConditionsSvc::callBack,&*m_siConditionsSvc,&ISiLorentzAngleSvc::callBack,dynamic_cast<ISiLorentzAngleSvc*>(this),true));
    } 
    else {
      ATH_MSG_WARNING("Conditions Summary Service has no callback." );
    }
  }

  // Get maximum hash for vector sizes. We need the idhelper for this.
  unsigned int maxHash = 0;
  if (m_isPixel) { // Pixel
    const PixelID * idHelper;
    CHECK(m_detStore->retrieve(idHelper,"PixelID"));
    maxHash = idHelper->wafer_hash_max();

    // Check layout
    if      (maxHash<2000) { ATH_MSG_INFO("Suppose RUN-1 geometry..."); }
    else if (maxHash<2100) { ATH_MSG_INFO("Suppose RUN-2 geometry..."); }
    else                   { ATH_MSG_INFO("Suppose RUN-4 geometry..."); }
  } 
  else { // SCT
    const SCT_ID * idHelper;
    CHECK(m_detStore->retrieve(idHelper,"SCT_ID"));
   maxHash = idHelper->wafer_hash_max();
  }

  // In case geoInitialize is called more than once (not likely in practice) 
  m_cacheValid.clear(); 
  m_magFieldCacheValid.clear(); 
  // Initialize the caches
  m_lorentzShift.resize(maxHash);
  m_lorentzShiftEta.resize(maxHash);
  m_tanLorentzAngle.resize(maxHash);
  m_tanLorentzAngleEta.resize(maxHash);
  m_monitorBiasVoltage.resize(maxHash);
  m_monitorTemperature.resize(maxHash);
  m_monitorDepletionVoltage.resize(maxHash);
  m_magFieldCache.resize(maxHash);
  m_cacheValid.resize(maxHash); // initialized to false
  m_magFieldCacheValid.resize(maxHash); // initialized to false
  m_outOfRangeWarning.resize(maxHash); // initialized to false

  return StatusCode::SUCCESS;
}

StatusCode SiLorentzAngleSvc::finalize() {
  // Count number of modules that produced out of range errors.
  int count = std::count(m_outOfRangeWarning.begin(),m_outOfRangeWarning.end(),true);
  if (count > 0)
      ATH_MSG_DEBUG( "Number of detector elements returning invalid temperature: " << count ); 
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode SiLorentzAngleSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( ISiLorentzAngleSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISiLorentzAngleSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode SiLorentzAngleSvc::corrFolderCallBack(IOVSVC_CALLBACK_ARGS) {
  m_correctionFactor=(*m_dbData)["LAScale"].data<float>();
  ATH_MSG_INFO("Correction factor set to " << m_correctionFactor << " based on database (overriding jobO value)" );
  return StatusCode::SUCCESS;
}


StatusCode SiLorentzAngleSvc::callBack(IOVSVC_CALLBACK_ARGS_P(I,keys)) {
  (void) I; // avoid warning about unused parameter 
  ATH_MSG_DEBUG("Callback called." );
  // Work out what triggered the callback
  bool magFieldTrigger = false;
  bool otherTrigger = false;
  if (!m_magFieldSvc.empty() || m_bfieldFolders.empty()) {
    for (std::list<std::string>::const_iterator iter = keys.begin(); iter != keys.end(); ++iter) {
      if (magFieldTrigger && otherTrigger) break; // No need to keep checking 
      if (std::find(m_bfieldFolders.begin(),m_bfieldFolders.end(),*iter) != m_bfieldFolders.end()) {
        magFieldTrigger = true;
      } else {
        otherTrigger = true;
      }
    }
    if (magFieldTrigger) {
      // Will return false if field was not actually updated
      ATH_MSG_VERBOSE("Invalidate mag field cache." );
      magFieldTrigger = invalidateMagFieldCache();
      if (magFieldTrigger){
         ATH_MSG_INFO("Field was updated." );
      } else {
         ATH_MSG_INFO("Field was not updated." );
      }
    }
  } else {
    otherTrigger = true;
  }
  // If either mag field or other item triggered callback then we need to invalidate the general cache.  
  if (otherTrigger || magFieldTrigger) invalidateCache();
  return StatusCode::SUCCESS;
}


double SiLorentzAngleSvc::getLorentzShift(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_lorentzShift[elementHash];
}

double SiLorentzAngleSvc::getLorentzShift(const IdentifierHash & elementHash, const Amg::Vector2D & locPos) {
  if (m_ignoreLocalPos) return getLorentzShift(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_lorentzShift[elementHash];
}

double SiLorentzAngleSvc::getLorentzShiftEta(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_lorentzShiftEta[elementHash];
}

double SiLorentzAngleSvc::getLorentzShiftEta(const IdentifierHash & elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getLorentzShiftEta(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_lorentzShiftEta[elementHash];
}

double SiLorentzAngleSvc::getTanLorentzAngle(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_tanLorentzAngle[elementHash];
}

double SiLorentzAngleSvc::getTanLorentzAngle(const IdentifierHash & elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getTanLorentzAngle(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_tanLorentzAngle[elementHash];
}

double SiLorentzAngleSvc::getTanLorentzAngleEta(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_tanLorentzAngleEta[elementHash];
}

double SiLorentzAngleSvc::getTanLorentzAngleEta(const IdentifierHash & elementHash, const Amg::Vector2D& locPos) {
  if (m_ignoreLocalPos) return getTanLorentzAngleEta(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_tanLorentzAngleEta[elementHash];
}

double SiLorentzAngleSvc::getBiasVoltage(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_monitorBiasVoltage[elementHash];
}

double SiLorentzAngleSvc::getTemperature(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_monitorTemperature[elementHash];
}

double SiLorentzAngleSvc::getDepletionVoltage(const IdentifierHash & elementHash) {
  if (!m_cacheValid[elementHash]) { updateCache(elementHash); }
  return m_monitorDepletionVoltage[elementHash];
}

void SiLorentzAngleSvc::invalidateCache() {
  ATH_MSG_DEBUG("Invalidating general cache." );
  // Invalidate all caches.
  std::fill(m_cacheValid.begin(), m_cacheValid.end(), false);
  
  // InDetReadoutGometry caches the result so we need to invalidate
  // the InDetReadoutGometry cache.
  for (InDetDD::SiDetectorElementCollection::const_iterator iter = m_detManager->getDetectorElementBegin(); 
       iter != m_detManager->getDetectorElementEnd(); 
       ++iter){
    const InDetDD::SiDetectorElement * element = *iter; 
    if (element) element->invalidateConditions();
  }
}

bool SiLorentzAngleSvc::invalidateMagFieldCache() {
  // Only invalidate cache if field was actually updated.
  if (false) {  // !< @TODO fixm e m_magField and m_magField->wasFieldUpdated()) {
    ATH_MSG_VERBOSE("Invalidating magnetic field cache." );
    std::fill(m_magFieldCacheValid.begin(), m_magFieldCacheValid.end(), false);
    return true;
  } else {
    ATH_MSG_DEBUG("Magnetic field callback called but field was not updated. Invalidation of magnetic cache not required." );
    return false;
  }
}  
   
void SiLorentzAngleSvc::updateCache(const IdentifierHash & elementHash) {
  Amg::Vector2D zeroLocalPos(0.,0.);
  updateCache(elementHash, zeroLocalPos, false);
}

void SiLorentzAngleSvc::updateCache(const IdentifierHash & elementHash, const Amg::Vector2D& locPos, bool useLocPos) {
  ATH_MSG_VERBOSE("Updating cache for elementHash = " << elementHash );

  if (!useLocPos) m_cacheValid[elementHash] = true;
  const InDetDD::SiDetectorElement * element = m_detManager->getDetectorElement(elementHash);

  double temperature;
  double deplVoltage;
  double biasVoltage;
  double forceLorentzToZero = 1.0;

  // SCT
  if (!m_isPixel) {
    if (m_sctDefaults && !m_isPixel) {
      biasVoltage = m_biasVoltage*CLHEP::volt;
      deplVoltage = m_deplVoltage*CLHEP::volt;
      temperature = m_temperature+273.15;
    }
    else {
      temperature = m_siConditionsSvc->temperature(elementHash)+273.15;
      deplVoltage = m_siConditionsSvc->depletionVoltage(elementHash)*CLHEP::volt;
      biasVoltage = m_siConditionsSvc->biasVoltage(elementHash)*CLHEP::volt;
      ATH_MSG_DEBUG("SCT Hash = " << elementHash << " Temperature = " << temperature << " BiasV = " << biasVoltage << " DeplV = " << deplVoltage);
    }
  }

  // Pixel
  if (m_isPixel) {
    const InDetDD::PixelModuleDesign* p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    if (m_pixelDefaults) {
      temperature = m_temperaturePix + 273.15;
      if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
        if (p_design->numberOfCircuits()==2) {       // IBL planar
          deplVoltage = 40.0*CLHEP::volt; 
          biasVoltage = m_biasVoltageIBLPl*CLHEP::volt; 
        }
        else if (p_design->numberOfCircuits()==1 && p_design->rowsPerCircuit()>100) {  // IBL 3D
          deplVoltage =  10.0*CLHEP::volt; 
          biasVoltage = m_biasVoltageIBL3D*CLHEP::volt; 
          forceLorentzToZero = 0.0;
        }
      }
      else {
        deplVoltage = m_deplVoltage*CLHEP::volt;
        biasVoltage = m_biasVoltage*CLHEP::volt;
      }
      ATH_MSG_DEBUG("Pixel Hash = " << elementHash << " Temperature = " << temperature << " BiasV = " << biasVoltage << " DeplV = " << deplVoltage);
    }
    else {
      temperature = m_siConditionsSvc->temperature(elementHash)+273.15;
      deplVoltage = m_siConditionsSvc->depletionVoltage(elementHash)*CLHEP::volt;
      biasVoltage = m_siConditionsSvc->biasVoltage(elementHash)*CLHEP::volt;
      if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
        if (p_design->numberOfCircuits()==2) {        // IBL planar (this classification is a bit ugly since there is no method to choose sensor technology.)
          deplVoltage = 40.0*CLHEP::volt; 
        }
        else if (p_design->numberOfCircuits()==1 && p_design->rowsPerCircuit()>100) {  // IBL 3D
          deplVoltage = 10.0*CLHEP::volt; 
          forceLorentzToZero = 0.0;
        }
      }
      ATH_MSG_DEBUG("Pixel Hash = " << elementHash << " Temperature = " << temperature << " BiasV = " << biasVoltage << " DeplV = " << deplVoltage);
    }
  }


  // Protect against invalid temperature
  double temperatureC = temperature -  273.15;
  if (!(temperatureC>-80.0 && temperatureC<100.0)) {
    m_outOfRangeWarning[elementHash] = true;
    temperature = m_temperature + 273.15;
    ATH_MSG_DEBUG("Invalid temperature: " << temperatureC << " C. " << "Setting to " << m_temperature << " C. " << "Detector element hash: " << elementHash);
  }

  // Calculate depletion depth. If biasVoltage is less than depletionVoltage
  // the detector is not fully depleted and we need to take this into account.
  // We take absolute values just in case voltages are signed .  
  double depletionDepth = element->thickness();
  if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
    depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
  }
 
  double meanElectricField = 0;
  if (depletionDepth) { meanElectricField = biasVoltage / depletionDepth; }
  const InDet::SiliconProperties &siProperties = m_siPropertiesSvc->getSiProperties(elementHash);
  double mobility = siProperties.signedHallMobility(element->carrierType());

  // should be removed in rel.22
  if (m_sctDefaults && !m_isPixel) {
    m_siProperties.setConditions(temperature, meanElectricField);
    mobility = m_siProperties.signedHallMobility(element->carrierType());
  }
  if (m_pixelDefaults && m_isPixel) {
    m_siProperties.setConditions(temperature, meanElectricField);
    mobility = m_siProperties.signedHallMobility(element->carrierType());
  }

  // Get magnetic field. This first checks that field cache is valid.
  const Amg::Vector3D& magneticField = getMagneticField(elementHash, locPos, useLocPos);
  

  // The angles are in the hit frame. This is because that is what is needed by the digization and also
  // gives a more physical sign of the angle (ie dosen't flip sign when the detector is flipped).
  // The hit depth axis is pointing from the readout side to the backside if  m_design->readoutSide() < 0
  // The hit depth axis is pointing from the backside to the readout side if  m_design->readoutSide() > 0  
  double tanLorentzAnglePhi = forceLorentzToZero*element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitPhiDirection()*(element->normal().cross(magneticField)).dot(element->phiAxis());
  m_tanLorentzAngle[elementHash] = m_correctionFactor*tanLorentzAnglePhi;

  // This gives the effective correction in the reconstruction frame hence the extra hitPhiDirection()
  // as the angle above is in the hit frame.
  double lorentzCorrectionPhi = -0.5*element->hitPhiDirection()*tanLorentzAnglePhi*depletionDepth;
  m_lorentzShift[elementHash] = m_correctionFactor*lorentzCorrectionPhi;
 
  // Generally no need for the eta component so we only calculate it if requested.
  // Will return zero if not calculated as m_tanLorentzAngleEta[] and  m_lorentzShiftEta[] are initialized to zero.
  //
  // The Lorentz eta shift very small and so can be ignored, but we include it for completeness.
  // It is < ~0.1 um in the pixel.
  // In the SCT its largest in the stereo side of the barrel modules where it is about 0.3 micron along the strip. 
  if (m_calcEta) {
    double tanLorentzAngleEta = forceLorentzToZero*element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitEtaDirection()*(element->normal().cross(magneticField)).dot(element->etaAxis());
    m_tanLorentzAngleEta[elementHash] = m_correctionFactor*tanLorentzAngleEta;
    double lorentzCorrectionEta = -0.5*element->hitPhiDirection()*tanLorentzAngleEta*depletionDepth;
    m_lorentzShiftEta[elementHash] = m_correctionFactor*lorentzCorrectionEta;
  }

  // Monitoring value
  m_monitorBiasVoltage[elementHash]      = biasVoltage/CLHEP::volt;
  m_monitorTemperature[elementHash]      = temperature - 273.15;
  m_monitorDepletionVoltage[elementHash] = deplVoltage/CLHEP::volt;

  ATH_MSG_DEBUG(" Hash = " << elementHash << " tanPhi = " << lorentzCorrectionPhi << " shiftPhi = " << m_lorentzShift[elementHash] << " Factor = " << m_correctionFactor << "Depletion depth = " << depletionDepth);

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Temperature (C), bias voltage, depletion voltage: " << temperature - 273.15 << ", " << biasVoltage/CLHEP::volt << ", " << deplVoltage/CLHEP::volt);
    ATH_MSG_VERBOSE("Depletion depth: " << depletionDepth/CLHEP::mm);
    ATH_MSG_VERBOSE("Mobility (cm2/V/s): " <<  mobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s));
    ATH_MSG_VERBOSE("Magnetic Field (tesla): " << "(" << magneticField.x()/CLHEP::tesla << "," << magneticField.y()/CLHEP::tesla << "," << magneticField.z()/CLHEP::tesla << ")");
    ATH_MSG_VERBOSE("LorentzShift, tanLorentzAngle = " << m_lorentzShift[elementHash] << ", " << m_tanLorentzAngle[elementHash]);
    if (m_calcEta) {
      ATH_MSG_VERBOSE("LorentzShiftEta, tanLorentzAngleEta = " << m_lorentzShiftEta[elementHash] << ", " << m_tanLorentzAngleEta[elementHash]);
    }
  }
}


const Amg::Vector3D& SiLorentzAngleSvc::getMagneticField(const IdentifierHash & elementHash, const Amg::Vector2D& locPos, bool useLocPos) {
  // Get the magnetic field. There are a few cases.
  // If we are not specifying the position (and using the default position which is the center of the detector) 
  // we return the cached value if it has been filled. Otherwise we fetch the field from the field service.
  // For the case where we have a fixed field we use that.
  const InDetDD::SiDetectorElement * element = m_detManager->getDetectorElement(elementHash);
  if (useLocPos || !m_magFieldCacheValid[elementHash]) {
    // Get Magnetic field
    Amg::Vector3D pointvec = element->center();
    if (useLocPos) {
      pointvec = element->globalPosition(locPos);
      // As we use the cache to store the result, it is invalid for subsequent calls
      // which could have a different position and so we invalidate the cache
      m_magFieldCacheValid[elementHash] = false; 
    } else {
      m_magFieldCacheValid[elementHash] = true;
    }
    if(m_useMagFieldSvc){
      ATH_MSG_VERBOSE("Getting magnetic field from magnetic field service." );
      double field[3];
      double point[3];
      point[0]= pointvec[0];
      point[1]= pointvec[1];
      point[2]= pointvec[2];
      m_magFieldSvc->getField(point, field);
      m_magFieldCache[elementHash] = Amg::Vector3D(field[0],field[1],field[2]);
    }
    else {
      m_magFieldCache[elementHash] = Amg::Vector3D(0,0,m_nominalField);
      ATH_MSG_VERBOSE("Using Nominal Field" );
    }
  }
  else ATH_MSG_VERBOSE("Using cached field. " );
  return m_magFieldCache[elementHash];
}
