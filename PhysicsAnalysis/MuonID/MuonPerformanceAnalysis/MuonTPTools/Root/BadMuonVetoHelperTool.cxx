/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * EventBadMuonVetoDecorator.cxx
 *
 *  Created on: Jun 3, 2016
 *      Author: goblirsc
 */

#include "MuonTPTools/BadMuonVetoHelperTool.h"

#include "xAODMuon/MuonContainer.h"


BadMuonVetoHelperTool::BadMuonVetoHelperTool(std::string myname) :
        AsgTool(myname),
		m_vetovar_applied("BadMuonVeto_decorated"),
		m_vetovars_HighPt("BadMuonVeto_MinValue_HighPt")
        {
    declareProperty("MuonContainer",      m_muonContainer="Muons");
    declareProperty("MuonSelectionTool",      m_MST);

	for (int q = (int) xAOD::Muon::Tight; q <= (int)xAOD::Muon::VeryLoose; ++q){
		// yuck! But it works...
		m_vetovars.insert (std::make_pair((xAOD::Muon::Quality)q,
				SG::AuxElement::Decorator<std::pair<float,float> >(
						Form("BadMuonVeto_MinValue_%i",q))));
	}

}

StatusCode BadMuonVetoHelperTool::initialize() {
    // retrieve the tool
    ATH_CHECK(m_MST.retrieve());
    return StatusCode::SUCCESS;
}

StatusCode BadMuonVetoHelperTool::getDiscriminatingVars (xAOD::Muon::Quality WP,float & ID_Discriminant, float & ME_Discriminant) const{

    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }

	StatusCode SC(apply_if_missing(info));
	try{
		std::pair<float,float> result = m_vetovars.at(WP)(*info);
		ID_Discriminant = result.first;
		ME_Discriminant = result.second;
	}
	catch (SG::ExcBadAuxVar & ex){
		ATH_MSG_ERROR("Unable to retrieve the Bad Muon Veto vars: "<<ex.what());
		return StatusCode::FAILURE;
	}
	return SC;

}
StatusCode BadMuonVetoHelperTool::getDiscriminatingVarsHighPt (float & ID_Discriminant, float & ME_Discriminant) const{

    const xAOD::EventInfo* info = 0;
    ATH_MSG_DEBUG(""<<evtStore());
    if (evtStore()->retrieve(info, "EventInfo").isFailure()){
        ATH_MSG_FATAL( "Unable to retrieve Event Info" );
    }

	StatusCode SC(apply_if_missing(info));

	try{
		std::pair<float,float> result = m_vetovars_HighPt(*info);
		ID_Discriminant = result.first;
		ME_Discriminant = result.second;
	}
	catch (SG::ExcBadAuxVar & ex){
		ATH_MSG_ERROR("Unable to retrieve the Bad Muon Veto vars: "<<ex.what());
		return StatusCode::FAILURE;
	}
	return SC;

}


StatusCode BadMuonVetoHelperTool::apply_if_missing (const xAOD::EventInfo* info) const{


    if (m_vetovar_applied.isAvailable(*info) && m_vetovar_applied(*info)){
    	// cool, nothing to do!
    	return StatusCode::SUCCESS;
    }

    // otherwise, we have to do something

    const xAOD::MuonContainer* muons = 0;
    if(evtStore()->retrieve(muons, m_muonContainer).isFailure()) {
		ATH_MSG_FATAL( "Unable to retrieve " << m_muonContainer );
		return StatusCode::FAILURE;
	}

    std::map <xAOD::Muon::Quality, std::pair<float,float> > minvars_perquality;
    for (auto & thing : m_vetovars){
		minvars_perquality[thing.first] = std::make_pair(-1,-1);
    }
    std::pair<float,float> minvar_highpt = std::make_pair(-1,-1);

    float discriminant_ID  = -1;
    float discriminant_ME  = -1;
    float sigma_CB = -1;
	float qoverp_CB = -1;
    float sigma_ID = -1;
	float qoverp_ID = -1;
    float sigma_ME = -1;
	float qoverp_ME = -1;

    for (const auto & mu : *muons){
    	xAOD::Muon::Quality outcome = m_MST->getQuality(*mu);
    	const xAOD::TrackParticle* CBtrk = mu->trackParticle(xAOD::Muon::CombinedTrackParticle);
		const xAOD::TrackParticle* MEtrk = mu->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
		const xAOD::TrackParticle* IDtrk = mu->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);

		if (!CBtrk || !MEtrk || !IDtrk) continue;

		qoverp_CB = CBtrk->qOverP();
		sigma_CB = sqrt(CBtrk->definingParametersCovMatrix()(4,4));

		qoverp_ME = MEtrk->qOverP();
		sigma_ME = sqrt(MEtrk->definingParametersCovMatrix()(4,4));

		qoverp_ID = IDtrk->qOverP();
		sigma_ID = sqrt(IDtrk->definingParametersCovMatrix()(4,4));

		// avoid division...
		float OneOver_denominator = fabs(qoverp_CB / sigma_CB);
		discriminant_ID = fabs(sigma_ID / qoverp_ID) * OneOver_denominator;
		discriminant_ME = fabs(sigma_ME / qoverp_ME) * OneOver_denominator;

		for (auto & thing : minvars_perquality){
			if (outcome <= thing.first){
				if (thing.second.first < 0 || discriminant_ID < thing.second.first) {
					thing.second.first = discriminant_ID;
				}
				if (thing.second.second < 0 || discriminant_ME < thing.second.second) {
						thing.second.second = discriminant_ME;
				}
			}
		}
		if (m_MST->passedHighPtCuts(*mu)){
			if (minvar_highpt.first < 0 || discriminant_ID < minvar_highpt.first) {
				minvar_highpt.first = discriminant_ID;
			}
			if (minvar_highpt.second < 0 || discriminant_ME < minvar_highpt.second) {
				minvar_highpt.second = discriminant_ME;
			}
		}
    }

    for (auto & thing : minvars_perquality){
    	m_vetovars.at(thing.first)(*info) = thing.second;
    }
	m_vetovars_HighPt(*info) = minvar_highpt;

	m_vetovar_applied(*info) = true;

    return StatusCode::SUCCESS;


}
