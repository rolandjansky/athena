/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * JPsiMuonTPEfficiencyTool.cxx
 *
 *  Created on: Sep 01, 2014
 *      Author: Maximiliano Sioli
 */

#include "MuonTPTools/JPsiMuonTPEfficiencyTool.h"

JPsiMuonTPEfficiencyTool::JPsiMuonTPEfficiencyTool(std::string myname)
: AsgTool(myname), MuonTPEfficiencyTool(myname) {
	declareProperty("MatchToAnyMS",m_match_MS=false);
	declareProperty("MatchToCB",m_match_CB=false);
	declareProperty("MatchToMedium",m_match_Medium=false);
	declareProperty("MatchToID",m_match_ID=false);
	declareProperty("MatchToCaloTag",m_match_CaloTag=false);
	declareProperty("IDhitCut",m_do_IDHits=true);

}
void JPsiMuonTPEfficiencyTool::dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
  // loop over probes
  for(auto probe : *probes) {
    double dRMin = 1;
    Probe* matchProbe = 0;

    // loop over matches
    for(auto match : *matches) {

      // Pt Cut
      if(match->pt() < m_matchPtCut) continue;

      // Eta Cut
      if(fabs(match->eta()) > m_matchEtaCut) continue;

      // for ID tracks, we need to do this a different way...
      xAOD::Muon* matchmuon = dynamic_cast <xAOD::Muon*>(match);
      if (matchmuon && m_do_IDHits){
    	  Root::TAccept matchResult = m_selection_tool->accept(*matchmuon);		// we only use the ID cuts for now
    	  if (! matchResult.getCutResult("IDHits")) continue;
      }

      if (!GoodMatchMuonType(match)) continue;

      // Calculate dR
      double dR = deltaR(probe, match);
      if(dR < dRMin) {
		dRMin = dR;
		matchProbe = probe;
      }
    }

    // check if a matched probe is found
    if(matchProbe && dRMin<m_maximumDrCut) matchProbe->isMatched(true);
  }
}
bool JPsiMuonTPEfficiencyTool::GoodMatchMuonType(const xAOD::IParticle* probe) const{

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
        return (mumatch && mumatch->isAuthor(xAOD::Muon::CaloTag) );
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
        Root::TAccept matchResult = m_selection_tool->accept(*mumatch);		// we only use the ID cuts for now
        ok &= mumatch->quality()==xAOD::Muon::Medium;
        return ok;
    }
    return false;
}
