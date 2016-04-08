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
  m_sctDefaults(true),
  m_siConditionsSvc("PixelSiliconConditionsSvc", name),
  m_magFieldSvc("AtlasFieldSvc", name),
  m_detStore("StoreGateSvc/DetectorStore", name),
  m_geoModelSvc("GeoModelSvc", name),
  m_conditionsSvcValid(false),
  m_conditionsSvcWarning(false),
  m_isPixel(true),
  m_magFieldInit(false),
  m_detManager(0)
 
{

  m_bfieldFolders.push_back("/GLOBAL/BField/Map");
  m_bfieldFolders.push_back("/EXT/DCS/MAGNETS/SENSORDATA");
    
  declareProperty("SiConditionsServices", m_siConditionsSvc);
  declareProperty("DetectorName", m_detectorName="Pixel", "Detector name (Pixel or SCT)");
  // Temperature and voltages from job options only used if SiConditionsServices is None or
  // if value read from database is out of range.
  declareProperty("Temperature",m_temperature = -7., "Default temperature in Celcius.");
  declareProperty("TemperaturePixels",m_temperaturePix = -13., "Default temperature in Celcius for Pixels (incl. IBL).");
  declareProperty("TemperatureMin",m_temperatureMin = -80., "Minimum temperature allowed in Celcius.");
  declareProperty("TemperatureMax",m_temperatureMax = 100., "Maximum temperature allowed in Celcius.");
  declareProperty("BiasVoltage", m_biasVoltage = 150., "Default bias voltage in Volt.");
  declareProperty("BiasVoltageIBLPl", m_biasVoltageIBLPl = 80., "Default bias voltage for IBL SiPl in Volt.");
  declareProperty("BiasVoltageIBL3D", m_biasVoltageIBL3D = 20., "Default bias voltage for IBL Si3D in Volt.");
  declareProperty("DepletionVoltage", m_deplVoltage = 70., "Default depletion voltage in Volt.");
  declareProperty("CorrectionFactor", m_correctionFactor = 1.);
  declareProperty("UseMagFieldSvc", m_useMagFieldSvc = true);
  declareProperty("LoadMagFieldOnInit", m_loadMagFieldOnInit = true);
  declareProperty("NominalField", m_nominalField =  2.0834*Gaudi::Units::tesla);
  declareProperty("UseFixedLorentzAngle", m_useFixedValue=false);
  declareProperty("TanLorentzAngleOverride", m_tanLorentzAngleOverride = 0);
  declareProperty("IgnoreLocalPos", m_ignoreLocalPos = false, "Treat methods that take a local position as if one "
                                                  "called the methods without a local position" );
  declareProperty("CalcEtaComponent", m_calcEta = false);
  declareProperty("DetStore", m_detStore);
  declareProperty("MagFieldSvc", m_magFieldSvc);
  declareProperty("GeoModelSvc", m_geoModelSvc);
  declareProperty("BFieldFolders", m_bfieldFolders,"Magnetic field folders which may trigger  ");
  declareProperty("OutOfRangeWarningThreshold", m_outOfRangeWarningThresh = 10, 
      "Warning will be given if more elements than this return an invalid temperature");

  declareProperty("CorrDBFolder",m_corrDBFolder="");
  declareProperty("usePixelDefaults",m_pixelDefaults);
  declareProperty("useSctDefaults",m_sctDefaults);
}

SiLorentzAngleSvc::~SiLorentzAngleSvc(){
  //nop
}

StatusCode SiLorentzAngleSvc::initialize(){ 

  ATH_MSG_INFO( "SiLorentzAngleSvc Initialized" );

  // Detector store
  if (m_detStore.retrieve().isFailure()) {
    ATH_MSG_FATAL( "DetectorStore service not found !" );
    return StatusCode::FAILURE;  
  }

  // GeoModelSvc
  if (m_geoModelSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Could not locate GeoModelSvc" );
    return StatusCode::FAILURE;
  }
  if (m_geoModelSvc->geoInitialized()) {
    ATH_MSG_INFO("Geometry already initialized. Call geoInitialize.");
    if (geoInitialize().isFailure()){
        if (m_geoModelSvc.retrieve().isFailure()) {
          ATH_MSG_FATAL( "GeoModelSvc::geoInitialize() failed." );
          return StatusCode::FAILURE;
       }
     }
  } else {
    ATH_MSG_INFO( "Geometry not yet initialized. Registering callback.");
    // Register callback to check when TagInfo has changed
    StatusCode sc =  m_detStore->regFcn(&IGeoModelSvc::geoInit, &*m_geoModelSvc,&SiLorentzAngleSvc::geoInitCallback, this);
    if (sc.isFailure()) 
      ATH_MSG_ERROR("Cannot register geoInitCallback function.");
    else 
      ATH_MSG_DEBUG("Registered geoInitCallback callback.");
  }
  // MagneticFieldSvc handles updates itself

  if(!m_useMagFieldSvc) ATH_MSG_DEBUG( "Not using MagneticFieldSvc - Will be using Nominal Field!" );
  else if (m_magFieldSvc.retrieve().isFailure()){
    ATH_MSG_WARNING( "Could not retrieve MagneticFieldSvc - Will be using Nominal Field!" );
    m_useMagFieldSvc = false;
    //return StatusCode::FAILURE;
  }
  
  // DB for corrections
  if (m_corrDBFolder.size()>0) {
    ATH_MSG_INFO("Loading lorentz angle correction value from database folder " << m_corrDBFolder);
    StatusCode sc = m_detStore->regFcn(&SiLorentzAngleSvc::corrFolderCallBack, this, m_dbData,m_corrDBFolder);
    if (sc.isFailure()) {
       ATH_MSG_ERROR("Failed to register Callback function to AthenaAttributeList with key " << m_corrDBFolder );
       return sc;
    }
  }
  else 
    ATH_MSG_INFO("No database folder set for lorentz angle correction. Use value from jobOptions" );

  // Alternative implementation: 
  // Register only the DataHandle and resolve it when needed:
  //    ATH_MSG_INFO("Loading correction value from database folder " << m_corrDBFolder );
  //     sc=m_detStore->regHandle(m_dbData,m_corrDBFolder);
  //     if (sc.isFailure()) {
  //       ATH_MSG_ERROR("Failed to register DataHandle to AthenaAttributeList with key " << m_corrDBFolder );
  //       return sc;
  //     }
  //   }
  //   else 
  //     ATH_MSG_INFO("No database folder set, use correction value from jobOptions" );
  return StatusCode::SUCCESS;
}    

StatusCode SiLorentzAngleSvc::geoInitCallback(IOVSVC_CALLBACK_ARGS){
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
 
  StatusCode sc = StatusCode::SUCCESS;

  // Get conditions summary service. 
  m_conditionsSvcValid = false; 
  if (!m_siConditionsSvc.empty()) {
    sc =  m_siConditionsSvc.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_FATAL("Unable to to retrieve Conditions Summary Service" );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO("Lorentz angle uses Conditions Summary Service." );
      m_conditionsSvcValid = true;
    }
  } else {
    ATH_MSG_INFO("Conditions Summary Service not requested." );
    // Only issue a warning if the service is used.
    m_conditionsSvcWarning = true;
  }

  // Get the detector manager
  m_detStore->retrieve(m_detManager, m_detectorName);
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Could not find the detector manager: " << m_detectorName << " !" );
    return StatusCode::FAILURE;
  } 
  
  if (m_conditionsSvcValid) {  
    if (m_siConditionsSvc->hasCallBack()) {
      //Register callback. To be triggered after SiConditionsSvc's callback,
      ATH_MSG_INFO("Registering callback." );
      sc = m_detStore->regFcn(&ISiliconConditionsSvc::callBack, &*m_siConditionsSvc,
            &ISiLorentzAngleSvc::callBack, dynamic_cast<ISiLorentzAngleSvc *>(this),
            true);
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Could not register callback." );
        return sc;
      }
    } else {
      ATH_MSG_DEBUG("Conditions Summary Service has no callback." );
    }
  }

  // Get maximum hash for vector sizes. We need the idhelper for this.
  unsigned int maxHash = 0;
  if (m_isPixel) {
    // Pixel
    const PixelID * idHelper;
    if (m_detStore->retrieve(idHelper, "PixelID").isFailure()) {
      ATH_MSG_FATAL("Could not get Pixel ID helper" );
      return StatusCode::FAILURE;
    }
    maxHash = idHelper->wafer_hash_max();
  } else {
    // SCT
    const SCT_ID * idHelper;
    if (m_detStore->retrieve(idHelper, "SCT_ID").isFailure()) {
      ATH_MSG_FATAL("Could not get SCT ID helper" );
      return StatusCode::FAILURE;
    }
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
  m_magFieldCache.resize(maxHash);
  m_cacheValid.resize(maxHash); // initialized to false
  m_magFieldCacheValid.resize(maxHash); // initialized to false
  m_outOfRangeWarning.resize(maxHash); // initialized to false
  return StatusCode::SUCCESS;
  
}
StatusCode 
SiLorentzAngleSvc::finalize(){
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
StatusCode
SiLorentzAngleSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
  if ( ISiLorentzAngleSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISiLorentzAngleSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode
SiLorentzAngleSvc::corrFolderCallBack(IOVSVC_CALLBACK_ARGS) {
  m_correctionFactor=(*m_dbData)["LAScale"].data<float>();
  ATH_MSG_INFO("Correction factor set to " << m_correctionFactor << " based on database (overriding jobO value)" );
  return StatusCode::SUCCESS;
}


StatusCode
SiLorentzAngleSvc::callBack(IOVSVC_CALLBACK_ARGS_P(I,keys)){  
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


double 
SiLorentzAngleSvc::getLorentzShift(const IdentifierHash & elementHash){ 
  if (!valid(elementHash)) updateCache(elementHash);
  return m_lorentzShift[elementHash];
}

double 
SiLorentzAngleSvc::getLorentzShift(const IdentifierHash & elementHash, const Amg::Vector2D & locPos){   
  if (m_ignoreLocalPos) return getLorentzShift(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_lorentzShift[elementHash];
}

double 
SiLorentzAngleSvc::getLorentzShiftEta(const IdentifierHash & elementHash){ 
  if (!valid(elementHash)) updateCache(elementHash);
  return m_lorentzShiftEta[elementHash];
}

double 
SiLorentzAngleSvc::getLorentzShiftEta(const IdentifierHash & elementHash, const Amg::Vector2D& locPos){   
  if (m_ignoreLocalPos) return getLorentzShiftEta(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_lorentzShiftEta[elementHash];
}

double 
SiLorentzAngleSvc::getTanLorentzAngle(const IdentifierHash & elementHash){ 
  if (!valid(elementHash)) updateCache(elementHash);
  return m_tanLorentzAngle[elementHash];
}

double 
SiLorentzAngleSvc::getTanLorentzAngle(const IdentifierHash & elementHash, const Amg::Vector2D& locPos){   
  if (m_ignoreLocalPos) return getTanLorentzAngle(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_tanLorentzAngle[elementHash];
}

double 
SiLorentzAngleSvc::getTanLorentzAngleEta(const IdentifierHash & elementHash){ 
  if (!valid(elementHash)) updateCache(elementHash);
  return m_tanLorentzAngleEta[elementHash];
}

double 
SiLorentzAngleSvc::getTanLorentzAngleEta(const IdentifierHash & elementHash, const Amg::Vector2D& locPos){   
  if (m_ignoreLocalPos) return getTanLorentzAngleEta(elementHash);
  // The cache is used to store the results.  The cache is therefore invalidated if we specify a position.
  updateCache(elementHash, locPos, true);
  return m_tanLorentzAngleEta[elementHash];
}

void 
SiLorentzAngleSvc::invalidateCache(){
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

bool
SiLorentzAngleSvc::invalidateMagFieldCache(){
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
   
bool
SiLorentzAngleSvc::valid(const IdentifierHash & elementHash){
  //if (msgLvl(MSG::VERBOSE) ATH_MSG_VERBOSE("Cache valid = " <<  m_cacheValid[elementHash] );
  return m_cacheValid[elementHash];
}

void 
SiLorentzAngleSvc::updateCache(const IdentifierHash & elementHash){
  Amg::Vector2D zeroLocalPos(0.,0.);
  updateCache(elementHash, zeroLocalPos, false);
}


void 
SiLorentzAngleSvc::updateCache(const IdentifierHash & elementHash, const Amg::Vector2D& locPos, bool useLocPos) {
  ATH_MSG_VERBOSE("Updating cache for elementHash = " << elementHash );
  if (m_conditionsSvcWarning) {
    // Only print the warning once.
    m_conditionsSvcWarning = false;
    ATH_MSG_WARNING( "Conditions Summary Service is not used. Will use temperature and voltages from job options. "
      << "Lorentz Angle corrections may be wrong!" );
  }

  if (!useLocPos) m_cacheValid[elementHash] = true;
  const InDetDD::SiDetectorElement * element = m_detManager->getDetectorElement(elementHash);

  bool IBL_DBM_Present = false;
  bool isIBL = false;
  bool is3D = false;
  int forceLorentzToZero = 1;

  if(m_isPixel){
    const PixelID * idHelper;
    if (m_detStore->retrieve(idHelper, "PixelID").isFailure()) {
      ATH_MSG_FATAL("Could not get Pixel ID helper" );
      return;
    }
    unsigned int maxHash = idHelper->wafer_hash_max();
    ATH_MSG_DEBUG( "maxHash: "<< maxHash);
    ///

    const PixelID* pixelID =(dynamic_cast<const PixelID*>(element->getIdHelper()));
    if (not pixelID){
      ATH_MSG_ERROR("Dynamic cast failed at line "<<__LINE__<<" of SiLorentzAngleSvc");
      return;
    }

    int barrelec = pixelID->barrel_ec(element->identify());
    int layer = pixelID->layer_disk(element->identify());
    //int phimodule = pixelID->phi_module(element->identify()); //phi module not needed.
    int etamodule = pixelID->eta_module(element->identify());

    ATH_MSG_DEBUG("m_isPixel: "<< m_isPixel <<" Hash = "<< elementHash <<" Barrel EC = "<< barrelec <<" Layer_Disk = " <<layer<< " etamodule = "
    << etamodule );   

    if(pixelID->wafer_hash_max()>1744)IBL_DBM_Present = true;
    
    if(IBL_DBM_Present){
      if (barrelec == 0 && layer == 0) isIBL = true;
      if (isIBL) {
        if(etamodule < -6 || etamodule > 5) is3D = true;
      }
      if (is3D) forceLorentzToZero = 0;
    }    
  }
  
  double temperature;
  double deplVoltage;
  double biasVoltage;
  if ((!m_conditionsSvcValid && m_isPixel) || (m_pixelDefaults && m_isPixel)) {
    temperature = m_temperaturePix + 273.15;
    if (isIBL && !is3D) deplVoltage = 40. * CLHEP::volt; 
    if (isIBL &&  is3D) deplVoltage =  10. * CLHEP::volt; 
    if (!isIBL) deplVoltage = m_deplVoltage * CLHEP::volt;
    if (isIBL && !is3D) biasVoltage = m_biasVoltageIBLPl * CLHEP::volt; 
    if (isIBL &&  is3D) biasVoltage = m_biasVoltageIBL3D * CLHEP::volt; 
    if (!isIBL) biasVoltage = m_biasVoltage * CLHEP::volt;
    ATH_MSG_DEBUG("Hash = " << elementHash << " Temperature = " << temperature << " BiasV = " << biasVoltage << " DeplV = " << deplVoltage);
  }

  else if ((!m_conditionsSvcValid && !m_isPixel) || (m_sctDefaults && !m_isPixel)) {
   biasVoltage = m_biasVoltage * CLHEP::volt;
   deplVoltage = m_deplVoltage * CLHEP::volt;
   temperature = m_temperature + 273.15;
  }

  else {
    temperature = m_siConditionsSvc->temperature(elementHash) + 273.15;
    deplVoltage = m_siConditionsSvc->depletionVoltage(elementHash) * CLHEP::volt;
    if (isIBL && !is3D) deplVoltage = 40. * CLHEP::volt; 
    if (isIBL &&  is3D) deplVoltage =  10. * CLHEP::volt; 
    biasVoltage = m_siConditionsSvc->biasVoltage(elementHash) * CLHEP::volt;
    ATH_MSG_DEBUG("Hash = " << elementHash << " Temperature = " << temperature << " BiasV = " << biasVoltage << " DeplV = " << deplVoltage);
 }

  // Protect against invalid temperature
  double temperatureC = temperature -  273.15;
  if (!(temperatureC > m_temperatureMin && temperatureC < m_temperatureMax)) {
    m_outOfRangeWarning[elementHash] = true;
    temperature = m_temperature + 273.15;
    ATH_MSG_DEBUG( "Invalid temperature: " 
              << temperatureC << " C. "
              << "Setting to " << m_temperature << " C. "
              << "Detector element hash: " << elementHash 
              );      
  }

  // Calculate depletion depth. If biasVoltage is less than depletionVoltage
  // the detector is not fully depleted and we need to take this into account.
  // We take absolute values just in case voltages are signed .  
  double depletionDepth = element->thickness();
  if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
    depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
  }
 
  double meanElectricField = 0;
  if (depletionDepth) meanElectricField = biasVoltage / depletionDepth;
  double mobility = 0;
  if (m_useFixedValue) {
    // TODO: Need to think about sign,
    mobility = m_tanLorentzAngleOverride/ m_nominalField;
  } else {
    m_siProperties.setConditions(temperature, meanElectricField);
    mobility = m_siProperties.signedHallMobility(element->carrierType());
  }
  // Get magnetic field. This first checks that field cache is valid.
  const Amg::Vector3D& magneticField = getMagneticField(elementHash, locPos, useLocPos);
  

  // The angles are in the hit frame. This is because that is what is needed by the digization and also
  // gives a more physical sign of the angle (ie dosen't flip sign when the detector is flipped).
  // The hit depth axis is pointing from the readout side to the backside if  m_design->readoutSide() < 0
  // The hit depth axis is pointing from the backside to the readout side if  m_design->readoutSide() > 0  
  double tanLorentzAnglePhi = forceLorentzToZero*element->design().readoutSide() * mobility  * element->hitDepthDirection() * element->hitPhiDirection() 
    * (element->normal().cross(magneticField)).dot(element->phiAxis());
  m_tanLorentzAngle[elementHash]    = m_correctionFactor * tanLorentzAnglePhi;

  // This gives the effective correction in the reconstruction frame hence the extra hitPhiDirection()
  // as the angle above is in the hit frame.
  double lorentzCorrectionPhi = -0.5 * element->hitPhiDirection() * tanLorentzAnglePhi * depletionDepth;
  m_lorentzShift[elementHash] = m_correctionFactor * lorentzCorrectionPhi;
  //
  // Generally no need for the eta component so we only calculate it if requested.
  // Will return zero if not calculated as m_tanLorentzAngleEta[] and  m_lorentzShiftEta[] are initialized to zero.
  //
  // The Lorentz eta shift very small and so can be ignored, but we include it for completeness.
  // It is < ~0.1 um in the pixel.
  // In the SCT its largest in the stereo side of the barrel modules where it is about 0.3 micron along the strip. 
  //
  if (m_calcEta) {
    double tanLorentzAngleEta = forceLorentzToZero*element->design().readoutSide() * mobility  * element->hitDepthDirection() * element->hitEtaDirection() 
      * (element->normal().cross(magneticField)).dot(element->etaAxis());
    m_tanLorentzAngleEta[elementHash] = m_correctionFactor * tanLorentzAngleEta;
    double lorentzCorrectionEta = -0.5 * element->hitPhiDirection() * tanLorentzAngleEta * depletionDepth;
    m_lorentzShiftEta[elementHash] = m_correctionFactor * lorentzCorrectionEta;
  }

  ATH_MSG_DEBUG(" Hash = " << elementHash << " tanPhi = " << lorentzCorrectionPhi << " shiftPhi = " << m_lorentzShift[elementHash]
  << " Factor = " << m_correctionFactor << "Depletion depth = " << depletionDepth);

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Temperature (C), bias voltage, depletion voltage: "
          << temperature - 273.15 << ", "
          << biasVoltage/CLHEP::volt << ", "
          << deplVoltage/CLHEP::volt );
    ATH_MSG_VERBOSE("Depletion depth: " << depletionDepth/CLHEP::mm );
    ATH_MSG_VERBOSE("Mobility (cm2/V/s): " <<  mobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s) );
    ATH_MSG_VERBOSE("Magnetic Field (tesla): " 
          << "(" << magneticField.x()/CLHEP::tesla << "," << magneticField.y()/CLHEP::tesla << "," << magneticField.z()/CLHEP::tesla << ")"
          );
    ATH_MSG_VERBOSE("LorentzShift, tanLorentzAngle = " 
          << m_lorentzShift[elementHash] << ", "
          << m_tanLorentzAngle[elementHash] );
    if (m_calcEta) {
      ATH_MSG_VERBOSE("LorentzShiftEta, tanLorentzAngleEta = " 
      << m_lorentzShiftEta[elementHash] << ", " 
      << m_tanLorentzAngleEta[elementHash] );
    }
  }
}


const Amg::Vector3D& SiLorentzAngleSvc::getMagneticField(const IdentifierHash & elementHash, const Amg::Vector2D& locPos, bool useLocPos){
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
