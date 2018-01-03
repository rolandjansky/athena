/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * ZmumuMuonTPEfficiencyTool.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"

MuonRecoTPEfficiencyTool::MuonRecoTPEfficiencyTool(std::string myname)
: MuonTPEfficiencyTool(myname) {
	declareProperty("ptrMatching",    m_ptrMatching    = false);
	declareProperty("MatchToAnyMS",m_match_MS=false);
	declareProperty("MatchToCB",m_match_CB=false);
	declareProperty("MatchToLoose",m_match_Loose=false);
	declareProperty("MatchToMedium",m_match_Medium=false);
	declareProperty("MatchToLoose_noCaloTag",m_match_Loose_noCT=false);
	declareProperty("MatchToTight",m_match_Tight=false);
	declareProperty("MatchToHighPt",m_match_HighPt=false);
	declareProperty("MatchToID",m_match_ID=false);
	declareProperty("MatchToCaloTag",m_match_CaloTag=false);
	declareProperty("IDhitCut",m_do_IDHits=true);

	declareProperty("MatchToMuidCB",m_match_MuidCB=false);
	declareProperty("MatchToSTACO",m_match_STACO=false);
	declareProperty("MatchToMuTag",m_match_MuTag=false);
	declareProperty("MatchToMuTagIMO",m_match_MuTagIMO=false);
	declareProperty("MatchToMuidSA",m_match_MuidSA=false);
	declareProperty("MatchToMuGirl",m_match_MuGirl=false);
	declareProperty("MatchToMuGirlLowBeta",m_match_MuGirlLowBeta=false);
	declareProperty("MatchToCaloLikelihood",m_match_CaloLikelihood=false);
	declareProperty("MatchToExtrapolateToIP",m_match_ExtrapolateToIP=false);
  	declareProperty("MatchContainerName", m_matchContainerName = "Muons");

}
void MuonRecoTPEfficiencyTool::dRMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
	// loop over probes
	for(auto probe : *probes) {
		double dRMin = 3;
		Probe* matchProbe = 0;
		const xAOD::Muon* best_match_muon = NULL;
		probe->dr_match(-1.);
		probe->sf_reco(1.);
		// loop over matches
		for(auto match : *matches) {
			// Pt Cut
			if(match->pt() < m_matchPtCut) continue;

			// Eta Cut
			if(fabs(match->eta()) > m_matchEtaCut) continue;

			if (!GoodMatchMuonType(match)) continue;

			const xAOD::Muon* matchmuon = dynamic_cast <const xAOD::Muon*>(match);
			if (matchmuon && m_do_IDHits){
				if (!m_selection_tool->passedIDCuts(*matchmuon)) continue;
			}
			// ID tracks

			if (m_match_ID){
				const xAOD::TrackParticle *trk = dynamic_cast<const xAOD::TrackParticle*>(match);
				if(!trk || (m_do_IDHits && !m_selection_tool->passedIDCuts(*trk))) continue;
			}

			// Calculate dR
			double dR = deltaR(probe, match);

			// test pointer-level matching!
			if(dR < dRMin) {
				dRMin = dR;
				matchProbe = probe;
				if (matchmuon) best_match_muon = matchmuon;
			}
		}

		// check if a matched probe is found
		probe->sf_reco(1.);
		probe->isMatched(matchProbe && dRMin<m_maximumDrCut);
		DecorateMatchInfo(probe->probeTrack(),best_match_muon, matches);
		probe->dr_match(dRMin);
		if (m_do_sf && best_match_muon && probe->isMatched()){
			float sf = 1.;
			if (m_reco_sf_tool->getEfficiencyScaleFactor(*best_match_muon,sf) == CP::CorrectionCode::Ok){
				probe->sf_reco(sf);
			}
		}
	}
}


void MuonRecoTPEfficiencyTool::ptrMatching(ProbeContainer* probes, const xAOD::IParticleContainer* matches) const
{
	// loop over probes
	for(auto probe : *probes) {
		Probe* matchProbe = 0;
		const xAOD::Muon* best_match_muon = NULL;

		const xAOD::Muon* probemuon = dynamic_cast <const xAOD::Muon*>(&(probe->probeTrack()));
		const xAOD::TrackParticle* probetrack = dynamic_cast <const xAOD::TrackParticle*>(&(probe->probeTrack()));

		probe->dr_match(-1.);
		// loop over matches
		for(auto match : *matches) {

			// Pt Cut
			if(match->pt() < m_matchPtCut) continue;

			// Eta Cut
			if(fabs(match->eta()) > m_matchEtaCut) continue;

			if (!GoodMatchMuonType(match)) continue;

			const xAOD::Muon* matchmuon = dynamic_cast <const xAOD::Muon*>(match);
			if (matchmuon && m_do_IDHits){
				if (!m_selection_tool->passedIDCuts(*matchmuon)) continue;
			}

			// ID tracks
			if (m_match_ID){
				const xAOD::TrackParticle *trk = dynamic_cast<const xAOD::TrackParticle*>(match);
				if(!trk || (m_do_IDHits && !m_selection_tool->passedIDCuts(*trk))) continue;
				// match MS to ID
				if (probemuon){
					if (probemuon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle ) == trk){
						matchProbe = probe;
						probe->dr_match(trk->p4().DeltaR(probemuon->p4()));
						best_match_muon = 0;
						break;

					}
				}
			}

			// Probe and Match are xAOD::Muon
			if(probemuon && matchmuon && probemuon==matchmuon) {
				matchProbe = probe;
				best_match_muon = matchmuon;
				probe->dr_match(0.);
				break;
			}

			// probe is xAOD::TrackParticle and match is xAOD::Muon
			if(probetrack && matchmuon) {
				if( probetrack == matchmuon->trackParticle( xAOD::Muon::InnerDetectorTrackParticle ) ||
						probetrack == matchmuon->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle ) ||
						probetrack == matchmuon->trackParticle( xAOD::Muon::MuonSpectrometerTrackParticle ) ) {
					matchProbe = probe;
					probe->dr_match(probetrack->p4().DeltaR(matchmuon->p4()));
					best_match_muon = matchmuon;
					break;
				}
			}

		}

		// check if a matched probe is found
		probe->sf_reco(1.);
		probe->isMatched(matchProbe);
		DecorateMatchInfo(probe->probeTrack(),best_match_muon, matches);
		if (m_do_sf && best_match_muon && probe->isMatched()){
			float sf = 1.;
			if (m_reco_sf_tool->getEfficiencyScaleFactor(*best_match_muon,sf) == CP::CorrectionCode::Ok){
				probe->sf_reco(sf);
			}
		}
	}
}

bool MuonRecoTPEfficiencyTool::GoodMatchMuonType(const xAOD::IParticle* probe) const{

	const xAOD::Muon* mumatch = dynamic_cast <const xAOD::Muon*> (probe);

	// if the particle is not a muon, assume that the user knows what TrackParticleContainer he is providing!
	if (! mumatch) return true;

	// otherwise, we have to manually pick the right probe

	// ID Track
	if (m_match_ID){
		return (mumatch->trackParticle(xAOD::Muon::InnerDetectorTrackParticle) != NULL);
	}
	// CT Match

	if (m_match_CaloTag){
		return (mumatch &&  m_selection_tool->passedCaloTagQuality(*mumatch) &&  mumatch->isAuthor(xAOD::Muon::CaloTag) );
	}
	// MS Match
	if (m_match_MS){
		return ((mumatch->muonType() == xAOD::Muon::MuonStandAlone || mumatch->muonType() == xAOD::Muon::Combined)
				&& mumatch->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle)!= NULL);
	}
	// CB Match
	if (m_match_CB){
		return (mumatch->muonType() == xAOD::Muon::Combined && mumatch->trackParticle(xAOD::Muon::CombinedTrackParticle) != NULL);
	}
	if (m_match_Loose){
		return (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Loose);
	}
	if (m_match_Loose_noCT){
		return (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Loose && mumatch->muonType() != xAOD::Muon::CaloTagged);
	}
	if (m_match_Medium){
		return (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Medium);
	}
	if (m_match_Tight){
		return (m_selection_tool->getQuality(*mumatch) <= xAOD::Muon::Tight);
	}
	if (m_match_HighPt){
		return m_selection_tool->passedHighPtCuts(*mumatch);
	}
	if (m_match_MuidCB){
		return (mumatch->isAuthor(xAOD::Muon::MuidCo));
	}
	if (m_match_STACO){
		return (mumatch->isAuthor(xAOD::Muon::STACO));
	}
	if (m_match_MuTag){
		return (mumatch->isAuthor(xAOD::Muon::MuTag));
	}
	if (m_match_MuTagIMO){
		return (mumatch->isAuthor(xAOD::Muon::MuTagIMO));
	}
	if (m_match_MuidSA){
		return (mumatch->isAuthor(xAOD::Muon::MuidSA));
	}
	if (m_match_MuGirl){
		return (mumatch->isAuthor(xAOD::Muon::MuGirl));
	}
	if (m_match_MuGirlLowBeta){
		return (mumatch->isAuthor(xAOD::Muon::MuGirlLowBeta));
	}
	if (m_match_CaloLikelihood){
		return (mumatch->isAuthor(xAOD::Muon::CaloLikelihood));
	}
	if (m_match_ExtrapolateToIP){
		return (mumatch->isAuthor(xAOD::Muon::ExtrapolateMuonToIP));
	}
	return false;
}
//**********************************************************************

void MuonRecoTPEfficiencyTool::matchProbes(ProbeContainer* probes) const
{

	ATH_MSG_DEBUG(" -- Pick up matches");
	// retrieve match container  
	const xAOD::IParticleContainer* matches = 0;
	if(evtStore()->retrieve(matches, m_matchContainerName).isFailure()) {
	  ATH_MSG_ERROR( "Unable to retrieve " << m_matchContainerName );
	  return ;
	}
	if(m_ptrMatching) ptrMatching(probes, matches);
	else dRMatching(probes, matches);

	if (msgLvl(MSG::DEBUG)) {
		int nmatched=0;
		for(auto probe : *probes)
			if(probe->isMatched())
				nmatched++;
		ATH_MSG_DEBUG("Number of matched probes    : " << nmatched );
	}
}
void MuonRecoTPEfficiencyTool::DecorateMatchInfo(const xAOD::IParticle & probe, const xAOD::IParticle* match, const xAOD::IParticleContainer* matches) const{
        ElementLink<xAOD::IParticleContainer> matchLink;
        if (match) matchLink = ElementLink<xAOD::IParticleContainer>(match,*matches);
	SG::AuxElement::Decorator< ElementLink<xAOD::IParticleContainer > > Dec_Match_object(std::string("match_ptr_")+m_efficiencyFlag);
	Dec_Match_object(probe) = matchLink;
}



