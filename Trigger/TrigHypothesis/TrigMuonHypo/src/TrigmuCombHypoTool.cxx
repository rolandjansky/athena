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
   std::vector<float> defBins;
   defBins.push_back(0);
   defBins.push_back(2.5);
   std::vector<float> defThrs;
   defThrs.push_back(5.83 * CLHEP::GeV);

   declareProperty("PtBins", m_ptBins = defBins);
   declareProperty("PtThresholds", m_ptThresholds = defThrs);

   declareProperty("AcceptAll", m_acceptAll = false);

   // Enable pi/K decay rejection
   declareProperty("ApplyPikCuts",        m_pikCuts = false);
   declareProperty("MaxPtToApplyPik",      m_maxPtToApplyPik = 25.);
   declareProperty("MaxChi2IDPik",         m_chi2MaxID = 3.5);

   // strategy dependent pT cuts
   //0: Std
   //1: simplified match Pt combined
   //2: simplified match Pt ID
   //3: simplified match Pt MU
   //4: simplified match Pt infinite
   declareProperty("ApplyStrategyDependentCuts", m_strategydependent = false);
   declareProperty("StrategyDependentPtCuts",    m_strategyDependentPtCuts);

   std::vector<double> defStrategyDependentPtCuts;
   defStrategyDependentPtCuts.push_back(0.0);     //strategy 1 (in GeV) (if<0 do not apply the standard signature cut)
   defStrategyDependentPtCuts.push_back(0.0);     //strategy 2 (in GeV) 
   defStrategyDependentPtCuts.push_back(0.0);     //strategy 3 (in GeV)
   defStrategyDependentPtCuts.push_back(-3.0);    //strategy 4 (in GeV)
   m_strategyDependentPtCuts.set(defStrategyDependentPtCuts);

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
  auto fexPt	= MonitoredScalar::declare("Pt", -9999.);
  auto ptFL	= MonitoredScalar::declare("PtFL", -9999.);
  auto Strategy	= MonitoredScalar::declare("StrategyFlag", 0);
  auto idEta 	= MonitoredScalar::declare("Eta", -9999.);
  auto idPhi 	= MonitoredScalar::declare("Phi", -9999.);
  auto idZ0 	= MonitoredScalar::declare("Z0", -9999.);
  auto idA0 	= MonitoredScalar::declare("A0", -9999.);

  auto monitorIt = MonitoredScope::declare(m_monTool, fexPt, ptFL, Strategy, 
					   idEta, idPhi, idZ0, idA0);

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

  auto ptValue = pMuon->pt() * pMuon->charge() / CLHEP::GeV;

  fexPt    = ptValue;
  ptFL      = ptValue;
  idEta    = pMuon->eta();
  idPhi    = pMuon->phi();
  int usealgo      = pMuon->strategy();
  float ptresComb = pMuon->sigmaPt() / CLHEP::GeV;
  Strategy  = usealgo;
  ATH_MSG_DEBUG("combined muon pt (GeV)/ sigma_pt (GeV)/ eta / phi / usedalgo: " 
              << fexPt << " (GeV) / " << ptresComb << " (GeV) / " << idEta << " / " << idPhi 
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

  idA0     = pMuon->idTrack()->d0();
  idZ0     = pMuon->idTrack()->z0();

  //Get the Pt cut for that eta bin
  float threshold = (idEta != -9999) ? 0 : 99999999;
  float absEta = fabs(idEta);
  for (std::vector<float>::size_type i = 0; i < m_bins; ++i) {
     if (absEta > m_ptBins[i] && absEta < m_ptBins[i + 1]) threshold = m_ptThresholds[i];
  }

  // Check pt threshold for hypothesis and pi/k rejection cuts,
  // convert units since Muonfeature is in GeV

  //Kpi rejection
  bool pikCut = true;
  if (m_pikCuts && (fabsf(fexPt) < m_maxPtToApplyPik)) {
     if (pMuon->idTrack()->chiSquared() > m_chi2MaxID) pikCut = false;
  }

  //Std Pt cut
  bool stdCut = true;
  if (fabsf(fexPt) <= (threshold / CLHEP::GeV)) stdCut = false;
  ATH_MSG_DEBUG("REGTEST muon pt is " << fexPt
             << " GeV and threshold cut is " << threshold / CLHEP::GeV
             << " GeV and pik_cut is " << (pikCut ? "true" : "false"));

  //Strategy dependent Pt cuts
  bool sdpCut = true;
  if (m_strategydependent && usealgo > 0) {
     if (usealgo >= 1 && usealgo <= 4) {
        double tmpcut = m_strategyDependentPtCuts.value()[usealgo - 1];
        if (std::abs(fexPt) <= std::abs(tmpcut)) sdpCut = false;
        if (tmpcut < 0) stdCut = true; //Do not apply std Pt cut
        ATH_MSG_DEBUG("REGTEST muon pt is " << fexPt << " GeV"
                   << " and threshold for strategy dependent cut is " << tmpcut
                   << " GeV and strategy dependent / std cuts are " << (sdpCut ? "true" : "false") << " / " << (stdCut ? "true" : "false"));
     } else {
        ATH_MSG_DEBUG("usealgo out of range, is: " << usealgo << " while should be in [1, 4]");
     }
  }

  result = stdCut && pikCut && sdpCut;

  if (result) ptFL = -9999.;

  ATH_MSG_DEBUG("REGTEST muon passed pt threshold: " << (stdCut ? "true" : "false")
             << " and pik_cut is " << (pikCut ? "true" : "false")
             << " and strategy dependent cuts is " << (sdpCut ? "true" : "false")
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


