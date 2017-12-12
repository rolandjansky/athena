/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <math.h>
#include <algorithm>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaMonitoring/MonitoredScope.h"

#include "DecisionHandling/TrigCompositeUtils.h"
#include "TrigMuonHypo/TrigmuCombHypoTool.h"

using namespace TrigCompositeUtils;
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TrigmuCombHypoTool::TrigmuCombHypoTool(const std::string& type, 
				       const std::string & name,
				       const IInterface* parent )
   : AthAlgTool( type, name, parent ),
     m_decisionId( HLT::Identifier::fromToolName( name ) ) 
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

   m_bins = 0;
}

TrigmuCombHypoTool::~TrigmuCombHypoTool(){
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigmuCombHypoTool::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);

  ATH_MSG_DEBUG( "Tool configured for chain/id: " << m_decisionId );

  if (m_acceptAll) {
     ATH_MSG_INFO("Accepting all the events with not cut!");
  } else {
     m_bins = m_ptBins.size() - 1;
     if (m_bins != m_ptThresholds.size()) {
        ATH_MSG_INFO("bad thresholds setup .... exiting!");
        return StatusCode::FAILURE;
     }

     for (std::vector<float>::size_type i = 0; i < m_bins; ++i) {

        ATH_MSG_INFO("bin " << m_ptBins[i] << " - " <<  m_ptBins[i + 1]
           	  << " with Pt Threshold of " << (m_ptThresholds[i]) / CLHEP::GeV << " GeV");
     }

  }

  if ( not m_monTool.name().empty() ) {
     ATH_CHECK( m_monTool.retrieve() );
     ATH_MSG_DEBUG("MonTool name: " << m_monTool);
  }

  ATH_MSG_INFO("Initializing" << name() << "successfully");

  return StatusCode::SUCCESS;
}

StatusCode TrigmuCombHypoTool::decide(TrigmuCombHypoTool::CombinedMuonInfo& input) const
{
  ATH_MSG_DEBUG("StatusCode decide() start");

  using namespace Monitored;

  // defined Monitoring variables
  auto fex_pt	= MonitoredScalar::declare("Pt", -9999.);
  auto ptFL	= MonitoredScalar::declare("PtFL", -9999.);
  auto Strategy	= MonitoredScalar::declare("StrategyFlag", 0);
  auto id_eta 	= MonitoredScalar::declare("Eta", -9999.);
  auto id_phi 	= MonitoredScalar::declare("Phi", -9999.);
  auto id_Z0 	= MonitoredScalar::declare("Z0", -9999.);
  auto id_A0 	= MonitoredScalar::declare("A0", -9999.);

  auto monitorIt = MonitoredScope::declare(m_monTool, fex_pt, ptFL, Strategy, 
					   id_eta, id_phi, id_Z0, id_A0);

  bool result = false;

  if(m_acceptAll) {
    result = true;
    ATH_MSG_DEBUG("Accept property is set: taking all the events");
    return StatusCode(result);
  } else {
    result = false;
    ATH_MSG_DEBUG("Accept property not set: applying selection!");
  }

  //Retrieve combined muon
  //xAOD::L2CombinedMuon
  auto pMuon = input.cluster;
  if (!pMuon) {
     ATH_MSG_ERROR("Retrieval of xAOD::L2CombinedMuon from vector failed");
     return StatusCode::FAILURE;
  }

  auto pt_value = pMuon->pt() * pMuon->charge() / CLHEP::GeV;

  fex_pt    = pt_value;
  ptFL      = pt_value;
  id_eta    = pMuon->eta();
  id_phi    = pMuon->phi();
  int usealgo      = pMuon->strategy();
  float ptres_comb = pMuon->sigmaPt() / CLHEP::GeV;
  Strategy  = usealgo;
  ATH_MSG_DEBUG("combined muon pt (GeV)/ sigma_pt (GeV)/ eta / phi / usedalgo: " 
              << fex_pt << " (GeV) / " << ptres_comb << " (GeV) / " << id_eta << " / " << id_phi 
              << " / " << usealgo);

  if (pMuon->pt() == 0) {
     ATH_MSG_DEBUG("L2CombinedMuon pt == 0, empty container -> rejected");
     result = false;
     return StatusCode(result);
  }

  // check the pointers to the L2StandAloneMuon
  if (!pMuon->muSATrack()) {
     ATH_MSG_DEBUG("L2CombinedMuon has no valid xaOD::L2StandaloneMuon -> rejected");
     result = false;
     return StatusCode(result);
  }

  // check the pointer to the ID track
  if (!pMuon->idTrack()) {
     ATH_MSG_DEBUG("L2CombinedMuon has no valid xAOD:TrackParticle IDtrack -> rejected");
     result = false;
     return StatusCode(result);
  }

  id_A0     = pMuon->idTrack()->d0();
  id_Z0     = pMuon->idTrack()->z0();

  //Get the Pt cut for that eta bin
  float threshold = (id_eta != -9999) ? 0 : 99999999;
  float absEta = fabs(id_eta);
  for (std::vector<float>::size_type i = 0; i < m_bins; ++i) {
     if (absEta > m_ptBins[i] && absEta < m_ptBins[i + 1]) threshold = m_ptThresholds[i];
  }

  // Check pt threshold for hypothesis and pi/k rejection cuts,
  // convert units since Muonfeature is in GeV

  //Kpi rejection
  bool pik_cut = true;
  if (m_pik_cuts && (fabsf(fex_pt) < m_maxPtToApply_pik)) {
     if (pMuon->idTrack()->chiSquared() > m_chi2MaxID) pik_cut = false;
  }

  //Std Pt cut
  bool std_cut = true;
  if (fabsf(fex_pt) <= (threshold / CLHEP::GeV)) std_cut = false;
  ATH_MSG_DEBUG("REGTEST muon pt is " << fex_pt
             << " GeV and threshold cut is " << threshold / CLHEP::GeV
             << " GeV and pik_cut is " << (pik_cut ? "true" : "false"));

  //Strategy dependent Pt cuts
  bool sdp_cut = true;
  if (m_strategydependent && usealgo > 0) {
     if (usealgo >= 1 && usealgo <= 4) {
        double tmpcut = m_strategyDependentPtCuts.value()[usealgo - 1];
        if (std::abs(fex_pt) <= std::abs(tmpcut)) sdp_cut = false;
        if (tmpcut < 0) std_cut = true; //Do not apply std Pt cut
        ATH_MSG_DEBUG("REGTEST muon pt is " << fex_pt << " GeV"
                   << " and threshold for strategy dependent cut is " << tmpcut
                   << " GeV and strategy dependent / std cuts are " << (sdp_cut ? "true" : "false") << " / " << (std_cut ? "true" : "false"));
     } else {
        ATH_MSG_DEBUG("usealgo out of range, is: " << usealgo << " while should be in [1, 4]");
     }
  }

  result = std_cut && pik_cut && sdp_cut;

  if (result) ptFL = -9999.;

  ATH_MSG_DEBUG("REGTEST muon passed pt threshold: " << (std_cut ? "true" : "false")
             << " and pik_cut is " << (pik_cut ? "true" : "false")
             << " and strategy dependent cuts is " << (sdp_cut ? "true" : "false")
             << " so hypothesis is " << (result ? "true" : "false"));

  return StatusCode(result);
}

// from TrigmuCombHypoTool 
StatusCode TrigmuCombHypoTool::decide(std::vector<TrigmuCombHypoTool::CombinedMuonInfo>& hypoToolInput) const
{
  ATH_MSG_DEBUG("Applying selection of single << " << m_decisionId.numeric());

  for ( auto& i: hypoToolInput) {
    if (TrigCompositeUtils::passed(m_decisionId.numeric(), i.previousDecisionIDs)) {
      if ( decide(i) ) {
        ATH_MSG_DEBUG("hypothesis is true, so object passed selection");
        TrigCompositeUtils::addDecisionID(m_decisionId, i.decision);
      } else {
        ATH_MSG_DEBUG("hypothesis is false, so no objest passed selection");
      } // End of TrigMufastHypoTool decide()
    } 
  } // End of MuonClusterInfo loops 

  return StatusCode::SUCCESS;
}
// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------


