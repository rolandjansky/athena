/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigMuonHypo/MuonRoiFex.h"

class ISvcLocator;

// ================================================================================
// ================================================================================

MuonRoiFex::MuonRoiFex(const std::string & name, ISvcLocator* pSvcLocator):
   HLT::FexAlgo(name, pSvcLocator)
{
   declareProperty("AcceptAll",            m_modeAcceptAll        =false);
   declareProperty("RpcOnly",              m_modeRpcOnly          =true);
   declareProperty("RpcHorizontal",        m_modeRpcHorizontal    =false);
   declareProperty("dPhiForRpcHorizontal", m_dPhiForRpcHorizontal =0.78);
}

MuonRoiFex::~MuonRoiFex(){
}

// ================================================================================
// ================================================================================

HLT::ErrorCode MuonRoiFex::hltInitialize()
{
   msg() << MSG::INFO << "Initializing " << name() << " - package version " 
	 << PACKAGE_VERSION << endmsg;

   if( m_modeAcceptAll )     msg() << MSG::INFO << "AcceptAll mode"     << endmsg;
   if( m_modeRpcOnly )       msg() << MSG::INFO << "RpcOnly mode"       << endmsg;
   if( m_modeRpcHorizontal ) {
      msg() << MSG::INFO << "RpcHorizontal mode" << endmsg;
      msg() << MSG::INFO << "...dPhiForRpcHorizontal=" << m_dPhiForRpcHorizontal << endmsg;
   }
 
   msg() << MSG::INFO << "Initialization completed successfully"  << endmsg; 
   
   return HLT::OK;
}

// ================================================================================
// ================================================================================

HLT::ErrorCode MuonRoiFex::hltFinalize()
{
   msg() << MSG::INFO << "in finalize()" << endmsg;
   return HLT::OK;
}

// ================================================================================
// ================================================================================

HLT::ErrorCode MuonRoiFex::hltExecute(const HLT::TriggerElement* te_in,
				      HLT::TriggerElement* te_out)
{
   if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;

   // AcceptAll mode
   if( m_modeAcceptAll ) {
      if(msgLvl() <= MSG::DEBUG) {
	 msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endmsg;
      }
      return HLT::OK;
   }

   // get RecMuonRoI
   const LVL1::RecMuonRoI *pMuonRoI = 0;
   std::vector<const LVL1::RecMuonRoI*> vectorOfRecMuonRoI;   
   HLT::ErrorCode sc = getFeatures(te_in, vectorOfRecMuonRoI, "");  
   if( sc == HLT::OK && vectorOfRecMuonRoI.size() == 1 ) {
      msg() << MSG::DEBUG << "getFeature(RecMuonRoI) - ok" << endmsg;      
      pMuonRoI = vectorOfRecMuonRoI.front();
   }
   else {
      msg() << MSG::DEBUG << "getFeature(RecMuonRoI) failed" << endmsg;      
      msg() << MSG::DEBUG << "...getFeature status=" << sc << endmsg;      
      msg() << MSG::DEBUG << "...vectorOfRecMuonRoI size=" << vectorOfRecMuonRoI.size() << endmsg;      
   }

   const unsigned int SYSID_BARREL = 0; // 0:barrel 1:endcap 2:forward

   // RpcOnly mode
   if( m_modeRpcOnly ) {
      bool pass = false;
      if( pMuonRoI==0 ) {  
	 msg() << MSG::DEBUG << "no RecMuonRoI, reject it" << endmsg;
      }
      else {
	 unsigned int sysid = pMuonRoI->sysID();
	 if( sysid == SYSID_BARREL ) pass = true;
	 if(msgLvl() <= MSG::DEBUG) {
	    double eta = pMuonRoI->eta();  
	    double phi = pMuonRoI->phi();  
	    msg() << MSG::DEBUG << "MuonRecRoI sysid/eta/phi=" << sysid << "/" << eta << "/" << phi << endmsg;
	 }
      }
      msg() << MSG::DEBUG << "--> pass=" << pass << endmsg;
      if( ! pass ) te_out->setActiveState(false);
      return HLT::OK;
   }

   // RpcHorizontal mode
   if( m_modeRpcHorizontal ) {
      bool pass = false; // start from false and enable later
      if( pMuonRoI==0 ) {  
	 msg() << MSG::DEBUG << "no RecMuonRoI, reject it" << endmsg;
      }
      else {
	 unsigned int sysid = pMuonRoI->sysID();
	 double eta = pMuonRoI->eta();  
	 double phi = pMuonRoI->phi();  
	 msg() << MSG::DEBUG << "MuonRecRoI sysid/eta/phi=" << sysid << "/" << eta << "/" << phi << endmsg;
	 if( sysid == SYSID_BARREL ) {
	    if( fabs(phi) < m_dPhiForRpcHorizontal ) pass = true;
	    if( fabs(phi) >=0 && fabs(M_PI-phi) < m_dPhiForRpcHorizontal/2 ) pass = true;
	    if( fabs(phi) <=0 && fabs(M_PI+phi) < m_dPhiForRpcHorizontal/2 ) pass = true;
	 }
      }
      msg() << MSG::DEBUG << "--> pass=" << pass << endmsg;
      if( ! pass ) te_out->setActiveState(false);
      return HLT::OK;
   }

   // if no mode --> error
   msg() << MSG::WARNING << "internal error - mode is strange" << endmsg;   
   return HLT::OK;
}

// ================================================================================
// ================================================================================
