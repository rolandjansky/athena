/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_ChargeTrappingSvc.cxx
 * @author Peter Vankov (peter.vankov@cern.ch)
 * @author Marco Filipuzzi (marco.filipuzzi@cern.ch)
 **/

#include "SCT_ChargeTrappingSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

#include "InDetIdentifier/SCT_ID.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorManager.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Random/RandFlat.h"



#include "StoreGate/StoreGate.h"

#include "InDetConditionsSummaryService/ISiliconConditionsSvc.h"

#include "SCT_ConfigurationConditionsSvc.h"

#include <algorithm>
#include "TMath.h"

SCT_ChargeTrappingSvc::SCT_ChargeTrappingSvc( const std::string& name,  ISvcLocator* pSvcLocator ) :
  AthService(name, pSvcLocator),
  m_siConditionsSvc{"SCT_SiliconConditionsSvc", name},
  m_detStore{"StoreGateSvc/DetectorStore", name},
  m_conditionsSvcValid{false},
  m_conditionsSvcWarning{false},
  m_isSCT{true},
  m_detManager{nullptr},
  m_getdoCTrap{false},
  m_PotentialValue{{0.}}
{
  
  declareProperty("SiConditionsServices", m_siConditionsSvc);
  declareProperty("DetectorName", m_detectorName="SCT",  "Detector name");
  // Temperature and voltages from job options only used if  SiConditionsServices is None or
  // if value read from database is out of range.
  declareProperty("Temperature",m_temperature = -2., "Default  temperature in Celcius."); 
  declareProperty("TemperatureMin",m_temperatureMin = -80.,  "Minimum temperature allowed in Celcius.");
  declareProperty("TemperatureMax",m_temperatureMax = 100.,  "Maximum temperature allowed in Celcius.");
  declareProperty("BiasVoltage", m_biasVoltage = 150., "Default  bias voltage in Volt.");
  declareProperty("DepletionVoltage", m_deplVoltage = 70.,  "Default depletion voltage in Volt.");
  // declareProperty("IgnoreLocalPos", m_ignoreLocalPos = false,  "Treat methods that take a local position as if one "
  //     "called the methods without a local position" );
  declareProperty("DetStore", m_detStore);
  
  // -- Radiation damage specific
  declareProperty("CalcHoles", m_calcHoles=false,"Default is to ignore holes in signal formation.");
  // -- Fluence: Need to make it layer-dependent
  declareProperty("Fluence", m_fluence=(double)1.0E15, "Fluence received by the detector."); 
  declareProperty("BetaElectrons",m_betaElectrons=(double)3.1E-16,"Constant for the trapping model for electrons, in [cm^2/ns] " 
                  "-- average value from Table 2 in ATL-INDET-2003-014");
  declareProperty("BetaHoles",m_betaHoles=(double)5.1E-16,"Constant for the trapping model for holes in [cm^2/ns] " 
                  "-- average value from Table 2 in ATL-INDET-2003-014");
}

SCT_ChargeTrappingSvc::~SCT_ChargeTrappingSvc()
{}

StatusCode 
SCT_ChargeTrappingSvc::initialize()
{
  
  StatusCode sc = AthService::initialize();
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Unable to initialize the service!" <<  endmsg;
    return sc;
  }
  
  // Detector store
  if (m_detStore.retrieve().isFailure()) {
    msg(MSG::FATAL) << "DetectorStore service not found !" <<  endmsg;
    return StatusCode::FAILURE;
  }
  
  if (m_detectorName != "SCT") {
    msg(MSG::FATAL) << "Invalid detector name: " << m_detectorName  << ". Must be SCT." << endmsg;
    return StatusCode::FAILURE;
  }
  m_isSCT = (m_detectorName == "SCT");
  
  // Get conditions summary service.
  m_conditionsSvcValid = false;
  if (!m_siConditionsSvc.empty()) {
    sc =  m_siConditionsSvc.retrieve();
    if (sc.isFailure()) {
      msg(MSG::FATAL) << "Unable to to retrieve Conditions Summary  Service" << endmsg;
      return StatusCode::FAILURE;
    } else {
      m_conditionsSvcValid = true;
    }
  } else {
    m_conditionsSvcWarning = true;
  }
  
  // Get the detector manager
  m_detStore->retrieve(m_detManager, m_detectorName);
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not find the detector manager: " <<  m_detectorName << " !" << endmsg;
    return StatusCode::FAILURE;
  }
  
  if (m_conditionsSvcValid) {
    if (m_siConditionsSvc->hasCallBack()) {
      sc = m_detStore->regFcn(&ISiliconConditionsSvc::callBack,  &*m_siConditionsSvc,
                              &ISCT_ChargeTrappingSvc::callBack,  dynamic_cast<ISCT_ChargeTrappingSvc *>(this),
                              true);
      if (sc.isFailure()) {
        msg(MSG::ERROR) << "Could not register callback." << endmsg;
        return sc;
      }
    } else {
    }
  }
  
  // Get maximum hash for vector sizes. We need the idhelper for  this.
  unsigned int maxHash = 0;
  if (m_isSCT) {
    // SCT
    const SCT_ID * idHelper;
    if (m_detStore->retrieve(idHelper, "SCT_ID").isFailure()) {
      msg(MSG::FATAL) << "Could not get SCT_ID helper" << endmsg;
      return StatusCode::FAILURE;
    }
    maxHash = idHelper->wafer_hash_max();
  } else {
    msg(MSG::FATAL) << "Invalid detector name: " << m_detectorName  << ". Must be SCT." << endmsg;
    msg(MSG::FATAL) << "Could not get " << m_detectorName << " ID helper" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // In case geoInitialize is called more than once (not likely in  practice)
  m_cacheValid.clear();

  
  // Initialize the caches
  m_electricField.resize(maxHash);
  m_meanFreePathElectrons.resize(maxHash);
  m_meanFreePathHoles.resize(maxHash);
  m_trappingElectrons.resize(maxHash);
  m_trappingHoles.resize(maxHash);
  m_trappingProbability.resize(maxHash);
  m_trappingTime.resize(maxHash);
  m_electrodeTime.resize(maxHash);
  m_trappingPosition.resize(maxHash);
  m_holeDriftMobility.resize(maxHash);
  m_getdoCTrap = false;

  return StatusCode::SUCCESS;
  
}


StatusCode SCT_ChargeTrappingSvc::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode SCT_ChargeTrappingSvc::queryInterface(const InterfaceID& riid, void**  ppvInterface)
{
  if ( ISCT_ChargeTrappingSvc::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<ISCT_ChargeTrappingSvc *>(this);
  }  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


StatusCode SCT_ChargeTrappingSvc::callBack(IOVSVC_CALLBACK_ARGS_P(I,keys))
{
  (void) I; // avoid warning about unused parameter
  (void) keys;
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Callback called." <<  endmsg;
  
  invalidateCache();
  
  return StatusCode::SUCCESS;
}


bool SCT_ChargeTrappingSvc::getdoCTrap(const IdentifierHash &  elementHash, const double & pos)  
{
  updateCache(elementHash, pos);
  return m_getdoCTrap=true;
}


double SCT_ChargeTrappingSvc::getElectricField(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_electricField[elementHash];
}

double SCT_ChargeTrappingSvc::getTrappingElectrons(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_trappingElectrons[elementHash];
}

double SCT_ChargeTrappingSvc::getTrappingHoles(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_trappingHoles[elementHash];
} 

double SCT_ChargeTrappingSvc::getMeanFreePathElectrons(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_meanFreePathElectrons[elementHash];
}

double SCT_ChargeTrappingSvc::getMeanFreePathHoles(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_meanFreePathHoles[elementHash];
}

double SCT_ChargeTrappingSvc::getTrappingProbability(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_trappingProbability[elementHash];
}

double SCT_ChargeTrappingSvc::getTrappingTime(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_trappingTime[elementHash];
}

double SCT_ChargeTrappingSvc::getTimeToElectrode(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_electrodeTime[elementHash];
}

double SCT_ChargeTrappingSvc::getTrappingPositionZ(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_trappingPosition[elementHash];
}

double SCT_ChargeTrappingSvc::getHoleDriftMobility(const IdentifierHash &  elementHash, const double & pos)
{
  (void) pos;
  return m_holeDriftMobility[elementHash];
}

void SCT_ChargeTrappingSvc::getHoleTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const
{
  holeTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2);
}

void SCT_ChargeTrappingSvc::getInitPotentialValue()
{
  initPotentialValue();
}

void SCT_ChargeTrappingSvc::invalidateCache()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Invalidating general  cache." << endmsg;
  
  // Invalidate all caches.
  std::fill(m_cacheValid.begin(), m_cacheValid.end(), false);
  
  // InDetReadoutGometry caches the result so we need to invalidate
  // the InDetReadoutGometry cache.
  for (InDetDD::SiDetectorElementCollection::const_iterator iter =  m_detManager->getDetectorElementBegin();
       iter != m_detManager->getDetectorElementEnd();
       ++iter){
    const InDetDD::SiDetectorElement * element = *iter;
    if (element) element->invalidateConditions();
  }
  
}


bool SCT_ChargeTrappingSvc::valid(const IdentifierHash & elementHash){
  return m_cacheValid[elementHash];
}


void SCT_ChargeTrappingSvc::updateCache(const IdentifierHash & elementHash,  const double & pos) {  
  
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Updating cache  for elementHash = " << elementHash << endmsg;
  
  if (m_conditionsSvcWarning) {
    // Only print the warning once.
    m_conditionsSvcWarning = false;
    msg(MSG::WARNING)
      << "Conditions Summary Service is not used. Will use temperature and voltages from job options. "
      << "Effects of radiation damage may be wrong!"
      << endmsg;
  }
  
  //  m_cacheValid[elementHash] = true;
  
  
  const InDetDD::SiDetectorElement * element = m_detManager->getDetectorElement(elementHash);
  
  double temperature;
  double deplVoltage;
  double biasVoltage;
  double totalFluence;
  double betaElectrons;
  double betaHoles;
  if (!m_conditionsSvcValid) {
    temperature = m_temperature + 273.15;
    deplVoltage = m_deplVoltage * CLHEP::volt;
    biasVoltage = m_biasVoltage * CLHEP::volt;
    totalFluence = m_fluence;
    betaElectrons = m_betaElectrons;
    betaHoles = m_betaHoles;
  } else {
    temperature = m_siConditionsSvc->temperature(elementHash) +  273.15;
    deplVoltage = m_siConditionsSvc->depletionVoltage(elementHash)  * CLHEP::volt;
    biasVoltage = m_siConditionsSvc->biasVoltage(elementHash) *  CLHEP::volt;
    // deplVoltage = m_siConditionsSvc->depletionVoltage(elementHash);
    // biasVoltage = m_siConditionsSvc->biasVoltage(elementHash);
    totalFluence = m_fluence;              // -- this should be read from the Conditions database
    betaElectrons = m_betaElectrons;
    betaHoles = m_betaHoles;
  }
  
  // Protect against invalid temperature
  double temperatureC = temperature -  273.15;
  if (!(temperatureC > m_temperatureMin && temperatureC <  m_temperatureMax)) {
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Invalid  temperature: " << temperatureC << " C. "
                                                << "Setting to "  << m_temperature << " C."
                                                << endmsg;
    temperature = m_temperature + 273.15;
  }
  
  // Calculate depletion depth. If biasVoltage is less than  depletionVoltage
  // the detector is not fully depleted and we need to take this  into account.
  // We take absolute values just in case voltages are signed .
  double depletionDepth = element->thickness();
  if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
    depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
    // -- if this was the case would need to re-calculate the Ramo Potential and other parameters.
  }
  
  double electricField=m_electricFieldTool->getElectricField(pos,//posZ
                                                             totalFluence,
                                                             fabs(deplVoltage),
                                                             element->thickness(),
                                                             fabs(biasVoltage));

  //electric field will be a function of bias voltage and fluence...

  //  std::cout << " posz: " << pos << " deplVoltage: " << deplVoltage << " thickness " << element->thickness() << 
  //   " biasVoltage " << fabs(biasVoltage) << " E field " << electricField << std::endl;

  m_electricField[elementHash] = electricField;

  m_siProperties.setConditions(temperature, electricField);


  // -- Calculate electron and holes drift mobility and velocity for these conditions (temperature, electricField)
  //    using parametrizations in SiliconProperties (SiPropertiesSvc). These will be used later for the trapping model.
  //    In the SCT we collect holes. 
  double electronDriftMobility = 0.;
  double holeDriftMobility = 0.;
  double electronDriftVelocity = 0.;
  double holeDriftVelocity = 0.;
  if(element->carrierType()==InDetDD::electrons){
    //    electronDriftMobility = m_siProperties.calcElectronDriftMobility(temperature,electricField);
    //    electronDriftVelocity = electronDriftMobility*electricField;
  } else {
    if (m_calcHoles){
      holeDriftMobility = m_siProperties.calcHoleDriftMobility(temperature,electricField*CLHEP::volt)*CLHEP::volt; //in this way you could put the electric field in V/mm and the mobility will be in [V mm^2 ns^-1]
      m_holeDriftMobility[elementHash] = holeDriftMobility;    
      holeDriftVelocity = holeDriftMobility*electricField;
    }
  }

  
  // -- Calculate Trapping Times
  double trappingElectrons = 1./(double)(totalFluence*betaElectrons);
  //  m_trappingElectrons[elementHash] = trappingElectrons;

  double trappingHoles = 0.;
  if (m_calcHoles) {trappingHoles = 1./(double)(totalFluence*betaHoles);
    m_trappingHoles[elementHash] = trappingHoles;
  }
  
  
  
  // -- Calculate Mean Free Path
  double meanFreePathElectrons = electronDriftVelocity*trappingElectrons;
  //  m_meanFreePathElectrons[elementHash] = meanFreePathElectrons;

  double meanFreePathHoles = 0.;
  if (m_calcHoles) {meanFreePathHoles = holeDriftVelocity*trappingHoles;
    m_meanFreePathHoles[elementHash] = meanFreePathHoles;
  }
  

  // -- Trapping probability
  double trappingProbability_electron = 0.0;
  double trappingProbability_hole = 0.0;
  double trappingProbability = 0.0;
  double zpos = pos;
  if (element->carrierType()==InDetDD::electrons){
    trappingProbability = 1.0 - TMath::Exp(-TMath::Abs(zpos/meanFreePathElectrons));
    trappingProbability_electron = trappingProbability;
  }
  else {
    if (m_calcHoles){
      trappingProbability = 1.0 - TMath::Exp(-TMath::Abs(zpos/meanFreePathHoles));
      trappingProbability_hole = trappingProbability;
    }
    else 
      trappingProbability = 0.0;
  }
  m_trappingProbability[elementHash] = trappingProbability;

  


  // -- Drift time without being trapped
  double u = CLHEP::RandFlat::shoot(0.,1.);
  double drift_time =(-1.)*TMath::Log(u)*trappingHoles;              
  m_trappingTime[elementHash] = drift_time;                   

  
  // -- Time to arrive to the electrode
  //double t_electrode = zpos/electronDriftVelocity;
  double t_electrode_hole = zpos/holeDriftVelocity;
  // m_electrodeTime[elementHash] = t_electrode;
  m_electrodeTime[elementHash] = t_electrode_hole;


  // -- Position at which the trapping happened
  //  double trappingPosition = electronDriftVelocity*drift_time;
  double trappingPosition_hole = holeDriftVelocity*drift_time;       
  // m_trappingPosition[elementHash] = trappingPosition;
  m_trappingPosition[elementHash] = trappingPosition_hole;

  //-------------------

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "Temperature (C), bias voltage, depletion  voltage: "
                      << temperature - 273.15 << ", "
                      << biasVoltage/CLHEP::volt << ", "
                      << deplVoltage/CLHEP::volt << endmsg;
    msg(MSG::VERBOSE) << "Depletion depth: " << depletionDepth/CLHEP::mm  << endmsg;
    msg(MSG::VERBOSE) << "Electric Field: " << electricField/(CLHEP::volt/CLHEP::mm)  << endmsg;
    msg(MSG::VERBOSE) << "Electron drift mobility (cm2/V/s): " <<  electronDriftMobility/(CLHEP::cm2/ CLHEP::volt/CLHEP::s) << endmsg;
    msg(MSG::VERBOSE) << "Electron drift velocity (cm/s): " <<   electronDriftVelocity << endmsg;
    msg(MSG::VERBOSE) << "Electron mean free path (cm): " <<  m_meanFreePathElectrons[elementHash] << endmsg;
    // msg(MSG::VERBOSE) << "Electron trapping probability: " <<  m_trappingProbability[elementHash] << endmsg;
    msg(MSG::VERBOSE) << "Electron trapping probability: " <<  trappingProbability_electron << endmsg;

    
    if (m_calcHoles) {
      msg(MSG::VERBOSE) << "Hole drift mobility (cm2/V/s): " <<  holeDriftMobility/(CLHEP::cm2/ CLHEP::volt/CLHEP::s) << endmsg;
      msg(MSG::VERBOSE) << "Hole drift velocity (cm/s): " <<   holeDriftVelocity << endmsg;
      msg(MSG::VERBOSE) << "Hole mean free path (cm): " <<  m_meanFreePathHoles[elementHash] << endmsg;
      // msg(MSG::VERBOSE) << "Hole trapping probability: " <<  m_trappingProbability[elementHash] << endmsg;
      msg(MSG::VERBOSE) << "Hole trapping probability: " <<  trappingProbability_hole << endmsg;


    }
  }
}



//-------------------------------------------------------------------------------------------------------------------
//    RAMO POTENTIAL 
//-------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------
//   initialize PotentialValue
//--------------------------------------------------------------
void SCT_ChargeTrappingSvc::initPotentialValue() {
  for (int ix=0 ; ix <81 ; ix++ ){
    for (int iy=0 ; iy<115 ; iy++ ) {
      m_PotentialValue[ix][iy] = GetPotentialValue(ix,iy);
    }
  }
}


//-------------------------------------------------------------------
//    calculation of induced charge using Weighting (Ramo) function
//-------------------------------------------------------------------
double SCT_ChargeTrappingSvc::induced (int istrip, double x, double y)const{
  // x and y are the coorlocation of charge (e or hole)
  // induced chardege on the strip "istrip" situated at the height y = d
  // the center of the strip (istrip=0) is x = 0.004 [cm]
  double deltax = 0.0005, deltay = 0.00025;
  
  double bulk_depth = 0.0285;    // in [cm]
  double strip_pitch = 0.0080;   // in [cm]

  // x is width, y is depth

  if ( y < 0. || y > bulk_depth) return 0;
  double xc = strip_pitch * (istrip + 0.5);
  double dx = fabs( x-xc );
  int ix = int( dx / deltax );
  if ( ix > 79 ) return 0.;
  int iy = int( y  / deltay );
  double fx = (dx - ix*deltax) / deltax;
  double fy = ( y - iy*deltay) / deltay;
  int ix1 = ix + 1;
  int iy1 = iy + 1;
  double P = m_PotentialValue[ix][iy]   *(1.-fx)*(1.-fy)
    + m_PotentialValue[ix1][iy]  *fx*(1.-fy)
    + m_PotentialValue[ix][iy1]  *(1.-fx)*fy
    + m_PotentialValue[ix1][iy1] *fx*fy ;
#ifdef SCT_DIG_DEBUG
  ATH_MSG_DEBUG("induced: x,y,iy="<<x<<" "<<y<<" "<<iy<<" istrip,xc,dx,ix="
                <<istrip<<" "<<xc<<" " <<dx<<" "<<ix<<" fx,fy="<<fx <<" " <<fy<< ", P="<<P);
#endif
  
  return P;
}


//---------------------------------------------------------------------
//  holeTransport
//---------------------------------------------------------------------
void SCT_ChargeTrappingSvc::holeTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double& Q_00, double & Q_p1, double & Q_p2 )const{
  // transport holes in the bulk 
  // T. Kondo, 2010.9.9
  // External parameters to be specified
  //     m_transportTimeMax  [nsec]
  //     m_transportTimeStep [nsec]     
  //     bulk_depth        [cm]
  // Induced currents are added to
  //     Q_m2,Q_m1,Q_00,Q_p1,Q_p2   
  // 
  // initPotentialValue(); // <-this has to go into the main

  // x is width, y is depth

  double x = x0/10.;  // original hole position [cm]
  double y = y0/10.;  // original hole position [cm]
  double qstrip[5];

  for (int istrip = -2 ; istrip < 3 ; istrip++) 
    qstrip[istrip+2] = induced(istrip, x, y);
#ifdef SCT_DIG_DEBUG 
  ATH_MSG_DEBUG("h:qstrip=" << qstrip[0] << " " << qstrip[1] << " " << qstrip[2] << " " << qstrip[3] << " " << qstrip[4]);
#endif
  
  // Get induced current by subtracting induced charges
  for (int istrip = -2 ; istrip < 3 ; istrip++) {
    x = xfin/10.;
    y = yfin/10.;
    double qnew = induced( istrip, x, y);
    int jj = istrip + 2;
    double dq = qnew - qstrip[jj];
    qstrip[jj] = qnew ;
#ifdef SCT_DIG_DEBUG
    ATH_MSG_DEBUG("dq= " << dq);
#endif
    switch(istrip) {
    case -2: Q_m2 += dq ; break;
    case -1: Q_m1 += dq ; break;
    case  0: Q_00 += dq ; break;
    case +1: Q_p1 += dq ; break;
    case +2: Q_p2 += dq ; break;     
    }
  }  
#ifdef SCT_DIG_DEBUG  
  ATH_MSG_DEBUG("h:qstrip=" << qstrip[0] << " " << qstrip[1] << " " << qstrip[2] << " " << qstrip[3] << " " << qstrip[4]);
#endif
  // end of hole tracing 
  //#ifdef SCT_DIG_DEBUG  
  //ATH_MSG_DEBUG("holeTransport : x,y=("<< x0*1.e4<<","<<y0*1.e4<<")->(" << x*1.e4<<"," <<y*1.e4 <<") t="<<t_current);
  //#endif 
  return;
}
