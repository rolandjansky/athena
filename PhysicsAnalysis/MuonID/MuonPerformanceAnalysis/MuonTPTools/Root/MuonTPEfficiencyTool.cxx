/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonTPEfficiencyTool.cxx

#include "MuonTPTools/MuonTPEfficiencyTool.h"
#include "xAODMuon/Muon.h"

//**********************************************************************

MuonTPEfficiencyTool::MuonTPEfficiencyTool(std::string myname)
: AsgTool(myname) {
  
  declareProperty("MatchPtCut",     m_matchPtCut     = 0.0); 
  declareProperty("MatchEtaCut",    m_matchEtaCut    = 5.0);
  declareProperty("dRMatching",     m_dRMatching     = true);
  declareProperty("MaximumDrCut",   m_maximumDrCut   = 0.05);
  declareProperty("MuonAuthor",     m_muonAuthor     = 12); // we need a central place for author<->algname mapping!
  declareProperty("EfficiencyFlag", m_efficiencyFlag = "CBMuons");  
  declareProperty("SelectionTool", m_selection_tool);
  declareProperty("ScaleFactorTool", m_sf_tool);
  declareProperty("ApplyScaleFactors", m_do_sf = false);
}

MuonTPEfficiencyTool::~MuonTPEfficiencyTool()
{}

StatusCode MuonTPEfficiencyTool::initialize()
{
   ATH_CHECK(m_selection_tool.retrieve());
   if (m_do_sf) ATH_CHECK(m_sf_tool.retrieve());

  return StatusCode::SUCCESS;
}


//**********************************************************************

void MuonTPEfficiencyTool::matchProbes(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
  if(m_dRMatching) dRMatching(probes, matches);

  if (msgLvl(MSG::DEBUG)) {
    int nmatched=0;
    for(auto probe : *probes) 
      if(probe->isMatched())
	nmatched++;    
    ATH_MSG_DEBUG("Number of matched probes    : " << nmatched );
  }
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
    probe->sfweight(1.);
    // check if a matched probe is found
    probe->isMatched((matchProbe && dRMin<m_maximumDrCut));
    if (m_do_sf && best_match_muon && probe->isMatched()){
        float sf = 1.;
        if (m_sf_tool->getEfficiencyScaleFactor(*best_match_muon,sf) == CP::CorrectionCode::Ok){
            probe->sfweight(sf);
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

