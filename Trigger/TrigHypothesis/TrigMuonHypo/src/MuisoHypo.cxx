/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MuisoHypo.cxx
// PACKAGE:  Trigger/TrigHypotheis/TrigMuonHypo/MuisoHypo
//
// AUTHOR:   S. Giagu <stefano.giagu@cern.ch>
//
// PURPOSE:  LVL2 Muon Isolation Hypothesis Algorithm: V3.5
// ********************************************************************
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigMuonEvent/IsoMuonFeature.h"
#include "TrigMuonHypo/MuisoHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MuisoHypo::MuisoHypo(const std::string & name, ISvcLocator* pSvcLocator):
  HLT::HypoAlgo(name, pSvcLocator){

    declareProperty("AcceptAll",            m_acceptAll = false);

    //Std isolation
    declareProperty("CaloConeSize",         m_CaloConeSize = 2); //Calorimetric cone size (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_CaloConeSize.verifier().setBounds(1,4);
    declareProperty("IDConeSize",           m_IDConeSize = 2);   //ID cone size (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_IDConeSize.verifier().setBounds(1,4);

    declareProperty("UseID",                m_UseID        = true );  //Iso based on ID
    declareProperty("UseAbsID",             m_UseAbsID     = false ); //Absolute ID iso
    declareProperty("UseCalo",              m_UseCalo      = true );  //Iso based on Calo
    declareProperty("UseAbsCalo",           m_UseAbsCalo   = true );  //Absolute Calo iso
    declareProperty("UseIDptForMu",         m_UseIDptForMu = false ); //muon pt is IDmeasured pt

    declareProperty("MaxCaloIso_1",         m_MaxCaloIso_1=4000.0);
    declareProperty("MaxCaloIso_2",         m_MaxCaloIso_2=4000.0);
    declareProperty("MaxCaloIso_3",         m_MaxCaloIso_3=3500.0);

    declareProperty("MaxIDIso_1",           m_MaxIDIso_1=0.03);
    declareProperty("MaxIDIso_2",           m_MaxIDIso_2=0.03);
    declareProperty("MaxIDIso_3",           m_MaxIDIso_3=0.04);

    declareProperty("EtaRegion_1",          m_EtaRegion_1 = 1.0);
    declareProperty("EtaRegion_2",          m_EtaRegion_2 = 1.4);
    declareProperty("EtaRegion_3",          m_EtaRegion_3 = 9.9);

    //Altrenative algorithms (for Exotic signatures)

    //Hollow cones
    declareProperty("UseHollowConeCalo",    m_UseHollowConeCalo = false); //If true use cone_ext-cone_int for isolation (Calo)
    declareProperty("HollowConeCaloInner",  m_HollowConeCaloInner = 2); // (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_HollowConeCaloInner.verifier().setBounds(1,4);
    declareProperty("HollowConeCaloOuter",  m_HollowConeCaloOuter = 4); // (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_HollowConeCaloOuter.verifier().setBounds(1,4);

    declareProperty("UseHollowConeID",      m_UseHollowConeID = false); //If true use cone_ext-cone_int for isolation (ID)
    declareProperty("HollowConeIDInner",    m_HollowConeIDInner = 1); // (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_HollowConeIDInner.verifier().setBounds(1,4);
    declareProperty("HollowConeIDOuter",    m_HollowConeIDOuter = 2); // (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
    m_HollowConeIDOuter.verifier().setBounds(1,4);

    //Subtract muon pt + pt of max pt ID track (different from muon) from ptsum
    declareProperty("RemoveMaxPtTrack",     m_RemoveMaxPtTrack = false);

    //Use max pt ID track for isolation (not associated to the muon)
    declareProperty("UseMaxPtTrackAlgo",    m_UseMaxPtTrackAlgo = false);

    //Don't subtract muon pt from ptsum
    declareProperty("DoNotSubtractMuonPt",  m_DoNotSubtractMuonPt = false);

    declareMonitoredVariable("CutCounter",  m_cutCounter);
    declareMonitoredVariable("SumEtCone",   m_SumEtCone);
    declareMonitoredVariable("SumPtCone",   m_SumPtCone);
    declareMonitoredVariable("StrategyFlag",m_Strategy);

    m_UseCalo = true;
}

MuisoHypo::~MuisoHypo(){
}

HLT::ErrorCode MuisoHypo::hltInitialize(){

  if(m_acceptAll) {
      msg() << MSG::DEBUG
            << "Accepting all the events with not cut!"
	    << endreq;
  } else {
    if (m_UseCalo) {
      msg() << MSG::DEBUG << "Calo based isolation activated" << endreq;
      if (m_UseAbsCalo) msg() << MSG::DEBUG << "Absolute isolation cut for calo selected" << endreq;
      msg() << MSG::DEBUG << "Calorimetric cone size selected: " << m_CaloConeSize << endreq;
    }
    if (m_UseID) {
      msg() << MSG::DEBUG << "ID   based isolation activated" << endreq;
      if (m_UseAbsID) msg() << MSG::DEBUG << "Absolute isolation cut for ID selected" << endreq;
      msg() << MSG::DEBUG << "ID cone size selected: " << m_IDConeSize << endreq;
    }
  
    if (m_UseHollowConeCalo) {
      msg() << MSG::DEBUG << "Hollow Cone algorithm selected for calo isolation" << endreq;
      msg() << MSG::DEBUG << "Outer cone: " << m_HollowConeCaloOuter << endreq;
      msg() << MSG::DEBUG << "Inner cone: " << m_HollowConeCaloInner << endreq;
    }
    if (m_UseHollowConeID) {
      msg() << MSG::DEBUG << "Hollow Cone algorithm selected for ID isolation" << endreq;
      msg() << MSG::DEBUG << "Outer cone: " << m_HollowConeIDOuter << endreq;
      msg() << MSG::DEBUG << "Inner cone: " << m_HollowConeIDInner << endreq;
    }

    if (m_RemoveMaxPtTrack)
      msg() << MSG::DEBUG << "Pt of max pt ID track (different from muon) subtracted from sumpt" << endreq;

    if (m_UseMaxPtTrackAlgo) 
      msg() << MSG::DEBUG << "Max pt ID Algo selected (sumpt == pt of max pt ID track (different from the muon)" << endreq;

    if (m_DoNotSubtractMuonPt)
      msg() << MSG::DEBUG << "Muon pt is not subtracted from ptsum" << endreq;
  }


  msg() << MSG::DEBUG 
        << "Initialization completed successfully" 
        << endreq;
  
  return HLT::OK;
}

HLT::ErrorCode MuisoHypo::hltFinalize(){
  msg() << MSG::DEBUG << "in finalize()" << endreq;
  return HLT::OK;
}

HLT::ErrorCode MuisoHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass){

  // Retrieve store.
  m_storeGate = store();

  if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;

  m_cutCounter = -1;

  if(m_acceptAll) {
      pass = true;
      if(msgLvl() <= MSG::DEBUG) {
	msg() << MSG::DEBUG 
	      << "Accept property is set: taking all the events"
	      << endreq;
      }
      return HLT::OK;
  }

  bool result = true; //set result to true in case of errors for safety
  
  // Some debug output:
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
  }

  // Get vector of pointers to all IsoMuonFeatures linked to the outputTE 
  //   by label "uses":

  std::vector<const IsoMuonFeature*> vectorOfIsoMuons; 
  HLT::ErrorCode status = getFeatures(outputTE, vectorOfIsoMuons);
  if(status!=HLT::OK) {
    msg() << MSG::DEBUG << "no IsoMuonFeatures found" << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::MISSING_FEATURE);
  }

  // Check that there is only one IsoMuonFeature
  if (vectorOfIsoMuons.size() != 1){
    msg() << MSG::ERROR
	  << "Size of vector is " << vectorOfIsoMuons.size() << " while must be 1"
	  << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }

  // Get first (and only) RoI:
  const IsoMuonFeature* pMuonIso = vectorOfIsoMuons.front();
  if(!pMuonIso){
    msg() << MSG::ERROR
	  << "Retrieval of IsoMuonFeature from vector failed"
	  << endreq;
    return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::NAV_ERROR);
  }

  // Requirements
  int  usealgo = pMuonIso->iso_strategy();
  m_Strategy   = usealgo;

  float etsum  = 0.0;
  float ptsum  = 0.0;
  float etsumI = 0.0;
  float ptsumI = 0.0;

  if      (m_CaloConeSize == 1) etsum  = pMuonIso->getsumet01();
  else if (m_CaloConeSize == 2) etsum  = pMuonIso->getsumet02();
  else if (m_CaloConeSize == 3) etsum  = pMuonIso->getsumet03();
  else if (m_CaloConeSize == 4) etsum  = pMuonIso->getsumet04();
  else                          etsum  = pMuonIso->getsumet02();

  if      (m_IDConeSize == 1)   ptsum  = pMuonIso->getsumpt01();
  else if (m_IDConeSize == 2)   ptsum  = pMuonIso->getsumpt02();
  else if (m_IDConeSize == 3)   ptsum  = pMuonIso->getsumpt03();
  else if (m_IDConeSize == 4)   ptsum  = pMuonIso->getsumpt04();
  else                          ptsum  = pMuonIso->getsumpt02();

  if (m_UseHollowConeCalo) {
    if      (m_HollowConeCaloOuter == 1) etsum  = pMuonIso->getsumet01();
    else if (m_HollowConeCaloOuter == 2) etsum  = pMuonIso->getsumet02();
    else if (m_HollowConeCaloOuter == 3) etsum  = pMuonIso->getsumet03();
    else if (m_HollowConeCaloOuter == 4) etsum  = pMuonIso->getsumet04();
    else                                 etsum  = pMuonIso->getsumet02();
    if      (m_HollowConeCaloInner == 1) etsumI = pMuonIso->getsumet01();
    else if (m_HollowConeCaloInner == 2) etsumI = pMuonIso->getsumet02();
    else if (m_HollowConeCaloInner == 3) etsumI = pMuonIso->getsumet03();
    else if (m_HollowConeCaloInner == 4) etsumI = pMuonIso->getsumet04();
    else                                 etsumI = pMuonIso->getsumet02();
    if (m_HollowConeCaloOuter <= m_HollowConeCaloInner) {
      msg() << MSG::ERROR << "CALO Outer cone radius smaller or equal to inner cone one, please check settings!" << endreq;
      etsum  = pMuonIso->getsumet02();
      etsumI = 0.0;
    }
  }

  if (m_UseHollowConeID) {
    if      (m_HollowConeIDOuter == 1) ptsum  = pMuonIso->getsumpt01();
    else if (m_HollowConeIDOuter == 2) ptsum  = pMuonIso->getsumpt02();
    else if (m_HollowConeIDOuter == 3) ptsum  = pMuonIso->getsumpt03();
    else if (m_HollowConeIDOuter == 4) ptsum  = pMuonIso->getsumpt04();
    else                               ptsum  = pMuonIso->getsumpt02();
    if      (m_HollowConeIDInner == 1) ptsumI = pMuonIso->getsumpt01();
    else if (m_HollowConeIDInner == 2) ptsumI = pMuonIso->getsumpt02();
    else if (m_HollowConeIDInner == 3) ptsumI = pMuonIso->getsumpt03();
    else if (m_HollowConeIDInner == 4) ptsumI = pMuonIso->getsumpt04();
    else                               ptsumI = pMuonIso->getsumpt02();
    if (m_HollowConeIDOuter <= m_HollowConeIDInner) {
      msg() << MSG::ERROR << "ID Outer cone radius smaller or equal to inner cone one, please check settings!" << endreq;
      ptsum  = pMuonIso->getsumpt02();
      ptsumI = 0.0;
    }
  }

  if (m_RemoveMaxPtTrack) {
    ptsum -= pMuonIso->getMaxPtID();
  }
     
  if (m_UseMaxPtTrackAlgo) {
    ptsum  = pMuonIso->getMaxPtID();
    ptsumI = 0.0;
  }

  float etamu  = pMuonIso->getEtaMu();
  float ptmuid = fabs(pMuonIso->getPtMuID());
  float ptmu   = fabs(pMuonIso->getPtMu());
  if (m_UseIDptForMu || usealgo != 1) ptmu = ptmuid;
  if (m_DoNotSubtractMuonPt) ptmuid = 0.0;

  float isoID = ptsum-ptsumI-ptmuid;
  m_SumPtCone = isoID/1000.;
  if (!m_UseAbsID) {
   if (ptmu > 0) isoID /= ptmu;
  }
  if (isoID < 0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " isoID < 0 --> should never happen, set to zero " << endreq;
    isoID = 0.0;
  }

  float isoCalo = etsum - etsumI;
  m_SumEtCone = isoCalo/1000.;
  if (!m_UseAbsCalo) {
    if (ptsum > 0) isoCalo /= ptsum;
  }
  if (isoCalo < 0) {
    if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " isoCalo < 0 --> should never happen, set to zero " << endreq;
    isoCalo = 0.0;
  }
 
  bool isIsolatedCalo = true;
  bool isIsolatedID   = true;
  if (fabs(etamu) <= m_EtaRegion_1) {
    if (m_UseCalo) {
      if (isoCalo > m_MaxCaloIso_1.value()) isIsolatedCalo = false;
    }
    if (m_UseID) {
      if (isoID > m_MaxIDIso_1.value())       isIsolatedID = false;
    }
    if (isIsolatedCalo && isIsolatedID) m_cutCounter = 1;
  }
  else if (fabs(etamu) > m_EtaRegion_1 && fabs(etamu) <= m_EtaRegion_2 ) {
    if (m_UseCalo) {
      if (isoCalo > m_MaxCaloIso_2.value()) isIsolatedCalo = false;
    }
    if (m_UseID) {
      if (isoID > m_MaxIDIso_2.value())       isIsolatedID = false;
    }
    if (isIsolatedCalo && isIsolatedID) m_cutCounter = 2;
  }
  else if (fabs(etamu) > m_EtaRegion_2 && fabs(etamu) <= m_EtaRegion_3 ) {
    if (m_UseCalo) {
      if (isoCalo > m_MaxCaloIso_3.value()) isIsolatedCalo = false;
    }
    if (m_UseID) {
      if (isoID > m_MaxIDIso_3.value())       isIsolatedID = false;
    }
    if (isIsolatedCalo && isIsolatedID) m_cutCounter = 3;
  }
 
  result = (isIsolatedCalo && isIsolatedID);

  if (msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST usealgo / eta / SumEtCone / SumPtCone: "
	  << " / " << usealgo
	  << " / " << etamu
	  << " / " << m_SumEtCone
	  << " / " << m_SumPtCone
	  << " / Muon Isolation Hypotesis is " << (result?"true":"false") 
	  << endreq;
  }
  
  //store result
  pass = result;

  return HLT::OK;
}
