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

SiPropertiesSvc::SiPropertiesSvc( const std::string& name, ISvcLocator* pSvcLocator ):
  AthService(name, pSvcLocator),
  m_electronSaturationVelocity(1.53e9),
  m_holeSaturationVelocity(1.62e8),
  m_siConditionsSvc("PixelSiliconConditionsSvc", name),
  m_detStore("StoreGateSvc/DetectorStore", name),
  m_conditionsSvcValid(false),
  m_detManager(0)
{
  declareProperty("TemperatureMin",m_temperatureMin = -80., "Minimum temperature allowed in Celcius.");
  declareProperty("TemperatureMax",m_temperatureMax = 100., "Maximum temperature allowed in Celcius.");
  declareProperty("ElectronSaturationVelocity", m_electronSaturationVelocity = 1.53e9, "Electron Saturation Velocity [cm/s]");
  declareProperty("HoleSaturationVelocity",     m_holeSaturationVelocity     = 1.62e8, "Hole Saturation Velocity [cm/s]");
  declareProperty("SiConditionsServices", m_siConditionsSvc);
  declareProperty("DetectorName", m_detectorName="Pixel");
  declareProperty("UseConditionsDB", m_conditionsSvcValid=true);
}

SiPropertiesSvc::~SiPropertiesSvc()
{}


StatusCode SiPropertiesSvc::initialize() { 
  ATH_MSG_INFO("SiPropertiesSvc Initialized");

  CHECK(AthService::initialize());

  if (m_detectorName!="Pixel" && m_detectorName!="SCT") {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be Pixel or SCT.");
    return StatusCode::FAILURE;
  }

  // Get conditions summary service. 
  if (m_conditionsSvcValid) {
    CHECK(m_siConditionsSvc.retrieve());
  }

  // Detector store
  CHECK(m_detStore.retrieve());

  // Get the detector manager
  CHECK(m_detStore->retrieve(m_detManager,m_detectorName));

  //Register callback. To be triggered after SiConditionsSvc's callback,
  if (m_siConditionsSvc->hasCallBack()) {
    ATH_MSG_INFO("Registering callback.");
    CHECK(m_detStore->regFcn(&ISiliconConditionsSvc::callBack,&*m_siConditionsSvc,&ISiPropertiesSvc::callBack,dynamic_cast<ISiPropertiesSvc *>(this),true));
  }

  bool isPixel = (m_detectorName == "Pixel");

  // Get maximum hash for vector sizes. We need the idhelper for this.
  unsigned int maxHash = 0;
  if (isPixel) {
    // Pixel
    const PixelID * idHelper;
    CHECK(m_detStore->retrieve(idHelper,"PixelID"));
    maxHash = idHelper->wafer_hash_max();
  } 
  else {
    // SCT
    const SCT_ID * idHelper;
    CHECK(m_detStore->retrieve(idHelper,"SCT_ID"));
    maxHash = idHelper->wafer_hash_max();
  }
  
  // Initialize the caches
  m_propertiesCache.resize(maxHash);
  m_cacheValid.resize(maxHash); // initialized to false
  //m_outOfRangeWarning.resize(maxHash); // initialized to false

  return StatusCode::SUCCESS;
}

StatusCode SiPropertiesSvc::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode SiPropertiesSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( ISiPropertiesSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISiPropertiesSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode SiPropertiesSvc::callBack(IOVSVC_CALLBACK_ARGS) {  
  if (msgLvl(MSG::DEBUG)) { 
    ATH_MSG_DEBUG("Callback called.");
  }
  invalidateCache();
  return StatusCode::SUCCESS;
}

const InDet::SiliconProperties & SiPropertiesSvc::getSiProperties(const IdentifierHash & elementHash) { 
  if (!valid(elementHash)) updateCache(elementHash);
  return m_propertiesCache[elementHash];
}

void SiPropertiesSvc::updateCache(const IdentifierHash & elementHash) {
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
    //						<< endreq;
    //  temperature = m_temperature + 273.15;
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG("Invalid temperature: "  << temperatureC << " C. " << "Setting to " <<  DEFAULTTEMPERATURE << " C. " << "Detector element hash: " << elementHash);
    }
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

  // Change saturation velocity
  m_propertiesCache[elementHash].setElectronSaturationVelocity(m_electronSaturationVelocity*CLHEP::cm/CLHEP::s);
  m_propertiesCache[elementHash].setHoleSaturationVelocity(m_holeSaturationVelocity*CLHEP::cm/CLHEP::s);

  m_propertiesCache[elementHash].setConditions(temperature, meanElectricField);
  m_cacheValid[elementHash] = true;

  if (msgLvl(MSG::VERBOSE)) {
    ATH_MSG_VERBOSE("Temperature (C), bias voltage, depletion voltage: " << temperature - 273.15 << ", " << biasVoltage/CLHEP::volt << ", " << deplVoltage/CLHEP::volt);
    double hallMobility = m_propertiesCache[elementHash].signedHallMobility(element->carrierType());
    double driftMobility = m_propertiesCache[elementHash].driftMobility(element->carrierType());
    double ehPairsPerEnergy = m_propertiesCache[elementHash].electronHolePairsPerEnergy();
    double diffConst = m_propertiesCache[elementHash].diffusionConstant(element->carrierType());
    ATH_MSG_VERBOSE("Signed Mobility (cm2/V/s):  " <<  hallMobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s));
    ATH_MSG_VERBOSE("Drift Mobility (cm2/V/s):   " <<  driftMobility/(CLHEP::cm2/CLHEP::volt/CLHEP::s));
    ATH_MSG_VERBOSE("eh pairs per eV:            " <<  ehPairsPerEnergy/(1./CLHEP::eV));
    ATH_MSG_VERBOSE("Diffusion constant (cm2/s): " <<  diffConst/(CLHEP::cm2/CLHEP::s));
  }
}

void SiPropertiesSvc::invalidateCache() {
  // Invalidate all caches.
  std::fill(m_cacheValid.begin(), m_cacheValid.end(), false);
}


bool SiPropertiesSvc::valid(const IdentifierHash & elementHash) {
  //if (msgLvl(MSG::VERBOSE) msg(MSG::VERBOSE) << "Cache valid = " <<  m_cacheValid[elementHash] << endreq;
  return m_cacheValid[elementHash];
}
