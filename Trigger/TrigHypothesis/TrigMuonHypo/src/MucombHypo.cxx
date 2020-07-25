/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "TrigMuonHypo/MucombHypo.h"

#include "CLHEP/Units/SystemOfUnits.h"

class ISvcLocator;

MucombHypo::MucombHypo(const std::string & name, ISvcLocator* pSvcLocator):
   HLT::HypoAlgo(name, pSvcLocator)
{

   std::vector<float> def_bins;
   def_bins.push_back(0);
   def_bins.push_back(2.5);
   std::vector<float> def_thrs;
   def_thrs.push_back(5.83 * CLHEP::GeV);

   declareProperty("PtBins", m_ptBins = def_bins);
   declareProperty("PtThresholds", m_ptThresholds = def_thrs);

   declareProperty("AcceptAll", m_acceptAll = false);

   // Enable pi/K decay rejection
   declareProperty("Apply_pik_Cuts",        m_pik_cuts = false);
   declareProperty("MaxPtToApply_pik",      m_maxPtToApply_pik = 25.);
   declareProperty("MaxChi2ID_pik",         m_chi2MaxID = 3.5);

   // strategy dependent pT cuts
   //0: Std
   //1: simplified match Pt combined
   //2: simplified match Pt ID
   //3: simplified match Pt MU
   //4: simplified match Pt infinite
   declareProperty("ApplyStrategyDependentCuts", m_strategydependent = false);
   declareProperty("StrategyDependentPtCuts",    m_strategyDependentPtCuts);

   std::vector<double> def_strategyDependentPtCuts;
   def_strategyDependentPtCuts.push_back(0.0);     //strategy 1 (in GeV) (if<0 do not apply the standard signature cut)
   def_strategyDependentPtCuts.push_back(0.0);     //strategy 2 (in GeV)
   def_strategyDependentPtCuts.push_back(0.0);     //strategy 3 (in GeV)
   def_strategyDependentPtCuts.push_back(-3.0);    //strategy 4 (in GeV)
   m_strategyDependentPtCuts.set(def_strategyDependentPtCuts);

   declareMonitoredVariable("Pt",            m_fex_pt);
   declareMonitoredVariable("PtFL",          m_ptFL);
   declareMonitoredVariable("StrategyFlag",  m_Strategy);
   declareMonitoredVariable("Eta",           m_id_eta);
   declareMonitoredVariable("Phi",           m_id_phi);
   declareMonitoredVariable("Z0",            m_id_Z0);
   declareMonitoredVariable("A0",            m_id_A0);

   m_bins = 0;
   m_storeGate = 0;
}

MucombHypo::~MucombHypo()
{
}

HLT::ErrorCode MucombHypo::hltInitialize()
{

   msg() << MSG::INFO << "Initializing " << name() << " - package version "
         << PACKAGE_VERSION << endmsg;

   if (m_acceptAll) {
      msg() << MSG::INFO
            << "Accepting all the events with not cut!"
            << endmsg;
   } else {
      m_bins = m_ptBins.size() - 1;
      if (m_bins != m_ptThresholds.size()) {
         msg() << MSG::INFO << "bad thresholds setup .... exiting!" << endmsg;
         return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }

      for (std::vector<float>::size_type i = 0; i < m_bins; ++i) {

         msg() << MSG::INFO
               << "bin " << m_ptBins[i] << " - " <<  m_ptBins[i + 1]
               << " with Pt Threshold of " << (m_ptThresholds[i]) / CLHEP::GeV
               << " GeV" << endmsg;
      }

   }

   msg() << MSG::INFO
         << "Initialization completed successfully"
         << endmsg;

   return HLT::OK;
}


HLT::ErrorCode MucombHypo::hltFinalize()
{
   msg() << MSG::INFO << "in finalize()" << endmsg;
   return HLT::OK;
}


HLT::ErrorCode  MucombHypo::hltExecute(const HLT::TriggerElement* outputTE, bool& pass)
{

   m_storeGate = store();

   if (msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG << "in execute()" << endmsg;

   if(m_acceptAll) {
     pass = true;
     if(msgLvl() <= MSG::DEBUG) {
       msg() << MSG::DEBUG << "Accept property is set: taking all the events" << endmsg;
     }
     return HLT::OK;
   }

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << "Accept property not set: applying selection!" << endmsg;
   }

   bool result = false;

   // Some debug output:
   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << "outputTE->ID(): " << outputTE->getId() << endmsg;

   //Retrieve combined muon
   const xAOD::L2CombinedMuonContainer* vectorOfMuons;
   HLT::ErrorCode status = getFeature(outputTE, vectorOfMuons);
   if (status != HLT::OK) {
      msg() << MSG::ERROR << " getFeature fails to get the L2CombinedMuonContainer " << endmsg;
      return status;
   }

   // Check that there is only one L2CombinedMuon
   if (vectorOfMuons->size() != 1) {
      msg() << MSG::ERROR << "Size of vector is " << vectorOfMuons->size() << endmsg;
      return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::NAV_ERROR);
   }

   // Get first (and only) L2CombinedMuon
   const xAOD::L2CombinedMuon* pMuon = vectorOfMuons->front();
   if (!pMuon) {
      msg() << MSG::ERROR << "Retrieval of xAOD::L2CombinedMuon from vector failed" << endmsg;
      return HLT::ErrorCode(HLT::Action::CONTINUE, HLT::Reason::NAV_ERROR);
   }

   m_fex_pt    = pMuon->pt() * pMuon->charge() / CLHEP::GeV;
   m_ptFL      = m_fex_pt;
   m_id_eta    = pMuon->eta();
   m_id_phi    = pMuon->phi();
   int usealgo      = pMuon->strategy();
   float ptres_comb = pMuon->sigmaPt() / CLHEP::GeV;
   m_Strategy  = usealgo;

   if (msgLvl() <= MSG::DEBUG)
      msg() << MSG::DEBUG << " combined muon pt (GeV)/ sigma_pt (GeV)/ eta / phi / usedalgo: " << m_fex_pt << " / " << ptres_comb << " / " << m_id_eta << " / " << m_id_phi << " / " << usealgo << endmsg;


   if (pMuon->pt() == 0) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << " L2CombinedMuon pt == 0, empty container -> rejected" << endmsg;
      pass = false;
      return HLT::OK;
   }

   // check the pointers to the L2StandAloneMuon
   if (!pMuon->muSATrack()) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << " L2CombinedMuon has no valid xaOD::L2StandaloneMuon -> rejected" << endmsg;
      pass = false;
      return HLT::OK;
   }

   // check the pointer to the ID track
   if (!pMuon->idTrack()) {
      if (msgLvl() <= MSG::DEBUG)
         msg() << MSG::DEBUG << " L2CombinedMuon has no valid xAOD:TrackParticle IDtrack -> rejected" << endmsg;
      pass = false;
      return HLT::OK;
   }

   m_id_A0     = pMuon->idTrack()->d0();
   m_id_Z0     = pMuon->idTrack()->z0();

   //Get the Pt cut for that eta bin
   float threshold = (m_id_eta != -9999) ? 0 : 999999;
   float absEta = fabs(m_id_eta);
   for (std::vector<float>::size_type i = 0; i < m_bins; ++i)
      if (absEta > m_ptBins[i] && absEta < m_ptBins[i + 1]) threshold = m_ptThresholds[i];

   // Check pt threshold for hypothesis and pi/k rejection cuts,
   // convert units since Muonfeature is in GeV

   //Kpi rejection
   bool pik_cut = true;
   if (m_pik_cuts && (fabsf(m_fex_pt) < m_maxPtToApply_pik)) {
      if (pMuon->idTrack()->chiSquared() > m_chi2MaxID) pik_cut = false;
   }

   //Std Pt cut
   bool std_cut = true;
   if (fabsf(m_fex_pt) <= (threshold / CLHEP::GeV)) std_cut = false;
   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " REGTEST muon pt is " << m_fex_pt
            << " GeV and threshold cut is " << threshold / CLHEP::GeV
            << " GeV and pik_cut is " << (pik_cut ? "true" : "false") << endmsg;
   }

   //Strategy dependent Pt cuts
   bool sdp_cut = true;
   if (m_strategydependent && usealgo > 0) {
      if (usealgo >= 1 && usealgo <= 4) {
         double tmpcut = m_strategyDependentPtCuts.value()[usealgo - 1];
         if (std::abs(m_fex_pt) <= std::abs(tmpcut)) sdp_cut = false;
         if (tmpcut < 0) std_cut = true; //Do not apply std Pt cut
         if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << " REGTEST muon pt is " << m_fex_pt << " GeV"
                  << " and threshold for strategy dependent cut is " << tmpcut
                  << " GeV and strategy dependent / std cuts are " << (sdp_cut ? "true" : "false") << " / " << (std_cut ? "true" : "false") << endmsg;
         }
      } else {
         if (msgLvl() <= MSG::DEBUG) {
            msg() << MSG::DEBUG << "usealgo out of range, is: " << usealgo << " while should be in [1, 4]" << endmsg;
         }
      }
   }

   result = std_cut && pik_cut && sdp_cut;

   if (result) m_ptFL = -9999.;

   if (msgLvl() <= MSG::DEBUG) {
      msg() << MSG::DEBUG << " REGTEST muon passed pt threshold: " << (std_cut ? "true" : "false")
            << " and pik_cut is " << (pik_cut ? "true" : "false")
            << " and strategy dependent cuts is " << (sdp_cut ? "true" : "false")
            << " so hypothesis is " << (result ? "true" : "false") << endmsg;
   }

   //store the result
   pass = result;

   return HLT::OK;
}
