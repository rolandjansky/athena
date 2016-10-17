/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SiPropertiesSvc/SiPropertiesSvc.h"


#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "StoreGate/StoreGate.h"

#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"

const double DEFAULTTEMPERATURE = -7;  // Degree C
const double DEFAULTDEPLVOLTAGE = 70;  // Volt
const double DEFAULTBIASVOLTAGE = 150; // Volt

SiPropertiesSvc::SiPropertiesSvc( const std::string& name, ISvcLocator* pSvcLocator ) : 
  AthService(name, pSvcLocator),
  m_siConditionsSvc("PixelSiliconConditionsSvc", name),
  m_detStore("StoreGateSvc/DetectorStore", name),
  m_conditionsSvcValid(false),
  m_detManager(0)
{
  declareProperty("TemperatureMin",m_temperatureMin = -80., "Minimum temperature allowed in Celcius.");
  declareProperty("TemperatureMax",m_temperatureMax = 100., "Maximum temperature allowed in Celcius.");
  declareProperty("SiConditionsServices", m_siConditionsSvc);
  declareProperty("DetectorName", m_detectorName="Pixel");
}

SiPropertiesSvc::~SiPropertiesSvc()
{}


StatusCode 
SiPropertiesSvc::initialize()
{ 
  msg(MSG::INFO) << "SiPropertiesSvc Initialized" << endmsg;
  
  StatusCode sc = AthService::initialize();
  if (sc.isFailure()) { 
    msg(MSG::FATAL) << "Unable to initialize the service!" << endmsg;
    return sc;
  } 
  
  if (m_detectorName != "Pixel" && m_detectorName != "SCT") {
    msg(MSG::FATAL) << "Invalid detector name: " << m_detectorName << ". Must be Pixel or SCT." << endmsg;
    return StatusCode::FAILURE;
  }

  // Get conditions summary service. 
  m_conditionsSvcValid = false; 
  sc =  m_siConditionsSvc.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Unable to to retrieve Conditions Summary Service" << endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "SiPropertiesSvc successfully loaded Conditions Summary Service." << endmsg;
    m_conditionsSvcValid = true;
  }
 
  // Detector store
  sc = m_detStore.retrieve();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" << endmsg;
    return StatusCode::FAILURE;  
  }

  // Get the detector manager
  m_detStore->retrieve(m_detManager, m_detectorName);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not find the detector manager: " << m_detectorName << " !" << endmsg;
    return StatusCode::FAILURE;
  } 
  
  if (m_conditionsSvcValid) {  
    if (m_siConditionsSvc->hasCallBack()) {
      //Register callback. To be triggered after SiConditionsSvc's callback,
      msg(MSG::INFO) << "Registering callback." << endmsg;
      sc = m_detStore->regFcn(&ISiliconConditionsSvc::callBack, &*m_siConditionsSvc,
			      &ISiPropertiesSvc::callBack, dynamic_cast<ISiPropertiesSvc *>(this),
			      true);
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Could not register callback." << endmsg;
	return sc;
      }
    } else {
      msg(MSG::DEBUG) << "Conditions Summary Service has no callback." << endmsg;
    }
  }

  bool isPixel = (m_detectorName == "Pixel");

  // Get maximum hash for vector sizes. We need the idhelper for this.
  unsigned int maxHash = 0;
  if (isPixel) {
    // Pixel
    const PixelID * idHelper;
    if (m_detStore->retrieve(idHelper, "PixelID").isFailure()) {
      msg(MSG::FATAL) << "Could not get Pixel ID helper" << endmsg;
      return StatusCode::FAILURE;
    }
    maxHash = idHelper->wafer_hash_max();
  } else {
    // SCT
    const SCT_ID * idHelper;
    if (m_detStore->retrieve(idHelper, "SCT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get SCT ID helper" << endmsg;
      return StatusCode::FAILURE;
    }
    maxHash = idHelper->wafer_hash_max();
  }
  
  // Initialize the caches
  m_propertiesCache.resize(maxHash);
  m_cacheValid.resize(maxHash); // initialized to false
  //m_outOfRangeWarning.resize(maxHash); // initialized to false

  return StatusCode::SUCCESS;
  
}

StatusCode 
SiPropertiesSvc::finalize()
{
  // Count number of modules that produced out of range errors.
  //int count = std::count(m_outOfRangeWarning.begin(),m_outOfRangeWarning.end(),true);
  
  //if (count > 0) {
    //if (count > m_outOfRangeWarningThresh) {
      //if (count == 1) { 
	//if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "There was 1 detector element returning an invalid temperature. Temperature was set to " 
	//					    << m_temperature << " C for this detector element." << endmsg; 
      //} else {
	//if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "There were " << count 
		//				    << " detector elements returning an invalid temperature. Temperature was set to " 
			//			    << m_temperature << " C for these detector elements." << endmsg; 
      //}
    //} else {
      //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of detector elements returning invalid temperature: " << count << endmsg; 
    //}
  //}
  
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode
SiPropertiesSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ISiPropertiesSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISiPropertiesSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode
SiPropertiesSvc::callBack(IOVSVC_CALLBACK_ARGS)
{  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Callback called." << endmsg;
  invalidateCache();
  return StatusCode::SUCCESS;
}

const InDet::SiliconProperties &
SiPropertiesSvc::getSiProperties(const IdentifierHash & elementHash){ 
  if (!valid(elementHash)) updateCache(elementHash);
  return m_propertiesCache[elementHash];
}

void
SiPropertiesSvc::updateCache(const IdentifierHash & elementHash)
{
  
  const InDetDD::SiDetectorElement * element = m_detManager->getDetectorElement(elementHash);
  
  double temperature;
  double deplVoltage;
  double biasVoltage;
  if (!m_conditionsSvcValid) {
    temperature = DEFAULTTEMPERATURE + 273.15;
    deplVoltage = DEFAULTDEPLVOLTAGE * CLHEP::volt;
    biasVoltage = DEFAULTBIASVOLTAGE * CLHEP::volt;
  } else {
    temperature = m_siConditionsSvc->temperature(elementHash) + 273.15;
    deplVoltage = m_siConditionsSvc->depletionVoltage(elementHash) * CLHEP::volt;
    biasVoltage = m_siConditionsSvc->biasVoltage(elementHash) * CLHEP::volt;
  }
  
  // Protect against invalid temperature
  double temperatureC = temperature -  273.15;
  if (!(temperatureC > m_temperatureMin && temperatureC < m_temperatureMax)) {
    //if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Invalid temperature: " << temperatureC << " C. "
    //						<< "Setting to " << DEFAULTTEMPERATURE << " C."
    //						<< endmsg;
   //  temperature = m_temperature + 273.15;

     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Invalid temperature: "  
 					    << temperatureC << " C. " 
 					    << "Setting to " <<  DEFAULTTEMPERATURE << " C. " 
 					    << "Detector element hash: " << elementHash  
 					    << endmsg;       
    
    temperature = DEFAULTTEMPERATURE + 273.15;
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

  m_propertiesCache[elementHash].setConditions(temperature, meanElectricField);
  m_cacheValid[elementHash] = true;


  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Temperature (C), bias voltage, depletion voltage: "
		      << temperature - 273.15 << ", "
		      << biasVoltage/CLHEP::volt << ", "
		      << deplVoltage/CLHEP::volt << endmsg;
    double hallMobility = m_propertiesCache[elementHash].signedHallMobility(element->carrierType());
    double driftMobility = m_propertiesCache[elementHash].driftMobility(element->carrierType());
    double ehPairsPerEnergy = m_propertiesCache[elementHash].electronHolePairsPerEnergy();
    double diffConst = m_propertiesCache[elementHash].diffusionConstant(element->carrierType());
    msg(MSG::VERBOSE) << "Signed Mobility (cm2/V/s):  " <<  hallMobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s) << endmsg;
    msg(MSG::VERBOSE) << "Drift Mobility (cm2/V/s):   " <<  driftMobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s) << endmsg;
    msg(MSG::VERBOSE) << "eh pairs per eV:            " <<  ehPairsPerEnergy/(1./CLHEP::eV) << endmsg;
    msg(MSG::VERBOSE) << "Diffusion constant (cm2/s): " <<  diffConst/(CLHEP::cm2/CLHEP::s) << endmsg;
  }

}

void 
SiPropertiesSvc::invalidateCache()
{
  // Invalidate all caches.
  std::fill(m_cacheValid.begin(), m_cacheValid.end(), false);
}


bool
SiPropertiesSvc::valid(const IdentifierHash & elementHash){
  //if (msgLvl(MSG::VERBOSE) msg(MSG::VERBOSE) << "Cache valid = " <<  m_cacheValid[elementHash] << endmsg;
  return m_cacheValid[elementHash];
}
