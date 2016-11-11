/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_RadDamageSummarySvc.h"

#include "StoreGate/StoreGate.h"

#include "InDetIdentifier/SCT_ID.h"


// #include "SCT_RamoPotentialSvc.h"
#include "SCT_ChargeTrappingSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"



// -- constructor
SCT_RadDamageSummarySvc::SCT_RadDamageSummarySvc(const std::string& name, ISvcLocator* sl) 
  : 
  AthService(name, sl),
  m_detStore("DetectorStore", name),
  m_SCT_ID(0),
  // m_ramoPotentialSvc("RamoPotentialSvc",name),
  m_chargeTrappingSvc("SCT_ChargeTrappingSvc",name),
  // m_useRamo(true),
  m_useTrapping(true)
{
  
  // declareProperty("RamoPotentialSvc",m_ramoPotentialSvc);
  declareProperty("SCT_ChargeTrappingSvc",m_chargeTrappingSvc);
  // declareProperty("UseRamoPotentialSvc",m_useRamo, "Set to true to use the Ramo Potential");
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
  
  // if(m_useRamo){
    // if (StatusCode::SUCCESS!=m_ramoPotentialSvc.retrieve()) {
      // msg(MSG::FATAL) << "Unable to retrieve RamoPotentialSvc" << endmsg;
      // return StatusCode::FAILURE;
    // } 
    // msg(MSG::INFO) << "RamoPotentialSvc retrieved" << endmsg;
  // }
  
  if(m_useTrapping){
    if (StatusCode::SUCCESS!=m_chargeTrappingSvc.retrieve()) {
      msg(MSG::FATAL) << "Unable to retrieve SCT_ChargeTrappingSvc" << endmsg;
      return StatusCode::FAILURE;
    } 
    msg(MSG::INFO) << "SCT_ChargeTrappingSvc retrieved" << endmsg;
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



// // -- Ramo potential 
// double RadDamageSummarySvc::RamoPotential(const IdentifierHash & elementHash, const Hep3Vector & pos){
 
  // double ramo = 0.0;
  // if (!m_useRamo){
    // //    msg(MSG::INFO) << "RamoPotentialSvc is not requested. Returning default value." << endmsg;
  // }
  // else{
    // //    msg(MSG::INFO) << "RamoPotentialSvc was requested. This may slow significantly your digitization." << endmsg;
    // ramo = m_ramoPotentialSvc->getRamoPotential(elementHash, pos);
  // }
  // return ramo;
// }

bool SCT_RadDamageSummarySvc::doCTrap(const IdentifierHash & elementHash, const double & zpos){
  
  m_doCTrap = false;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    m_doCTrap =  m_chargeTrappingSvc->getdoCTrap(elementHash, zpos);
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  return m_doCTrap;
}


// -- Charge trapping probability
double SCT_RadDamageSummarySvc::ChargeTrappingProbability(const IdentifierHash & elementHash, const double & zpos){

  double prob = 0.0;
  if (!m_useTrapping){
    //    msg(MSG::INFO) << "ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing ChargeTrappingSvc." << endmsg;
    prob = m_chargeTrappingSvc->getTrappingProbability(elementHash, zpos);
  }
  return prob;
}

// -- Trapping Constant
double SCT_RadDamageSummarySvc::TrappingConstant(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    k = m_chargeTrappingSvc->getTrappingHoles(elementHash, zpos);              // Mod
  }

  return k;
}


// -- Electric Field
double SCT_RadDamageSummarySvc::ElectricField(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    k = m_chargeTrappingSvc->getElectricField(elementHash, zpos);
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return k;
}

// -- Trapping time: Time at which the trapping happened
double SCT_RadDamageSummarySvc::TrappingTime(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    k = m_chargeTrappingSvc->getTrappingTime(elementHash, zpos);
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return k;
}

// -- Time to electrode: Time that the charge needs to arrive to the electrode
double SCT_RadDamageSummarySvc::TimeToElectrode(const IdentifierHash & elementHash, const double & zpos){

  double k = 0.0;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    k = m_chargeTrappingSvc->getTimeToElectrode(elementHash, zpos);
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return k;
}

// -- Trapping position: position at which the trapping happened
double SCT_RadDamageSummarySvc::TrappingPositionZ(const IdentifierHash & elementHash, const double & zpos){
  
  double k = 0.0;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    k = m_chargeTrappingSvc->getTrappingPositionZ(elementHash, zpos);
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return k;
}

// -- Hole drift mobility
double SCT_RadDamageSummarySvc::HoleDriftMobility(const IdentifierHash & elementHash, const double & zpos){
  
  double k = 0.0;
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    k = m_chargeTrappingSvc->getHoleDriftMobility(elementHash, zpos);
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return k;
}
//------------------------------------------------
// -- Ramo Potential
//-----------------------------------------------
void SCT_RadDamageSummarySvc::InitPotentialValue() {
    
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    m_chargeTrappingSvc->getInitPotentialValue(); 
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return;
}


void SCT_RadDamageSummarySvc::HoleTransport(double & x0, double & y0, double & xfin, double & yfin, double & Q_m2, double & Q_m1, double & Q_00, double & Q_p1, double & Q_p2 )const{  
    
  if (!m_useTrapping){
    //  msg(MSG::INFO) << "SCT_ChargeTrappingSvc is not requested. Returning default value." << endmsg;
  }
  else{
    //    msg(MSG::INFO) << "Accessing SCT_ChargeTrappingSvc." << endmsg;
    m_chargeTrappingSvc->getHoleTransport(x0, y0, xfin, yfin, Q_m2, Q_m1, Q_00, Q_p1, Q_p2 );
  }
  //  msg(MSG::WARNING) << "Electric Field = " << k << endmsg;
  
  return;
}


////
