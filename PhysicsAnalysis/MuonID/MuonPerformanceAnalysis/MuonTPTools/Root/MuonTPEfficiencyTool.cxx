/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPEfficiencyTool.cxx

#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "xAODMuon/Muon.h"

//**********************************************************************

MuonTPEfficiencyTool::MuonTPEfficiencyTool(std::string myname)
  : AsgTool(myname){
  
  declareProperty("MatchPtCut",     m_matchPtCut     = 0.0); 
  declareProperty("MatchEtaCut",    m_matchEtaCut    = 5.0);
  declareProperty("MaximumDrCut",   m_maximumDrCut   = 0.05);
  declareProperty("MuonAuthor",     m_muonAuthor     = 12); // we need a central place for author<->algname mapping!
  declareProperty("EfficiencyFlag", m_efficiencyFlag = "CBMuons");  
  declareProperty("IsNominal", m_isNominal=true);
  declareProperty("SelectionTool", m_selection_tool);
  declareProperty("RecoScaleFactorTool", m_reco_sf_tool);
  declareProperty("IsolScaleFactorTool", m_isol_sf_tool);
  declareProperty("TrigScaleFactorTool", m_trig_sf_tool);
  declareProperty("IDTrackIsoDecorator", m_IDtrack_isol_tool);
  declareProperty("ApplyScaleFactors", m_do_sf = false);
  declareProperty("TriggerUtils",     m_trigUtils);
  declareProperty("IsoSelectionTool",        m_isolTool);
  declareProperty("TrigItem",          m_trigger_item="");
  declareProperty("RerunMode", m_rerunMode=false);
}

MuonTPEfficiencyTool::~MuonTPEfficiencyTool()
{}

StatusCode MuonTPEfficiencyTool::initialize()
{
  ATH_CHECK(m_selection_tool.retrieve());
  ATH_CHECK(m_trigUtils.retrieve());
  ATH_CHECK(m_IDtrack_isol_tool.retrieve());
  ATH_CHECK(m_isolTool.retrieve());
  ATH_CHECK(m_isolTool->setIParticleCutsFrom(xAOD::Type::Muon));
  if (m_do_sf){
      ATH_CHECK(m_reco_sf_tool.retrieve());
      ATH_CHECK(m_isol_sf_tool.retrieve());
      ATH_CHECK(m_trig_sf_tool.retrieve());
  }

  return StatusCode::SUCCESS;
}

//**********************************************************************

void MuonTPEfficiencyTool::matchProbes(ProbeContainer* ) const
{
    // do nothing by default
}


//**********************************************************************

void MuonTPEfficiencyTool::dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
  // loop over probes
  for(auto probe : *probes) {
    // reset the
    double dRMin = 1;
    Probe* matchProbe = 0;
    const xAOD::Muon* best_match_muon = NULL;
    
    // loop over matches
    for(auto match : *matches) {

      // Pt Cut
      if(match->pt() < m_matchPtCut) continue;

      // Eta Cut
      if(fabs(match->eta()) > m_matchEtaCut) continue;
      
      // Muon author cut (when matching to a xAOD::Muon)
      const xAOD::Muon* mumatch = dynamic_cast<const xAOD::Muon*>(match);
      if(mumatch && m_muonAuthor!=21 && mumatch->author()!=m_muonAuthor) continue; // Assuming here 21 as "all authors"

      // Muon quality cut (when matching to a xAOD::Muon)
      // tba

      // Track quality cut (when matching to a xAOD::TrackParticle)
      // tba

      // Calculate dR
      double dR = deltaR(probe, match);
      if(dR < dRMin) {
        dRMin = dR;
        matchProbe = probe;
        if (mumatch) best_match_muon = mumatch;
      }
    }
    probe->sf_reco(1.);
    // check if a matched probe is found
    probe->isMatched((matchProbe && dRMin<m_maximumDrCut));
    if (m_do_sf && best_match_muon && probe->isMatched()){
      float sf = 1.;
      if (m_reco_sf_tool->getEfficiencyScaleFactor(*best_match_muon,sf) == CP::CorrectionCode::Ok){
        probe->sf_reco(sf);
      }

    }
  }
}
//**********************************************************************

double MuonTPEfficiencyTool::deltaR(Probe* probe, const xAOD::IParticle* match) const
{
  return probe->probeTrack().p4().DeltaR(match->p4());
}
//**********************************************************************

//  check for a trigger match (probe side)
bool MuonTPEfficiencyTool::MatchTrigger (const xAOD::IParticle* match,  std::string trigger) const {
	const xAOD::Muon* matchmu = dynamic_cast <const xAOD::Muon*>(match);
	if (matchmu){
		return m_trigUtils->Trig_Match(*matchmu, trigger);
	}
	// case of ID probes - not likely to be used
	else {
		ATH_MSG_WARNING("Trigger matching to ID tracks not yet supported.");
		return false;
	}
}
