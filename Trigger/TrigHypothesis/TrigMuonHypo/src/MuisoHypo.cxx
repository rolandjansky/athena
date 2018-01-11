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
// PURPOSE:  LVL2 Muon Isolation Hypothesis Algorithm: V4.5
// ********************************************************************
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTrigMuon/L2IsoMuonContainer.h"
#include "TrigMuonHypo/MuisoHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MuisoHypo::MuisoHypo(const std::string & name, ISvcLocator* pSvcLocator):
   HLT::HypoAlgo(name, pSvcLocator)
{

   declareProperty("AcceptAll",            m_acceptAll = false);

   declareProperty("IDConeSize",           m_IDConeSize = 2);   //ID cone size (1: DR=0.1, 2: DR=0.2, 3: DR=0.3: 4: DR=0.4)
   m_IDConeSize.verifier().setBounds(1, 4);

   declareProperty("MaxIDIso_1",           m_MaxIDIso_1 = 0.03);
   declareProperty("MaxIDIso_2",           m_MaxIDIso_2 = 0.03);
   declareProperty("MaxIDIso_3",           m_MaxIDIso_3 = 0.04);

   declareProperty("EtaRegion_1",          m_EtaRegion_1 = 1.0);
   declareProperty("EtaRegion_2",          m_EtaRegion_2 = 1.4);
   declareProperty("EtaRegion_3",          m_EtaRegion_3 = 9.9);

   declareMonitoredVariable("CutCounter",  m_cutCounter);
   declareMonitoredVariable("SumPtCone",   m_SumPtCone);

   m_storeGate = 0;
}

MuisoHypo::~MuisoHypo()
{
}

HLT::ErrorCode MuisoHypo::hltInitialize()
{

   msg() << MSG::INFO << "Initializing " << name() << " - package version " << PACKAGE_VERSION << endreq;

   if (m_acceptAll) {
      msg() << MSG::DEBUG
            << "Accepting all the events with not cut!"
            << endreq;
   }

   msg() << MSG::DEBUG
         << "Initialization completed successfully"
         << endreq;

   return HLT::OK;
}

HLT::ErrorCode MuisoHypo::hltFinalize()
{
   msg() << MSG::DEBUG << "in finalize()" << endreq;
   return HLT::OK;
}

HLT::ErrorCode MuisoHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

   // Retrieve store.
   m_storeGate = store();

   if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endreq;

   m_cutCounter = -1;

   if (m_acceptAll) {
      pass = true;
      if (msgLvl() <= MSG::DEBUG) {
         msg() << MSG::DEBUG
               << "Accept property is set: taking all the events"
               << endreq;
      }
      return HLT::OK;
   }

   bool result = true; //set result to true in case of errors for safety

   // Some debug output:
   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endreq;
   }


   //Retrieve combined muon
   const xAOD::L2IsoMuonContainer* vectorOfMuons;
   HLT::ErrorCode status = getFeature(outputTE, vectorOfMuons);
   if (status != HLT::OK) {
      msg() << MSG::ERROR << " getFeature fails to get the L2IsoMuonContainer " << endreq;
      return status;
   }

   // Check that there is only one L2IsoMuon
   if (vectorOfMuons->size() != 1) {
     msg() << MSG::DEBUG << "Size of L2Iso muon vector is " << vectorOfMuons->size() << ", so isolation failed" << endreq;
     pass = false;
     return HLT::OK;
   }

   // Get first (and only) L2IsoMuon
   const xAOD::L2IsoMuon* pMuonIso = vectorOfMuons->front();
   if (!pMuonIso) {
      msg() << MSG::ERROR << "Retrieval of xAOD::L2IsoMuon from vector failed" << endreq;
      return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::NAV_ERROR);
   }

   // Requirements
   float ptsum  = 0.0;
   if (m_IDConeSize == 1)   ptsum  = pMuonIso->sumPt01();
   else if (m_IDConeSize == 2)   ptsum  = pMuonIso->sumPt02();
   else if (m_IDConeSize == 3)   ptsum  = pMuonIso->sumPt03();
   else if (m_IDConeSize == 4)   ptsum  = pMuonIso->sumPt04();
   else                          ptsum  = pMuonIso->sumPt02();

   float etamu  = pMuonIso->eta();
   float ptmu   = fabs(pMuonIso->pt());

   m_SumPtCone = ptsum / CLHEP::GeV;

   float isoID = 0.0;
   if (ptmu > 0) isoID = ptsum/ptmu;

   if (isoID < 0) {
      if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << " isoID < 0 --> should never happen, set to zero " << endreq;
      isoID = 0.0;
   }

   bool isIsolatedID   = true;
   if (fabs(etamu) <= m_EtaRegion_1) {
      if (isoID > m_MaxIDIso_1.value())       isIsolatedID = false;
      if (isIsolatedID) m_cutCounter = 1;
   } else if (fabs(etamu) > m_EtaRegion_1 && fabs(etamu) <= m_EtaRegion_2) {
      if (isoID > m_MaxIDIso_2.value())       isIsolatedID = false;
      if (isIsolatedID) m_cutCounter = 2;
   } else if (fabs(etamu) > m_EtaRegion_2 && fabs(etamu) <= m_EtaRegion_3) {
      if (isoID > m_MaxIDIso_3.value())       isIsolatedID = false;
      if (isIsolatedID) m_cutCounter = 3;
   }

   result = isIsolatedID;

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " REGTEST pt / eta / SumPtCone / isoID : "
            << " / " << ptmu / CLHEP::GeV
            << " / " << etamu
            << " / " << m_SumPtCone
            << " / " << isoID
            << " / Muon Isolation Hypotesis is " << (result ? "true" : "false")
            << endreq;
   }

   //store result
   pass = result;

   return HLT::OK;
}
