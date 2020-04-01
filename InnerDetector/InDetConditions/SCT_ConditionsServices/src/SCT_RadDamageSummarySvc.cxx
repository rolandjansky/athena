/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RadDamageSummarySvc.h"

#include "StoreGate/StoreGate.h"

#include "InDetIdentifier/SCT_ID.h"

#include "SCT_ChargeTrappingSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"



// -- constructor
SCT_RadDamageSummarySvc::SCT_RadDamageSummarySvc(const std::string& name, ISvcLocator* sl) 
  : 
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_SCT_ID(0),
  m_chargeTrappingSvc("SCT_ChargeTrappingSvc",name),
  m_useTrapping(true)
{
  
  declareProperty("SCT_ChargeTrappingSvc",m_chargeTrappingSvc);
  declareProperty("UseChargeTrappingSvc",m_useTrapping, "Set to true to switch on charge trapping");
  declareProperty("DoChargeTrapping",m_doCTrap=false, "Do charge trapping");
  declareProperty("StoreGateSvc", m_detStore);
}

// -- destructor
SCT_RadDamageSummarySvc::~SCT_RadDamageSummarySvc(){}


//-- initialize
StatusCode SCT_RadDamageSummarySvc::initialize(){
  msg(MSG::INFO) << "Initializing SCT_RadDamageSummarySvc" << endmsg;
  
  StatusCode sc = setProperties();
  if( !sc.isSuccess() ){
    msg(MSG::FATAL) << "Unable to set properties" << endmsg;
    return StatusCode::FAILURE;
  }
  
  sc = m_detStore.retrieve();
  if( !sc.isSuccess() ){
    msg(MSG::FATAL) << "Unable to retrieve detector store" << endmsg;
    return StatusCode::FAILURE;
  }
    
  if(m_useTrapping){
    if (StatusCode::SUCCESS!=m_chargeTrappingSvc.retrieve()) {
      msg(MSG::FATAL) << "Unable to retrieve SCT_ChargeTrappingSvc" << endmsg;
      return StatusCode::FAILURE;
    } 
    msg(MSG::INFO) << "SCT_ChargeTrappingSvc retrieved" << endmsg;
  }
  else{
    msg(MSG::DEBUG) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  
  sc = m_detStore->retrieve( m_SCT_ID, "SCT_ID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve SCT_ID helper" );
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
}


// -- finalize
StatusCode SCT_RadDamageSummarySvc::finalize(){
  msg(MSG::INFO) << "Finalizing SCT_RadDamageSummarySvc" << endmsg;
  
  return StatusCode::SUCCESS;
}


StatusCode SCT_RadDamageSummarySvc::queryInterface(const InterfaceID& riid, void** ppvIf){
 if(ISCT_RadDamageSummarySvc::interfaceID() == riid){
   *ppvIf = dynamic_cast< ISCT_RadDamageSummarySvc* > (this);
 }
 else if(interfaceID() == riid){
   *ppvIf = dynamic_cast< SCT_RadDamageSummarySvc* > (this);
 }
 else{
   return AthService::queryInterface(riid, ppvIf);
 }

 addRef();
 return StatusCode::SUCCESS;
}


bool SCT_RadDamageSummarySvc::doCTrap(const IdentifierHash & elementHash, const double & zpos){
  
  m_doCTrap = false;
  if (m_useTrapping){
    m_doCTrap =  m_chargeTrappingSvc->getdoCTrap(elementHash, zpos);
  }
  return m_doCTrap;
}


// -- Charge trapping probability
double SCT_RadDamageSummarySvc::ChargeTrappingProbability(const IdentifierHash & elementHash, const double & zpos){

  double prob = 0.0;
  if (m_useTrapping){
    prob = m_chargeTrappingSvc->getTrappingProbability(elementHash, zpos);
  }
  return prob;
}

// -- Trapping Constant
double SCT_RadDamageSummarySvc::TrappingConstant(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getTrappingHoles(elementHash, zpos);              // Mod
  }
  return k;
}


// -- Electric Field
double SCT_RadDamageSummarySvc::ElectricField(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getElectricField(elementHash, zpos);
  }
  return k;
}

// -- Trapping time: Time at which the trapping happened
double SCT_RadDamageSummarySvc::TrappingTime(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getTrappingTime(elementHash, zpos);
  }
  return k;
}

// -- Time to electrode: Time that the charge needs to arrive to the electrode
double SCT_RadDamageSummarySvc::TimeToElectrode(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getTimeToElectrode(elementHash, zpos);
  }
  return k;
}

// -- Trapping position: position at which the trapping happened
double SCT_RadDamageSummarySvc::TrappingPositionZ(const IdentifierHash & elementHash, const double & zpos){
  
  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getTrappingPositionZ(elementHash, zpos);
  }
  return k;
}

// -- Hole drift mobility
double SCT_RadDamageSummarySvc::HoleDriftMobility(const IdentifierHash & elementHash, const double & zpos){
  
  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getHoleDriftMobility(elementHash, zpos);
  }
  return k;
}


// -- electron drift mobility
double SCT_RadDamageSummarySvc::ElectronDriftMobility(const IdentifierHash & elementHash, const double & zpos){
  
  double k = 0.0;
  if (m_useTrapping){
    k = m_chargeTrappingSvc->getElectronDriftMobility(elementHash, zpos);
  }
  return k;
}


//------------------------------------------------
// -- Ramo Potential
//-----------------------------------------------
void SCT_RadDamageSummarySvc::InitPotentialValue() {
  if (m_useTrapping)
    m_chargeTrappingSvc->getInitPotentialValue(); 
  return;
}

void SCT_RadDamageSummarySvc::HoleTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const{
  if (m_useTrapping)
    m_chargeTrappingSvc->getHoleTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2 );
  return;
}


void SCT_RadDamageSummarySvc::ElectronTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const{  
  if (m_useTrapping)
    m_chargeTrappingSvc->getElectronTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2 );
  return;
}



////
