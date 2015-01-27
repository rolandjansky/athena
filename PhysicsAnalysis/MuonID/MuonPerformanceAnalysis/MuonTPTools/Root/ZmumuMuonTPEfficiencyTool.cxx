/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPEfficiencyTool.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/ZmumuMuonTPEfficiencyTool.h"

ZmumuMuonTPEfficiencyTool::ZmumuMuonTPEfficiencyTool(std::string myname)
: AsgTool(myname), MuonTPEfficiencyTool(myname) {
	declareProperty("MatchToAnyMS",m_match_MS=false);
	declareProperty("MatchToCB",m_match_CB=false);
	declareProperty("MatchToMedium",m_match_Medium=false);
	declareProperty("MatchToID",m_match_ID=false);
	declareProperty("MatchToCaloTag",m_match_CaloTag=false);
	declareProperty("IDhitCut",m_do_IDHits=true);

}
void ZmumuMuonTPEfficiencyTool::dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
  // loop over probes

  for(auto probe : *probes) {
    double dRMin = 1;
    Probe* matchProbe = 0;
    const xAOD::Muon* best_match_muon = NULL;

    // loop over matches
    for(auto match : *matches) {
      // Pt Cut
      if(match->pt() < m_matchPtCut) continue;

      // Eta Cut
      if(fabs(match->eta()) > m_matchEtaCut) continue;

      if (!GoodMatchMuonType(match)) continue;

      xAOD::Muon* matchmuon = dynamic_cast <xAOD::Muon*>(match);
      if (matchmuon && m_do_IDHits){
    	  if (!m_selection_tool->passedIDCuts(*matchmuon)) continue;
      }
      // ID tracks

      if (m_match_ID){
          xAOD::TrackParticle *trk = dynamic_cast<xAOD::TrackParticle*>(match);
          if(trk && m_do_IDHits && !m_selection_tool->passedIDCuts(*trk)) continue;
      }

      // Calculate dR
      double dR = deltaR(probe, match);
      if(dR < dRMin) {
		dRMin = dR;
		matchProbe = probe;
        if (matchmuon) best_match_muon = matchmuon;
      }
    }

    // check if a matched probe is found
    probe->sfweight(1.);
    probe->isMatched(matchProbe && dRMin<m_maximumDrCut);
    if (m_do_sf && best_match_muon && probe->isMatched()){
        float sf = 1.;
        if (m_sf_tool->getEfficiencyScaleFactor(*best_match_muon,sf) == CP::CorrectionCode::Ok){
            probe->sfweight(sf);
        }
    }
  }
}
bool ZmumuMuonTPEfficiencyTool::GoodMatchMuonType(const xAOD::IParticle* probe) const{

    const xAOD::Muon* mumatch = dynamic_cast <const xAOD::Muon*> (probe);

    // if the particle is not a muon, assume that the user knows what TrackParticleContainer he is providing!
    if (! mumatch) return true;

    // otherwise, we have to manually pick the right probe

    // ID Probe
    if (m_match_ID){
        return (mumatch->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) != NULL);
    }
    // CT Probe

    if (m_match_CaloTag){
        return (mumatch &&  m_selection_tool->passedCaloTagQuality(*mumatch) &&  mumatch->isAuthor(xAOD::Muon::CaloTag) );
    }
    // MS Probe
    if (m_match_MS){
        return ((mumatch->muonType() == xAOD::Muon::MuonStandAlone || mumatch->muonType() == xAOD::Muon::Combined) && mumatch->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)!= NULL);
    }
    // CB probe
    if (m_match_CB){
        return (mumatch->muonType() == xAOD::Muon::Combined && mumatch->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL);
    }
    if (m_match_Medium){
        bool ok = (mumatch->muonType() == xAOD::Muon::Combined && mumatch->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL );
        ok &= mumatch->quality()==xAOD::Muon::Medium;
        return ok;
    }
    return false;
}
