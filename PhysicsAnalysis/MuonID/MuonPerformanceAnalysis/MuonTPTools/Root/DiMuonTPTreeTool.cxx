/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPTrees.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/DiMuonTPTreeTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODJet/JetContainer.h"
#include "MuonTPTools/TrackParticlexAODHelpers.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

DiMuonTPTreeTool::DiMuonTPTreeTool(std::string name)
:  MuonTPTreeTool(name),
   m_runNumber(0),
   m_eventNumber(0),
   m_mcChannelNumber(0),
   m_lumiblock(0),
   m_average_mu(-1.),
   m_actual_mu(-1),
   m_PV_n(-1),
   m_BCID(-1),
   m_mcEventWeight(-1),
   m_pt(-1),
   m_eta(-1),
   m_etaMS(-1),
   m_phi(-1),
   m_type(-1),
   m_author(-1),
   m_quality(-1),
   m_passHighPt(false),
   m_fineEtaPhi(-1),
   m_detregion(-1),
   m_q(-1),
   m_d0(-1),
   m_d0err(-1),
   m_z0(-1),
   m_primarySector(0),
   m_secondarySector(0),
   m_phiLayer1Hits(0),
   m_phiLayer2Hits(0),
   m_phiLayer3Hits(0),
   m_phiLayer4Hits(0),

   m_innerSmallHits(0),
   m_innerLargeHits(0),
   m_middleSmallHits(0),
   m_middleLargeHits(0),
   m_outerSmallHits(0),
   m_outerLargeHits(0),
   m_extendedSmallHits(0),
   m_extendedLargeHits(0),

   m_innerSmallHoles(0),
   m_innerLargeHoles(0),
   m_middleSmallHoles(0),
   m_middleLargeHoles(0),
   m_outerSmallHoles(0),
   m_outerLargeHoles(0),
   m_extendedSmallHoles(0),
   m_extendedLargeHoles(0),
   m_phiLayer1Holes(0),
   m_phiLayer2Holes(0),
   m_phiLayer3Holes(0),
   m_phiLayer4Holes(0),
   m_probe_precisionLayers(0),
   m_probe_precisionHoleLayers(0),
   m_probe_phiLayers(0),
   m_probe_phiHoleLayers(0),
   m_probe_trigEtaLayers(0),
   m_probe_trigEtaHoleLayers(0),
   m_probe_EnergyLoss(-1),
   m_probe_ParamEnergyLoss(-1),
   m_probe_MeasEnergyLoss(-1),
   m_numberOfGoodPrecisionLayers(0),
   m_innerClosePrecisionHits(0),
   m_middleClosePrecisionHits(0),
   m_outerClosePrecisionHits(0),
   m_extendedClosePrecisionHits(0),
   m_innerOutBoundsPrecisionHits(0),
   m_middleOutBoundsPrecisionHits(0),
   m_outerOutBoundsPrecisionHits(0),
   m_extendedOutBoundsPrecisionHits(0),
   m_combinedTrackOutBoundsPrecisionHits(0),
   m_isEndcapGoodLayers(false),
   m_isSmallGoodSectors(false),
   m_allauthors(0),
   m_qoverp_CB(-1),
   m_qoverp_err_CB(-1),
   m_qoverp_MS(-1),
   m_qoverp_err_MS(-1),
   m_qoverp_ME(-1),
   m_qoverp_err_ME(-1),
   m_qoverp_ID(-1),
   m_qoverp_err_ID(-1),
   m_chi2(-1),
   m_nDof(-1),
   m_momentumBalanceSignificance(-1),
   m_probe_CaloMuonIDTag(-1),
   m_probe_CaloLRLikelihood(-1),
   m_tagPt(-1),
   m_tagEta(-1),
   m_tagPhi(-1),
   m_tag_q(-1),
   m_tag_d0(-1),
   m_tag_d0err(-1),
   m_tag_z0(-1),
   m_mll(-1),
   m_dPhi(-1),
   m_dEta(-1),
   m_dPhi_Pivot(-1),
   m_dEta_Pivot(-1),
   m_dR_Pivot (-1),
   m_Phi_Pivot(-1),
   m_Eta_Pivot(-1),
   m_dilep_pt(-1),
   m_dilep_tof(-1),
   m_tag_deltaR(-1),
   m_tag_jetpt(-1),
   m_deltaR(-1),
   m_closest_jetpt(-1),
   m_closest_jetptrel(-1),
   m_closest_jetemfrac(-1),
   m_closest_jetchargedfrac(-1),
   m_closest_jetfsrdr(-1),
   m_leading_jetptrel(-1),
   m_leading_jetemfrac(-1),
   m_leading_jetchargedfrac(-1),
   m_leading_jetfsrdr(-1),
   m_sum_jetptrel(-1),
   m_probe_hasFSRcand(false),
   m_probe_FSRtype(-1),
   m_probe_FSR_dR(-1),
   m_probe_FSR_Et(-1),
   m_probe_FSR_Etrel(-1),
   m_closest_jetDrToFsr(-1),
   m_closest_jetPtRel_fsrfrac(-1),
   m_closest_jetPt_fsrfrac(-1),
   m_leading_jetDrToFsr(-1),
   m_leading_jetPtRel_fsrfrac(-1),
   m_leading_jetPt_fsrfrac(-1),

   m_probe_truth_pdgId(-1),
   m_probe_truth_type(-1),
   m_probe_truth_origin(-1),

   m_tag_truth_pdgId(-1),
   m_tag_truth_type(-1),
   m_tag_truth_origin(-1),


   m_tagiso_neflowisol20(-1),
   m_tagiso_topoetcone20(-1),
   m_tagiso_ptcone20(-1),
   m_tagiso_etcone20(-1),
   m_tagiso_ptvarcone20(-1),

   m_tagiso_neflowisol30(-1),
   m_tagiso_topoetcone30(-1),
   m_tagiso_ptcone30(-1),
   m_tagiso_etcone30(-1),
   m_tagiso_ptvarcone30(-1),

   m_tagiso_neflowisol40(-1),
   m_tagiso_topoetcone40(-1),
   m_tagiso_ptcone40(-1),
   m_tagiso_etcone40(-1),
   m_tagiso_ptvarcone40(-1),

   m_tag_topocore(-1),

   m_probeiso_neflowisol20(-1),
   m_probeiso_topoetcone20(-1),
   m_probeiso_ptcone20(-1),
   m_probeiso_etcone20(-1),
   m_probeiso_ptvarcone20(-1),

   m_probeiso_neflowisol30(-1),
   m_probeiso_topoetcone30(-1),
   m_probeiso_ptcone30(-1),
   m_probeiso_etcone30(-1),
   m_probeiso_ptvarcone30(-1),

   m_probeiso_neflowisol40(-1),
   m_probeiso_topoetcone40(-1),
   m_probeiso_ptcone40(-1),
   m_probeiso_etcone40(-1),
   m_probeiso_ptvarcone40(-1),

   m_probe_topocore(-1),

   m_probe_pl_tagweight(-1),
   m_probe_pl_trackjetntrack(-1),

   m_tag_pl_tagweight(-1),
   m_tag_pl_trackjetntrack(-1),

   m_energyDensity_central(-1),
   m_energyDensity_forward(-1),
   m_FCAL_Et(-1),

   m_EventBadMuonVetoVar_VeryLoose_ME(-1),
   m_EventBadMuonVetoVar_VeryLoose_ID(-1),

   m_EventBadMuonVetoVar_Loose_ME(-1),
   m_EventBadMuonVetoVar_Loose_ID(-1),

   m_EventBadMuonVetoVar_Medium_ME(-1),
   m_EventBadMuonVetoVar_Medium_ID(-1),

   m_EventBadMuonVetoVar_Tight_ME(-1),
   m_EventBadMuonVetoVar_Tight_ID(-1),

   m_EventBadMuonVetoVar_HighPt_ME(-1),
   m_EventBadMuonVetoVar_HighPt_ID(-1),

   m_tag_isTruthMatched(false),
   m_probe_isTruthMatched(false)

// m_jet_cleaning_tool("JetCleaningTool")
{
	declareProperty("MuonSelectionTool",m_muon_selection_tool);
	declareProperty("FSRPhotonTool",m_IFsrPhotonTool);
	declareProperty("JetCleaningTool",m_jet_cleaning_tool);
	declareProperty("BadMuonVetoHelper",m_bad_muon_veto_helper);
	declareProperty("AddExtendedIsolation",m_extended_Iso_Info = false);
	declareProperty("AddExtendedTrigger",m_extended_Trig_Info = false);
	declareProperty("AddTruthMatching",m_add_Truth_Matching= false);
	declareProperty("ProbeIsTruth",m_is_truthprobe= false);
    declareProperty("WriteMatchObjectInfo",m_write_matchobj_info= true);


}

StatusCode DiMuonTPTreeTool::initialize()
{
	ATH_CHECK(MuonTPTreeTool::initialize());
	ATH_CHECK(m_IFsrPhotonTool.retrieve());
	ATH_CHECK(m_muon_selection_tool.retrieve());
	ATH_CHECK(m_jet_cleaning_tool.retrieve());
	ATH_CHECK(m_bad_muon_veto_helper.retrieve());
	return StatusCode::SUCCESS;

}

void DiMuonTPTreeTool::FillBadMuonVetoInfo(){

	float var_ID = -1;
	float var_ME = -1;

	if (m_bad_muon_veto_helper->getDiscriminatingVars(xAOD::Muon::VeryLoose, var_ID, var_ME).isSuccess()){
		m_EventBadMuonVetoVar_VeryLoose_ID = var_ID;
		m_EventBadMuonVetoVar_VeryLoose_ME = var_ME;
	}
	else{
		m_EventBadMuonVetoVar_VeryLoose_ID = -1;
		m_EventBadMuonVetoVar_VeryLoose_ME = -1;
	}

	if (m_bad_muon_veto_helper->getDiscriminatingVars(xAOD::Muon::Loose, var_ID, var_ME).isSuccess()){
		m_EventBadMuonVetoVar_Loose_ID = var_ID;
		m_EventBadMuonVetoVar_Loose_ME = var_ME;
	}
	else{
		m_EventBadMuonVetoVar_Loose_ID = -1;
		m_EventBadMuonVetoVar_Loose_ME = -1;
	}

	if (m_bad_muon_veto_helper->getDiscriminatingVars(xAOD::Muon::Medium, var_ID, var_ME).isSuccess()){
		m_EventBadMuonVetoVar_Medium_ID = var_ID;
		m_EventBadMuonVetoVar_Medium_ME = var_ME;
	}
	else{
		m_EventBadMuonVetoVar_Medium_ID = -1;
		m_EventBadMuonVetoVar_Medium_ME = -1;
	}

	if (m_bad_muon_veto_helper->getDiscriminatingVars(xAOD::Muon::Tight, var_ID, var_ME).isSuccess()){
		m_EventBadMuonVetoVar_Tight_ID = var_ID;
		m_EventBadMuonVetoVar_Tight_ME = var_ME;
	}
	else{
		m_EventBadMuonVetoVar_Tight_ID = -1;
		m_EventBadMuonVetoVar_Tight_ME = -1;
	}

	if (m_bad_muon_veto_helper->getDiscriminatingVarsHighPt(var_ID, var_ME).isSuccess()){
		m_EventBadMuonVetoVar_HighPt_ID = var_ID;
		m_EventBadMuonVetoVar_HighPt_ME = var_ME;
	}
	else{
		m_EventBadMuonVetoVar_HighPt_ID = -1;
		m_EventBadMuonVetoVar_HighPt_ME = -1;
	}

}

void DiMuonTPTreeTool::FillTruthMatchInfo(Probe & probe){

	const xAOD::Muon* mutag = dynamic_cast<const xAOD::Muon*>(&probe.tagTrack());
	const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());
	const xAOD::TrackParticle* idprobe = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
	const xAOD::TruthParticle* trueprobe = dynamic_cast<const xAOD::TruthParticle*>(&probe.probeTrack());

	static SG::AuxElement::Accessor< int > truthType ("truthType");
	static SG::AuxElement::Accessor< int > truthOrigin ("truthOrigin");

	try{
		if (mutag){
			ElementLink<xAOD::TruthParticleContainer> link = mutag->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
			if(link.isValid() && *link){
				m_tag_truth_pdgId = abs((*link)->pdgId());
				m_tag_isTruthMatched = ( m_tag_truth_pdgId==13 );
				m_tag_truth_type = truthType(**link);
				m_tag_truth_origin = truthOrigin(**link);
			}
		}

	}
	catch (SG::ExcBadAuxVar & ex){
		ATH_MSG_WARNING("Missing truth matching decorations for a tag muon");
	}

	// mu probe
	try{
		if (muprobe){
			ElementLink<xAOD::TruthParticleContainer> link = muprobe->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
			if(link.isValid() && *link){
				m_probe_truth_pdgId = abs((*link)->pdgId());
				m_probe_isTruthMatched = ( m_probe_truth_pdgId==13 );
				m_probe_truth_type = truthType(**link);
				m_probe_truth_origin = truthOrigin(**link);
				return;
			}
		}
	}
	catch (SG::ExcBadAuxVar & ex){
		ATH_MSG_WARNING("Missing truth matching decorations for a probe muon");
	}

	// ID probe
	try{
		if (idprobe){
			ElementLink<xAOD::TruthParticleContainer> link = idprobe->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink");
			if(link.isValid() && *link){
				m_probe_truth_pdgId = abs((*link)->pdgId());
				m_probe_isTruthMatched = ( m_probe_truth_pdgId==13 );
				m_probe_truth_type = truthType(*idprobe);
				m_probe_truth_origin = truthOrigin(*idprobe);
				return;
			}
		}
	}
	catch (SG::ExcBadAuxVar & ex){
		ATH_MSG_WARNING("Missing truth matching decorations for a probe ID track");
	}

	// true probe
	try{
		if (trueprobe){

				m_probe_truth_pdgId = abs(trueprobe->pdgId());
				m_probe_isTruthMatched = ( m_probe_truth_pdgId==13 );
				m_probe_truth_type = truthType(*trueprobe);
				m_probe_truth_origin = truthOrigin(*trueprobe);
				return;
		}
	}
	catch (SG::ExcBadAuxVar & ex){
		ATH_MSG_WARNING("Missing truth matching decorations for a truth probe");
	}

}

void DiMuonTPTreeTool::FillCustomStuff(Probe& probe){

	m_energyDensity_central = 0.;
	m_energyDensity_forward = 0.;
	m_tag_topocore = 0.;
	m_probe_topocore = 0.;
	m_tag_isTruthMatched = false;
	m_probe_isTruthMatched = false;
	m_probe_hasFSRcand = false; // protection if misordered

	// Event Info
	const xAOD::EventInfo* info = 0;
	ATH_MSG_DEBUG(""<<evtStore());
	if (evtStore()->retrieve(info, "EventInfo").isFailure()){
		ATH_MSG_FATAL( "Unable to retrieve Event Info" );
	}
	static bool isMC = info->eventType(xAOD::EventInfo::IS_SIMULATION);

	ATH_MSG_DEBUG("Filling event shapes");
	// event shapes
	FillEventShapeInfo();
	// FSR information
	ATH_MSG_DEBUG("Filling FSR");
	FillFSRInfo(probe);
	// Close-by jet information
	ATH_MSG_DEBUG("Filling Jet assoc.");
	FillJetAssocInfo(probe);
	ATH_MSG_DEBUG("Filling prompt lepton tagger");
	FillPromptLeptonTaggerInfo(probe);

	FillMatchObjectInfo(probe);

	FillBadMuonVetoInfo();

	if(m_add_Truth_Matching && isMC){
		FillTruthMatchInfo(probe);
	}


	// Primary vertex variables
	ATH_MSG_DEBUG("Filling Vertices");
	const xAOD::VertexContainer* primVertices = 0 ;
	const xAOD::Vertex* pv = 0;
	if(evtStore()->retrieve(primVertices,"PrimaryVertices").isFailure()){
		ATH_MSG_ERROR("Found no PV candidate for IP computation!");
	}
	else {
		pv = primVertices->at(0);
	}
	m_PV_n=0;
	for (auto vx: *primVertices){
		if (vx->vertexType() == xAOD::VxType::PriVtx || vx->vertexType() == xAOD::VxType::PileUp) ++m_PV_n;
	}
	// check for truth probe
	const xAOD::TruthParticle* truthtrk = dynamic_cast<const xAOD::TruthParticle*>(&probe.probeTrack());
	// check for muon probe
	const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());

	int charge = -2;
	m_type = 0;
	m_author = -1;
	m_quality = -1;
	const xAOD::Vertex* vx_tag = 0;
	const xAOD::Vertex* vx_probe = 0;
	// tag muon properties
	const xAOD::Muon* mutag = dynamic_cast<const xAOD::Muon*>(&probe.tagTrack());
	int charge2 = -2;
	ATH_MSG_DEBUG("Filling tag muon");
	if (mutag) {

		static SG::AuxElement::Accessor< float > DecDeta ("probe_dEtaTriggerPivot");
		static SG::AuxElement::Accessor< float > DecDphi ("probe_dPhiTriggerPivot");
		static SG::AuxElement::Accessor< float > DecEta ("probe_EtaTriggerPivot");
		static SG::AuxElement::Accessor< float > DecPhi ("probe_PhiTriggerPivot");
        static SG::AuxElement::Accessor< float > DecDR ("probe_dRTriggerPivot");

		if(DecDeta.isAvailable(*mutag))
			m_dEta_Pivot = DecDeta(*mutag);

		if(DecDphi.isAvailable(*mutag))
			m_dPhi_Pivot = DecDphi(*mutag);

        if (DecDR.isAvailable(*mutag)){
            m_dR_Pivot = DecDR(*mutag);
        }

		if(DecEta.isAvailable(probe.probeTrack()))
			m_Eta_Pivot = DecEta(probe.probeTrack());

		if(DecPhi.isAvailable(probe.probeTrack()))
			m_Phi_Pivot = DecPhi(probe.probeTrack());

		charge2 = mutag->trackParticle(xAOD::Muon::Primary)->charge();
		vx_tag = mutag->trackParticle(xAOD::Muon::Primary)->vertex();

		if (mutag->primaryTrackParticle()){
			m_tag_d0 = mutag->primaryTrackParticle()->d0();

			float d0sign = xAOD::MuonTP_TrackingHelpers::d0significance(mutag->primaryTrackParticle(), info->beamPosSigmaX(), info->beamPosSigmaY(), info->beamPosSigmaXY() );
			m_tag_d0err = fabs( m_tag_d0 / d0sign );
			//m_tag_d0err = sqrt(mutag->primaryTrackParticle()->definingParametersCovMatrix()(0,0));

			m_tag_z0 = mutag->primaryTrackParticle()->z0();
			if (pv){
				m_tag_z0 = m_tag_z0 - pv->z() + mutag->primaryTrackParticle()->vz();
			}
		}
		if (m_extended_Iso_Info){
			bool ok2 = mutag->isolation(m_tagiso_etcone40,xAOD::Iso::topoetcone40);
			if (!ok2) m_tagiso_etcone40 = -1;
			ok2 = mutag->isolation(m_tagiso_ptcone40,xAOD::Iso::ptcone40);
			if (!ok2) m_tagiso_ptcone40 = -1;
			ok2 = mutag->isolation(m_tagiso_topoetcone40,xAOD::Iso::topoetcone40);
			if (!ok2) m_tagiso_topoetcone40 = -1;
			ok2 = mutag->isolation(m_tagiso_ptvarcone40,xAOD::Iso::ptvarcone40);
			if (!ok2) m_tagiso_ptvarcone40 = -1;
			ok2 = mutag->isolation(m_tagiso_neflowisol40,xAOD::Iso::neflowisol40);
			if (!ok2) m_tagiso_neflowisol40 = -1;

			ok2 = mutag->isolation(m_tagiso_etcone30,xAOD::Iso::topoetcone30);
			if (!ok2) m_tagiso_etcone30 = -1;
			ok2 = mutag->isolation(m_tagiso_ptcone30,xAOD::Iso::ptcone30);
			if (!ok2) m_tagiso_ptcone30 = -1;
			ok2 = mutag->isolation(m_tagiso_topoetcone30,xAOD::Iso::topoetcone30);
			if (!ok2) m_tagiso_topoetcone30 = -1;
			ok2 = mutag->isolation(m_tagiso_ptvarcone30,xAOD::Iso::ptvarcone30);
			if (!ok2) m_tagiso_ptvarcone30 = -1;
			ok2 = mutag->isolation(m_tagiso_neflowisol30,xAOD::Iso::neflowisol30);
			if (!ok2) m_tagiso_neflowisol30 = -1;

			ok2 = mutag->isolation(m_tagiso_etcone20,xAOD::Iso::topoetcone20);
			if (!ok2) m_tagiso_etcone20 = -1;
			ok2 = mutag->isolation(m_tagiso_ptcone20,xAOD::Iso::ptcone20);
			if (!ok2) m_tagiso_ptcone20 = -1;
			ok2 = mutag->isolation(m_tagiso_topoetcone20,xAOD::Iso::topoetcone20);
			if (!ok2) m_tagiso_topoetcone20 = -1;
			ok2 = mutag->isolation(m_tagiso_ptvarcone20,xAOD::Iso::ptvarcone20);
			if (!ok2) m_tagiso_ptvarcone20 = -1;
			ok2 = mutag->isolation(m_tagiso_neflowisol20,xAOD::Iso::neflowisol20);
			if (!ok2) m_tagiso_neflowisol20 = -1;
		}
	}


	// case of a muon probe
	if (muprobe) {
		ATH_MSG_DEBUG("Filling probe muon");



		charge = muprobe->trackParticle(xAOD::Muon::Primary)->charge() ;

		FillMuProbeHitInfo(muprobe);
		FillMuProbeTrkDetails(muprobe);

		if (muprobe->primaryTrackParticle()){
			m_d0 = muprobe->primaryTrackParticle()->d0();

			float d0sign = xAOD::MuonTP_TrackingHelpers::d0significance(muprobe->primaryTrackParticle(), info->beamPosSigmaX(), info->beamPosSigmaY(), info->beamPosSigmaXY() );
			m_d0err = fabs( m_d0 / d0sign );
			//m_d0err = sqrt(muprobe->primaryTrackParticle()->definingParametersCovMatrix()(0,0));

			m_z0 = muprobe->primaryTrackParticle()->z0();
			if (pv){
				m_z0 = m_z0 - pv->z() + muprobe->primaryTrackParticle()->vz();
			}
		}
		vx_probe = muprobe->trackParticle(xAOD::Muon::Primary)->vertex();
		muprobe->isolationCaloCorrection(m_probe_topocore,
				xAOD::Iso::topoetcone,
				xAOD::Iso::coreCone,
				xAOD::Iso::IsolationCorrectionParameter::coreEnergy);

		xAOD::Muon::Quality probe_qual = m_muon_selection_tool->getQuality(*muprobe);
		m_passHighPt = m_muon_selection_tool->passedHighPtCuts(*muprobe);
		m_quality = probe_qual;
		m_author = muprobe->author();
		m_type = muprobe->muonType();

		if (m_extended_Iso_Info){
			try{
				bool ok = muprobe->isolation(m_probeiso_etcone40,xAOD::Iso::topoetcone40);
				if (!ok) m_probeiso_etcone40 = -1;
				ok = muprobe->isolation(m_probeiso_ptcone40,xAOD::Iso::ptcone40);
				if (!ok) m_probeiso_ptcone40 = -1;
				ok = muprobe->isolation(m_probeiso_topoetcone40,xAOD::Iso::topoetcone40);
				if (!ok) m_probeiso_topoetcone40 = -1;
				ok = muprobe->isolation(m_probeiso_ptvarcone40,xAOD::Iso::ptvarcone40);
				if (!ok) m_probeiso_ptvarcone40 = -1;
				ok = muprobe->isolation(m_probeiso_neflowisol40,xAOD::Iso::neflowisol40);
				if (!ok) m_probeiso_neflowisol40 = -1;

				ok = muprobe->isolation(m_probeiso_etcone30,xAOD::Iso::topoetcone30);
				if (!ok) m_probeiso_etcone30 = -1;
				ok = muprobe->isolation(m_probeiso_ptcone30,xAOD::Iso::ptcone30);
				if (!ok) m_probeiso_ptcone30 = -1;
				ok = muprobe->isolation(m_probeiso_topoetcone30,xAOD::Iso::topoetcone30);
				if (!ok) m_probeiso_topoetcone30 = -1;
				ok = muprobe->isolation(m_probeiso_ptvarcone30,xAOD::Iso::ptvarcone30);
				if (!ok) m_probeiso_ptvarcone30 = -1;
				ok = muprobe->isolation(m_probeiso_neflowisol30,xAOD::Iso::neflowisol30);
				if (!ok) m_probeiso_neflowisol30 = -1;

				ok = muprobe->isolation(m_probeiso_etcone20,xAOD::Iso::topoetcone20);
				if (!ok) m_probeiso_etcone20 = -1;
				ok = muprobe->isolation(m_probeiso_ptcone20,xAOD::Iso::ptcone20);
				if (!ok) m_probeiso_ptcone20 = -1;
				ok = muprobe->isolation(m_probeiso_topoetcone20,xAOD::Iso::topoetcone20);
				if (!ok) m_probeiso_topoetcone20 = -1;
				ok = muprobe->isolation(m_probeiso_ptvarcone20,xAOD::Iso::ptvarcone20);
				if (!ok) m_probeiso_ptvarcone40 = -1;
				ok = muprobe->isolation(m_probeiso_neflowisol20,xAOD::Iso::neflowisol20);
				if (!ok) m_probeiso_neflowisol20 = -1;
			}
			catch (SG::ExcBadAuxVar & ){
				ATH_MSG_WARNING("Unable to fill probe isolation info!");
			}
		}
	}

	// case of an ID probe
	const xAOD::TrackParticle* trkprobe = dynamic_cast<const xAOD::TrackParticle*>(&probe.probeTrack());
	if (!muprobe && trkprobe){
		ATH_MSG_DEBUG("Filling id probe");
		charge = trkprobe->charge();
		vx_probe = trkprobe->vertex();
		m_d0 = trkprobe->d0();

		float d0sign = xAOD::MuonTP_TrackingHelpers::d0significance(trkprobe, info->beamPosSigmaX(), info->beamPosSigmaY(), info->beamPosSigmaXY() );
		m_d0err = fabs( m_d0 / d0sign );
		//m_d0err = sqrt(trkprobe->definingParametersCovMatrix()(0,0));

		m_z0 = trkprobe->z0();
		//         const xAOD::Vertex* vx = trkprobe->vertex();
		if (pv){
			m_z0 = m_z0 - pv->z() + trkprobe->vz();
		}

		try{
			// Note: Some variables are not available in the MUON1 derivation. Therefore, we only
			// fill those that are always well-defined.
			// The non-filled vars are not used for the muon WP, so should be fine!
			m_probeiso_ptcone20 = -1;
			m_probeiso_ptcone30 = trkprobe->auxdataConst<float>("ptcone30");
			m_probeiso_ptcone40 = trkprobe->auxdataConst<float>("ptcone40");

			m_probeiso_ptvarcone20 = -1;
			m_probeiso_ptvarcone30 = trkprobe->auxdataConst<float>("ptvarcone30");
			m_probeiso_ptvarcone40 = trkprobe->auxdataConst<float>("ptvarcone40");

			m_probeiso_topoetcone20 = trkprobe->auxdataConst<float>("topoetcone20");
			m_probeiso_topoetcone30 = -1;
			m_probe_topocore = trkprobe->auxdataConst<float>("etcore_correction");


			m_probeiso_topoetcone40 = trkprobe->auxdataConst<float>("topoetcone40");
		}
		catch (const SG::ExcBadAuxVar&){
			ATH_MSG_WARNING("ID probe without isolation decoration!!");
			m_probeiso_ptcone20 = -1;
			m_probeiso_ptcone30 = -1;
			m_probeiso_ptcone40 = -1;
			m_probeiso_ptvarcone20 = -1;
			m_probeiso_ptvarcone30 = -1;
			m_probeiso_ptvarcone40 = -1;
			m_probeiso_topoetcone20 = -1;
			m_probeiso_topoetcone30 = -1;
			m_probeiso_topoetcone40 = -1;
			m_probe_topocore = -1;
		}
		m_passHighPt = false;
	}
	// case of an truth probe: rely on the sign of the pdg ID
	if (truthtrk) {
		ATH_MSG_DEBUG("Filling truth probe");
		if (truthtrk->charge() > 0) charge = 1.0;
		if (truthtrk->charge() < 0) charge = -1.0;

	}


	ATH_MSG_DEBUG("Filling run etc");
	m_runNumber = info->runNumber();
	m_eventNumber = info->eventNumber();
	if (isMC){
		m_mcChannelNumber = info->mcChannelNumber();
	}
	else {
		m_mcChannelNumber = m_runNumber;
	}
	m_lumiblock = info->lumiBlock();
	m_average_mu = info->averageInteractionsPerCrossing();
	m_actual_mu = info->actualInteractionsPerCrossing();
	m_mcEventWeight = (isMC ? info->mcEventWeight() : 1.00);
	m_BCID = info->bcid();
	m_pt = probe.pt();
	m_eta = probe.eta();
	if (muprobe){
		const xAOD::TrackParticle* tp = muprobe->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
		if (!tp){
			tp = muprobe->trackParticle(xAOD::Muon::CombinedTrackParticle);
		}
		if (tp){
			m_etaMS = tp->eta();
		}
		else{
			m_etaMS = muprobe->eta();
		}

	}
	else {
		m_etaMS = probe.eta();
	}
	m_phi = probe.phi();
	m_fineEtaPhi = m_fepb.bin(probe.probeTrack().p4());
	m_detregion = m_epb.symmetricBin(probe.probeTrack().p4());
	m_q = charge;
	m_tag_q = charge2;
	m_tagPt = probe.tagTrack().pt();
	m_tagEta = probe.tagTrack().eta();
	m_tagPhi = probe.tagTrack().phi();
	m_mll = (probe.tagTrack().p4()+probe.probeTrack().p4()).M();
	m_dilep_pt = (probe.tagTrack().p4()+probe.probeTrack().p4()).Pt();

	//if (!vx_tag) std::cout << "vx_tag non ok" << std::endl;
	//if (!(vx_tag == vx_probe)) std::cout << "vx_tag != vx_probe" << std::endl;
	//if (!pv) std::cout << "pv non ok" << std::endl;

	// if we have a valid decay vertex and primary vertex, compute the time of flight
	if (vx_tag && vx_tag == vx_probe && pv){
		TVector3 vvx_tag (vx_tag->x(),vx_tag->y(),vx_tag->z());
		TVector3 vpv (pv->x(),pv->y(),pv->z());
		m_dilep_tof = (vvx_tag - vpv).Perp() * 3096. / m_dilep_pt;
	}
	else{
		m_dilep_tof = -1;
	}
	m_dPhi = probe.tagTrack().p4().DeltaPhi(probe.probeTrack().p4());
	m_dEta = probe.tagTrack().eta()-probe.probeTrack().eta();
	ATH_MSG_DEBUG("Done with the custom tree contents");

}
void DiMuonTPTreeTool::AddCustomBranches(TTree* tree){

	tree->Branch("runNumber",&m_runNumber);
	tree->Branch("eventNumber",&m_eventNumber);
	tree->Branch("mcChannelNumber",&m_mcChannelNumber);
	tree->Branch("lumiblock",&m_lumiblock);
	tree->Branch("average_mu",&m_average_mu);
	tree->Branch("actual_mu",&m_actual_mu);
	tree->Branch("PV_n",&m_PV_n);
	tree->Branch("BCID",&m_BCID);
	tree->Branch("mcEventWeight",&m_mcEventWeight);
	tree->Branch("probe_pt",&m_pt);
	tree->Branch("probe_eta",&m_eta);
	tree->Branch("probe_etaMS",&m_etaMS);
	tree->Branch("probe_phi",&m_phi);
	tree->Branch("probe_fineEtaPhi",&m_fineEtaPhi);
	tree->Branch("probe_detRegion",&m_detregion);
	tree->Branch("probe_q",&m_q);
	tree->Branch("probe_dRJet",&m_deltaR);
	tree->Branch("probe_closestJetPt",&m_closest_jetpt);
	tree->Branch("probe_closestJetPtRel",&m_closest_jetptrel);
	tree->Branch("probe_leadingJetPtRel",&m_leading_jetptrel);
	tree->Branch("probe_sumJetPtRel",&m_sum_jetptrel);
	tree->Branch("probe_closestJetEMFrac",&m_closest_jetemfrac);
	tree->Branch("probe_leadingJetEMFrac",&m_leading_jetemfrac);
	tree->Branch("probe_closestJetChargedFrac",&m_closest_jetchargedfrac);
	tree->Branch("probe_leadingJetChargedFrac",&m_leading_jetchargedfrac);
	if (!m_is_truthprobe){
		tree->Branch("probe_type",&m_type);
		tree->Branch("probe_author",&m_author);
		tree->Branch("probe_quality",&m_quality);
		tree->Branch("probe_passesHighPt",&m_passHighPt);
		tree->Branch("probe_promtlepton_tagweight",&m_probe_pl_tagweight);
		tree->Branch("probe_promtlepton_trackjetntrack",&m_probe_pl_trackjetntrack);
		tree->Branch("probe_d0",&m_d0);
		tree->Branch("probe_d0err",&m_d0err);
		tree->Branch("probe_z0",&m_z0);
		if (m_is_on_DAOD){
			tree->Branch("probe_hasFSRcand",&m_probe_hasFSRcand);
			tree->Branch("probe_FSR_type",&m_probe_FSRtype);
			tree->Branch("probe_FSR_dR",&m_probe_FSR_dR);
			tree->Branch("probe_FSR_Et",&m_probe_FSR_Et);
			tree->Branch("probe_FSR_Eta",&m_probe_FSR_Eta);
			tree->Branch("probe_FSR_Phi",&m_probe_FSR_Phi);
			tree->Branch("probe_FSR_F1",&m_probe_FSR_F1);
			tree->Branch("probe_FSR_EtRel",&m_probe_FSR_Etrel);
			tree->Branch("probe_closestJetDrToFSR",&m_closest_jetDrToFsr);
			tree->Branch("probe_closestJetPtRelFSRFraction",&m_closest_jetPtRel_fsrfrac);
			tree->Branch("probe_closestJetPtFSRFraction",&m_closest_jetPt_fsrfrac);
			tree->Branch("probe_leadingJetDrToFSR",&m_leading_jetDrToFsr);
			tree->Branch("probe_leadingJetPtRelFSRFraction",&m_leading_jetPtRel_fsrfrac);
			tree->Branch("probe_leadingJetPtFSRFraction",&m_leading_jetPt_fsrfrac);
		}

		tree->Branch("probe_CaloLRLikelihood",&m_probe_CaloLRLikelihood);
		tree->Branch("probe_CaloMuonIDTag",&m_probe_CaloMuonIDTag);
		tree->Branch("probe_EnergyLoss",&m_probe_EnergyLoss);
		tree->Branch("probe_MeasEnergyLoss",&m_probe_MeasEnergyLoss);
		tree->Branch("probe_ParamEnergyLoss",&m_probe_ParamEnergyLoss);

		tree->Branch("probe_innerSmallHits",&m_innerSmallHits);
		tree->Branch("probe_innerLargeHits",&m_innerLargeHits);
		tree->Branch("probe_middleSmallHits",&m_middleSmallHits);
		tree->Branch("probe_middleLargeHits",&m_middleLargeHits);
		tree->Branch("probe_outerSmallHits",&m_outerSmallHits);
		tree->Branch("probe_outerLargeHits",&m_outerLargeHits);
		tree->Branch("probe_extendedSmallHits",&m_extendedSmallHits);
		tree->Branch("probe_extendedLargeHits",&m_extendedLargeHits);

		tree->Branch("probe_innerSmallHoles",&m_innerSmallHoles);
		tree->Branch("probe_innerLargeHoles",&m_innerLargeHoles);
		tree->Branch("probe_middleSmallHoles",&m_middleSmallHoles);
		tree->Branch("probe_middleLargeHoles",&m_middleLargeHoles);
		tree->Branch("probe_outerSmallHoles",&m_outerSmallHoles);
		tree->Branch("probe_outerLargeHoles",&m_outerLargeHoles);
		tree->Branch("probe_extendedSmallHoles",&m_extendedSmallHoles);
		tree->Branch("probe_extendedLargeHoles",&m_extendedLargeHoles);

		tree->Branch("probe_phiLayer1Holes",&m_phiLayer1Holes);
		tree->Branch("probe_phiLayer2Holes",&m_phiLayer2Holes);
		tree->Branch("probe_phiLayer3Holes",&m_phiLayer3Holes);
		tree->Branch("probe_phiLayer4Holes",&m_phiLayer4Holes);

		tree->Branch("probe_precisionLayers",&m_probe_precisionLayers);
		tree->Branch("probe_precisionHoleLayers",&m_probe_precisionHoleLayers);
		tree->Branch("probe_phiLayers",&m_probe_phiLayers);
		tree->Branch("probe_phiHoleLayers",&m_probe_phiHoleLayers);
		tree->Branch("probe_trigEtaLayers",&m_probe_trigEtaLayers);
		tree->Branch("probe_trigEtaHoleLayers",&m_probe_trigEtaHoleLayers);

		// new stuff
		tree->Branch("probe_numberOfGoodPrecisionLayers",&m_numberOfGoodPrecisionLayers);
		tree->Branch("probe_innerClosePrecisionHits",&m_innerClosePrecisionHits);
		tree->Branch("probe_middleClosePrecisionHits",&m_middleClosePrecisionHits);
		tree->Branch("probe_outerClosePrecisionHits",&m_outerClosePrecisionHits);
		tree->Branch("probe_extendedClosePrecisionHits",&m_extendedClosePrecisionHits);
		tree->Branch("probe_innerOutBoundsPrecisionHits",&m_innerOutBoundsPrecisionHits);
		tree->Branch("probe_middleOutBoundsPrecisionHits",&m_middleOutBoundsPrecisionHits);
		tree->Branch("probe_outerOutBoundsPrecisionHits",&m_outerOutBoundsPrecisionHits);
		tree->Branch("probe_extendedOutBoundsPrecisionHits",&m_extendedOutBoundsPrecisionHits);
		tree->Branch("probe_combinedTrackOutBoundsPrecisionHits",&m_combinedTrackOutBoundsPrecisionHits);
		tree->Branch("probe_isEndcapGoodLayers",&m_isEndcapGoodLayers);
		tree->Branch("probe_numberOfOutliersOnTrack",&m_numberOfOutliersOnTrack);
		tree->Branch("probe_isSmallGoodSectors",&m_isSmallGoodSectors);
		tree->Branch("probe_allauthors",&m_allauthors);
		tree->Branch("probe_qoverp_CB",&m_qoverp_CB);
		tree->Branch("probe_qoverp_err_CB",&m_qoverp_err_CB);
		tree->Branch("probe_qoverp_MS",&m_qoverp_MS);
		tree->Branch("probe_qoverp_err_MS",&m_qoverp_err_MS);
		tree->Branch("probe_qoverp_ME",&m_qoverp_ME);
		tree->Branch("probe_qoverp_err_ME",&m_qoverp_err_ME);
		tree->Branch("probe_qoverp_ID",&m_qoverp_ID);
		tree->Branch("probe_qoverp_err_ID",&m_qoverp_err_ID);
		tree->Branch("probe_chi2",&m_chi2);
		tree->Branch("probe_nDof",&m_nDof);
		tree->Branch("probe_momentumBalanceSignificance",&m_momentumBalanceSignificance);


		tree->Branch("tag_pt",&m_tagPt);
		tree->Branch("tag_eta",&m_tagEta);
		tree->Branch("tag_phi",&m_tagPhi);
		tree->Branch("tag_q",&m_tag_q);
		tree->Branch("tag_d0",&m_tag_d0);
		tree->Branch("tag_d0err",&m_tag_d0err);
		tree->Branch("tag_z0",&m_tag_z0);
		tree->Branch("tag_dRJet",&m_tag_deltaR);
		tree->Branch("tag_closestJetPt",&m_tag_jetpt);
		tree->Branch("tag_promtlepton_tagweight",&m_tag_pl_tagweight);
		tree->Branch("tag_promtlepton_trackjetntrack",&m_tag_pl_trackjetntrack);

		tree->Branch("dilep_mll",&m_mll);
		tree->Branch("dilep_dphi",&m_dPhi);
		tree->Branch("dilep_deta",&m_dEta);
		tree->Branch("dilep_dphi_exTP",&m_dPhi_Pivot);
		tree->Branch("dilep_deta_exTP",&m_dEta_Pivot);
        tree->Branch("dilep_dR_exTP",&m_dR_Pivot);
		tree->Branch("probe_phi_exTP",&m_Phi_Pivot);
		tree->Branch("probe_eta_exTP",&m_Eta_Pivot);
		tree->Branch("dilep_pt",&m_dilep_pt);
		tree->Branch("dilep_tof",&m_dilep_tof);
	}

	tree->Branch("EventBadMuonVetoVar_VeryLoose_ME",&m_EventBadMuonVetoVar_VeryLoose_ME);
	tree->Branch("EventBadMuonVetoVar_VeryLoose_ID",&m_EventBadMuonVetoVar_VeryLoose_ID);

	tree->Branch("EventBadMuonVetoVar_Loose_ME",&m_EventBadMuonVetoVar_Loose_ME);
	tree->Branch("EventBadMuonVetoVar_Loose_ID",&m_EventBadMuonVetoVar_Loose_ID);


	tree->Branch("EventBadMuonVetoVar_Medium_ME",&m_EventBadMuonVetoVar_Medium_ME);
	tree->Branch("EventBadMuonVetoVar_Medium_ID",&m_EventBadMuonVetoVar_Medium_ID);


	tree->Branch("EventBadMuonVetoVar_Tight_ME",&m_EventBadMuonVetoVar_Tight_ME);
	tree->Branch("EventBadMuonVetoVar_Tight_ID",&m_EventBadMuonVetoVar_Tight_ID);


	tree->Branch("EventBadMuonVetoVar_HighPt_ME",&m_EventBadMuonVetoVar_HighPt_ME);
	tree->Branch("EventBadMuonVetoVar_HighPt_ID",&m_EventBadMuonVetoVar_HighPt_ID);


	if(!m_is_truthprobe &&  m_add_Truth_Matching) {
		tree->Branch("tag_matched_TruthMuons", &m_tag_isTruthMatched);
		tree->Branch("probe_matched_TruthMuons", &m_probe_isTruthMatched);
		tree->Branch("probe_truth_pdgId", &m_probe_truth_pdgId);
		tree->Branch("probe_truth_type", &m_probe_truth_type);
		tree->Branch("probe_truth_origin", &m_probe_truth_origin);
		tree->Branch("tag_truth_pdgId", &m_tag_truth_pdgId);
		tree->Branch("tag_truth_type", &m_tag_truth_type);
		tree->Branch("tag_truth_origin", &m_tag_truth_origin);
	}

	if (!m_is_truthprobe && m_extended_Iso_Info){
		tree->Branch("tag_neflowisol20",&m_tagiso_neflowisol20);
		tree->Branch("tag_topoetcone20",&m_tagiso_topoetcone20);
		tree->Branch("tag_ptcone20",&m_tagiso_ptcone20);
		tree->Branch("tag_etcone20",&m_tagiso_etcone20);
		tree->Branch("tag_ptvarcone20",&m_tagiso_ptvarcone20);

		tree->Branch("tag_neflowisol30",&m_tagiso_neflowisol30);
		tree->Branch("tag_topoetcone30",&m_tagiso_topoetcone30);
		tree->Branch("tag_ptcone30",&m_tagiso_ptcone30);
		tree->Branch("tag_etcone30",&m_tagiso_etcone30);
		tree->Branch("tag_ptvarcone30",&m_tagiso_ptvarcone30);

		tree->Branch("tag_neflowisol40",&m_tagiso_neflowisol40);
		tree->Branch("tag_topoetcone40",&m_tagiso_topoetcone40);
		tree->Branch("tag_ptcone40",&m_tagiso_ptcone40);
		tree->Branch("tag_etcone40",&m_tagiso_etcone40);
		tree->Branch("tag_ptvarcone40",&m_tagiso_ptvarcone40);

		tree->Branch("tag_topocore",&m_tag_topocore);

		tree->Branch("probe_neflowisol20",&m_probeiso_neflowisol20);
		tree->Branch("probe_topoetcone20",&m_probeiso_topoetcone20);
		tree->Branch("probe_ptcone20",&m_probeiso_ptcone20);
		tree->Branch("probe_etcone20",&m_probeiso_etcone20);
		tree->Branch("probe_ptvarcone20",&m_probeiso_ptvarcone20);

		tree->Branch("probe_neflowisol30",&m_probeiso_neflowisol30);
		tree->Branch("probe_topoetcone30",&m_probeiso_topoetcone30);
		tree->Branch("probe_ptcone30",&m_probeiso_ptcone30);
		tree->Branch("probe_etcone30",&m_probeiso_etcone30);
		tree->Branch("probe_ptvarcone30",&m_probeiso_ptvarcone30);

		tree->Branch("probe_neflowisol40",&m_probeiso_neflowisol40);
		tree->Branch("probe_topoetcone40",&m_probeiso_topoetcone40);
		tree->Branch("probe_ptcone40",&m_probeiso_ptcone40);
		tree->Branch("probe_etcone40",&m_probeiso_etcone40);
		tree->Branch("probe_ptvarcone40",&m_probeiso_ptvarcone40);

		tree->Branch("probe_topocore",&m_probe_topocore);

		tree->Branch("energyDensity_central",&m_energyDensity_central);
		tree->Branch("energyDensity_forward",&m_energyDensity_forward);
	}

	if(!m_is_truthprobe && m_extended_Trig_Info) {
		tree->Branch("probe_primarySector",&m_primarySector);
		tree->Branch("probe_secondarySector",&m_secondarySector);
		tree->Branch("probe_phiLayer1Hits",&m_phiLayer1Hits);
		tree->Branch("probe_phiLayer2Hits",&m_phiLayer2Hits);
		tree->Branch("probe_phiLayer3Hits",&m_phiLayer3Hits);
		tree->Branch("probe_phiLayer4Hits",&m_phiLayer4Hits);
	}
    if (m_write_matchobj_info){
    	AddBranchesForMatch(m_matchobj_combinedTrackOutBoundsPrecisionHits	,tree,"combinedTrackOutBoundsPrecisionHits");
    	AddBranchesForMatch(m_matchobj_innerClosePrecisionHits				,tree,"innerClosePrecisionHits");
    	AddBranchesForMatch(m_matchobj_middleClosePrecisionHits				,tree,"middleClosePrecisionHits");
    	AddBranchesForMatch(m_matchobj_outerClosePrecisionHits				,tree,"outerClosePrecisionHits");
    	AddBranchesForMatch(m_matchobj_extendedClosePrecisionHits			,tree,"extendedClosePrecisionHits");
    	AddBranchesForMatch(m_matchobj_innerOutBoundsPrecisionHits			,tree,"innerOutBoundsPrecisionHits");
    	AddBranchesForMatch(m_matchobj_middleOutBoundsPrecisionHits			,tree,"middleOutBoundsPrecisionHits");
    	AddBranchesForMatch(m_matchobj_outerOutBoundsPrecisionHits			,tree,"outerOutBoundsPrecisionHits");
    	AddBranchesForMatch(m_matchobj_extendedOutBoundsPrecisionHits		,tree,"extendedOutBoundsPrecisionHits");
    	AddBranchesForMatch(m_matchobj_numberOfOutliersOnTrack				,tree,"numberOfOutliersOnTrack");
    }

}

void DiMuonTPTreeTool::AddBranchesForMatch(std::map<std::string, int> & thing, TTree* tree, std::string name){
	for (auto & flag : m_reco_match_flags){
		thing[flag] = -1;
		tree->Branch((std::string("matchobj_")+flag+"_"+name).c_str(),&(thing[flag]));
	}
}

void DiMuonTPTreeTool::FillMuProbeTrkDetails(const xAOD::Muon* muprobe){

	const xAOD::TrackParticle* CBtrk = muprobe->trackParticle(xAOD::Muon::CombinedTrackParticle);
	const xAOD::TrackParticle* MStrk = muprobe->trackParticle(xAOD::Muon::MuonSpectrometerTrackParticle);
	const xAOD::TrackParticle* MEtrk = muprobe->trackParticle(xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle);
	const xAOD::TrackParticle* IDtrk = muprobe->trackParticle(xAOD::Muon::InnerDetectorTrackParticle);
	const xAOD::TrackParticle* primary = muprobe->primaryTrackParticle();

	FillQoverP(CBtrk, m_qoverp_CB, m_qoverp_err_CB);
	FillQoverP(MStrk, m_qoverp_MS, m_qoverp_err_MS);
	FillQoverP(MEtrk, m_qoverp_ME, m_qoverp_err_ME);
	FillQoverP(IDtrk, m_qoverp_ID, m_qoverp_err_ID);

	if (primary){
		m_chi2 = primary->chiSquared();
		m_nDof = primary->numberDoF();
	}
}

void DiMuonTPTreeTool::FillQoverP(const xAOD::TrackParticle* TP, float & qoverp, float & delta_qoverp){

	if (TP == 0){
		qoverp = -999;
		delta_qoverp = -999.;
	}
	else{
		qoverp = TP->qOverP();
		delta_qoverp = sqrt(TP->definingParametersCovMatrix()(4,4));
		ATH_MSG_DEBUG("qoverp = "<<qoverp<< " with error "<<delta_qoverp);
	}

}
void DiMuonTPTreeTool::FillMuProbeHitInfo(const xAOD::Muon* muprobe){

	m_primarySector = muprobe->uint8MuonSummaryValue (xAOD::MuonSummaryType::primarySector);
	m_secondarySector = muprobe->uint8MuonSummaryValue (xAOD::MuonSummaryType::secondarySector);
	m_phiLayer1Hits = muprobe->uint8MuonSummaryValue (xAOD::MuonSummaryType::phiLayer1Hits);
	m_phiLayer2Hits = muprobe->uint8MuonSummaryValue (xAOD::MuonSummaryType::phiLayer2Hits);
	m_phiLayer3Hits = muprobe->uint8MuonSummaryValue (xAOD::MuonSummaryType::phiLayer3Hits);
	m_phiLayer4Hits = muprobe->uint8MuonSummaryValue (xAOD::MuonSummaryType::phiLayer4Hits);

	m_allauthors = muprobe->allAuthors();

	if(!muprobe->parameter(m_momentumBalanceSignificance, xAOD::Muon::momentumBalanceSignificance)){
		m_momentumBalanceSignificance = -1;
	}
	if(!muprobe->summaryValue(m_probe_precisionLayers, xAOD::SummaryType::numberOfPrecisionLayers)){
		m_probe_precisionLayers = -1;
	}
	if(!muprobe->summaryValue(m_probe_precisionHoleLayers, xAOD::SummaryType::numberOfPrecisionHoleLayers)){
		m_probe_precisionHoleLayers = -1;
	}
	if(!muprobe->summaryValue(m_probe_phiLayers, xAOD::SummaryType::numberOfPhiLayers)){
		m_probe_phiLayers = -1;
	}
	if(!muprobe->summaryValue(m_probe_phiHoleLayers, xAOD::SummaryType::numberOfPhiHoleLayers)){
		m_probe_phiHoleLayers = -1;
	}
	if(!muprobe->summaryValue(m_probe_trigEtaLayers, xAOD::SummaryType::numberOfTriggerEtaLayers)){
		m_probe_trigEtaLayers = -1;
	}
	if(!muprobe->summaryValue(m_probe_trigEtaHoleLayers, xAOD::SummaryType::numberOfTriggerEtaHoleLayers)){
		m_probe_trigEtaHoleLayers = -1;
	}

	if(!muprobe->summaryValue(m_innerSmallHits,xAOD::MuonSummaryType::innerSmallHits)){
		m_innerSmallHits = -1;
	}
	if(!muprobe->summaryValue(m_innerLargeHits,xAOD::MuonSummaryType::innerLargeHits)){
		m_innerLargeHits = -1;
	}
	if(!muprobe->summaryValue(m_middleSmallHits,xAOD::MuonSummaryType::middleSmallHits)){
		m_middleSmallHits = -1;
	}
	if(!muprobe->summaryValue(m_middleLargeHits,xAOD::MuonSummaryType::middleLargeHits)){
		m_middleLargeHits = -1;
	}
	if(!muprobe->summaryValue(m_outerSmallHits,xAOD::MuonSummaryType::outerSmallHits)){
		m_outerSmallHits = -1;
	}
	if(!muprobe->summaryValue(m_outerLargeHits,xAOD::MuonSummaryType::outerLargeHits)){
		m_outerLargeHits = -1;
	}
	if(!muprobe->summaryValue(m_extendedSmallHits,xAOD::MuonSummaryType::extendedSmallHits)){
		m_extendedSmallHits = -1;
	}
	if(!muprobe->summaryValue(m_extendedLargeHits,xAOD::MuonSummaryType::extendedLargeHits)){
		m_extendedLargeHits = -1;
	}

	if(!muprobe->summaryValue(m_innerSmallHoles,xAOD::MuonSummaryType::innerSmallHoles)){
		m_innerSmallHoles = -1;
	}
	if(!muprobe->summaryValue(m_innerLargeHoles,xAOD::MuonSummaryType::innerLargeHoles)){
		m_innerLargeHoles = -1;
	}
	if(!muprobe->summaryValue(m_middleSmallHoles,xAOD::MuonSummaryType::middleSmallHoles)){
		m_middleSmallHoles = -1;
	}
	if(!muprobe->summaryValue(m_middleLargeHoles,xAOD::MuonSummaryType::middleLargeHoles)){
		m_middleLargeHoles = -1;
	}
	if(!muprobe->summaryValue(m_outerSmallHoles,xAOD::MuonSummaryType::outerSmallHoles)){
		m_outerSmallHoles = -1;
	}
	if(!muprobe->summaryValue(m_outerLargeHoles,xAOD::MuonSummaryType::outerLargeHoles)){
		m_outerLargeHoles = -1;
	}
	if(!muprobe->summaryValue(m_extendedSmallHoles,xAOD::MuonSummaryType::extendedSmallHoles)){
		m_extendedSmallHoles = -1;
	}
	if(!muprobe->summaryValue(m_extendedLargeHoles,xAOD::MuonSummaryType::extendedLargeHoles)){
		m_extendedLargeHoles = -1;
	}
	if(!muprobe->summaryValue(m_phiLayer1Holes,xAOD::MuonSummaryType::phiLayer1Holes)){
		m_phiLayer1Holes = -1;
	}
	if(!muprobe->summaryValue(m_phiLayer2Holes,xAOD::MuonSummaryType::phiLayer2Holes)){
		m_phiLayer2Holes = -1;
	}
	if(!muprobe->summaryValue(m_phiLayer3Holes,xAOD::MuonSummaryType::phiLayer3Holes)){
		m_phiLayer3Holes = -1;
	}
	if(!muprobe->summaryValue(m_phiLayer4Holes,xAOD::MuonSummaryType::phiLayer4Holes)){
		m_phiLayer4Holes = -1;
	}
	if (! muprobe->parameter(m_probe_EnergyLoss,xAOD::Muon::EnergyLoss)) {
		m_probe_EnergyLoss = -1;
	}
	if (! muprobe->parameter(m_probe_ParamEnergyLoss,xAOD::Muon::ParamEnergyLoss)) {
		m_probe_ParamEnergyLoss = -1;
	}
	if (! muprobe->parameter(m_probe_MeasEnergyLoss,xAOD::Muon::MeasEnergyLoss)) {
		m_probe_MeasEnergyLoss = -1;
	}
	if (! muprobe->parameter(m_probe_CaloMuonIDTag,xAOD::Muon::CaloMuonIDTag)) {
		m_probe_CaloMuonIDTag = -1;
	}
	if (! muprobe->parameter(m_probe_CaloLRLikelihood,xAOD::Muon::CaloLRLikelihood)) {
		m_probe_CaloLRLikelihood = -1;
	}

	if (! muprobe->summaryValue(m_numberOfGoodPrecisionLayers,xAOD::SummaryType::numberOfGoodPrecisionLayers)) {
		m_numberOfGoodPrecisionLayers = -1;
	}
	if (! muprobe->summaryValue(m_innerClosePrecisionHits,xAOD::MuonSummaryType::innerClosePrecisionHits )) {
		m_innerClosePrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_middleClosePrecisionHits,xAOD::MuonSummaryType::middleClosePrecisionHits )) {
		m_middleClosePrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_outerClosePrecisionHits,xAOD::MuonSummaryType::outerClosePrecisionHits )) {
		m_outerClosePrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_extendedClosePrecisionHits,xAOD::MuonSummaryType::extendedClosePrecisionHits )) {
		m_extendedClosePrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_innerOutBoundsPrecisionHits,xAOD::MuonSummaryType::innerOutBoundsPrecisionHits )) {
		m_innerOutBoundsPrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_middleOutBoundsPrecisionHits,xAOD::MuonSummaryType::middleOutBoundsPrecisionHits )) {
		m_middleOutBoundsPrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_outerOutBoundsPrecisionHits,xAOD::MuonSummaryType::outerOutBoundsPrecisionHits )) {
		m_outerOutBoundsPrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_extendedOutBoundsPrecisionHits,xAOD::MuonSummaryType::extendedOutBoundsPrecisionHits )) {
		m_extendedOutBoundsPrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_combinedTrackOutBoundsPrecisionHits,xAOD::MuonSummaryType::combinedTrackOutBoundsPrecisionHits )) {
		m_combinedTrackOutBoundsPrecisionHits = -1;
	}
	if (! muprobe->summaryValue(m_isEndcapGoodLayers,xAOD::MuonSummaryType::isEndcapGoodLayers )) {
		m_isEndcapGoodLayers = -1;
	}
	if (! muprobe->summaryValue(m_isSmallGoodSectors,xAOD::MuonSummaryType::isSmallGoodSectors )) {
		m_isSmallGoodSectors = -1;
	}
	if (! muprobe->summaryValue(m_numberOfOutliersOnTrack,xAOD::SummaryType::numberOfOutliersOnTrack )) {
		m_numberOfOutliersOnTrack = -1;
	}

}
void DiMuonTPTreeTool::FillMatchObjectInfo(Probe & probe){

	const xAOD::IParticle & probetrk = probe.probeTrack();

	for (auto & flag : m_reco_match_flags){
		SG::AuxElement::Accessor< ElementLink<xAOD::IParticleContainer > > Match_object(std::string("match_ptr_")+flag);
		const xAOD::Muon* matchobj = 0;
		if (Match_object.isAvailable(probetrk)){
			try{
				ElementLink<xAOD::IParticleContainer > matchMu = Match_object(probetrk);
				if (matchMu.isValid()){
					matchobj = dynamic_cast<const xAOD::Muon*> (*matchMu);
				}
			}
			catch (SG::ExcBadAuxVar & ){
				// no need to do anything here
				ATH_MSG_DEBUG("Missing match muon element link for effi flag "<<flag);
			}
		}
		m_matchobj_combinedTrackOutBoundsPrecisionHits[flag] = -1 ;
		m_matchobj_innerClosePrecisionHits[flag] 		     = -1 ;
		m_matchobj_middleClosePrecisionHits[flag] 			 = -1 ;
		m_matchobj_outerClosePrecisionHits[flag] 			 = -1 ;
		m_matchobj_extendedClosePrecisionHits[flag] 		 = -1 ;
		m_matchobj_innerOutBoundsPrecisionHits[flag] 	 	 = -1 ;
		m_matchobj_middleOutBoundsPrecisionHits[flag] 		 = -1 ;
		m_matchobj_outerOutBoundsPrecisionHits[flag] 		 = -1 ;
		m_matchobj_extendedOutBoundsPrecisionHits[flag] 	 = -1 ;
		m_matchobj_numberOfOutliersOnTrack[flag] 			 = -1 ;

		if (matchobj){
			uint8_t aux = 0;
			if(matchobj->summaryValue(aux, xAOD::MuonSummaryType::combinedTrackOutBoundsPrecisionHits)){
				m_matchobj_combinedTrackOutBoundsPrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::innerClosePrecisionHits)){
				m_matchobj_innerClosePrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::middleClosePrecisionHits)){
				m_matchobj_middleClosePrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::outerClosePrecisionHits)){
				m_matchobj_outerClosePrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::extendedClosePrecisionHits)){
				m_matchobj_extendedClosePrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::MuonSummaryType::innerOutBoundsPrecisionHits)){
				m_matchobj_innerOutBoundsPrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::MuonSummaryType::middleOutBoundsPrecisionHits)){
				m_matchobj_middleOutBoundsPrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::MuonSummaryType::outerOutBoundsPrecisionHits)){
				m_matchobj_outerOutBoundsPrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::MuonSummaryType::extendedOutBoundsPrecisionHits)){
				m_matchobj_extendedOutBoundsPrecisionHits[flag] = aux;
			}
			if(matchobj->summaryValue(aux, xAOD::SummaryType::numberOfOutliersOnTrack)){
				m_matchobj_numberOfOutliersOnTrack[flag] = aux;
			}
		}
	}
}

void DiMuonTPTreeTool::FillEventShapeInfo(){

	// Event shape variables for isolation
	const xAOD::EventShape* edShape = 0;
	double rho = 0.;
	if (evtStore()->retrieve(edShape,"TopoClusterIsoCentralEventShape").isFailure()){  // Returns a boolean, needs to be surrounded with error checks.
		ATH_MSG_FATAL( "Unable to retrieve TopoClusterIsoCentralEventShape" );
	} else {
		edShape->getDensity(xAOD::EventShape::Density,rho); // Returns a boolean, needs to be surrounded with error checks.
		m_energyDensity_central = rho;
	}
	if (evtStore()->retrieve(edShape,"TopoClusterIsoForwardEventShape").isFailure()){  // Returns a boolean, needs to be surrounded with error checks.
		ATH_MSG_FATAL( "Unable to retrieve TopoClusterIsoForwardEventShape" );
	} else {
		edShape->getDensity(xAOD::EventShape::Density,rho); // Returns a boolean, needs to be surrounded with error checks.
		m_energyDensity_forward = rho;
	}

	//    // FCAL sum et for heavy ion analyses
	//    m_FCAL_Et = 0;
	//    const xAOD::HIEventShapeContainer *hiue(0);
	//
	//    if ( evtStore()->retrieve(hiue, "CaloSums").isFailure() ) {
	//        ATH_MSG_WARNING( "Unable to retrieve CaloSums container" );
	//        return;
	//    }
	//    // find the index in the vector where the FCal sum is stored
	//    static const size_t i_fcal = std::distance( hiue->cbegin(),
	//            std::find_if (hiue->cbegin(),hiue->cend(), [=](const xAOD::HIEventShape* evt) {
	//                                                        return evt->auxdataConst<std::string>("Summary") == "FCal";}
	//                        ));
	//    // and retrieve it
	//    if (i_fcal < hiue->size()){
	//        m_FCAL_Et = hiue->at(i_fcal)->et()*1e-6;
	//    }
	//    else {
	//        ATH_MSG_WARNING("Unable to find FCal sumet in CaloSums container");
	//        m_FCAL_Et = -1;
	//    }

}

void DiMuonTPTreeTool::FillFSRInfo(Probe & probe){

	// reset info

	m_probe_hasFSRcand = false;
	m_probe_FSRtype = -1;
	m_probe_FSR_dR = -1;
	m_probe_FSR_Et = -1;
	m_probe_FSR_Eta = -11;
	m_probe_FSR_Phi = -11;
	m_probe_FSR_F1 = -1;
	m_probe_FSR_Etrel = -1000000;

	if(m_is_on_DAOD){
		ATH_MSG_DEBUG("Skipping FSR since this is a DAOD!");
		return;
	}
	ATH_MSG_DEBUG("Skipping FSR since this is NOT a DAOD!");
	// check for muon probe
	FSR::FsrCandidate candidate;
	TLorentzVector FSRlv;
	const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());

	// check for FSR
	if (muprobe &&  m_IFsrPhotonTool->getFsrPhoton(muprobe, candidate) ==  CP::CorrectionCode::Ok){
		ATH_MSG_DEBUG( " FSR candidate found, container = " << candidate.container
				//const xAOD::IParticle* particle;
				<< " deltaR = " << candidate.deltaR
				<< " Et = " << candidate.Et
				<<" f1 = " << candidate.f1
				<<" eta = " << candidate.eta
				<<" phi = " << candidate.phi
				<<" phIso = "<< candidate.phIso
				<<" fsrtype = " << candidate.type
		);
		m_probe_hasFSRcand = true;
		m_probe_FSRtype = candidate.type;
		m_FSR_candidate.SetPtEtaPhiM(candidate.Et,candidate.eta,candidate.phi,0);
		m_probe_FSR_dR = FSRlv.DeltaR(muprobe->p4());
		m_probe_FSR_Et = m_FSR_candidate.Et();
		m_probe_FSR_Eta = m_FSR_candidate.Eta();
		m_probe_FSR_Phi = m_FSR_candidate.Phi();
		m_probe_FSR_F1 = candidate.f1;
		m_probe_FSR_Etrel = m_FSR_candidate.Pt()*cos(probe.probeTrack().p4().Vect().Angle(m_FSR_candidate.Vect()));

	}


}

void DiMuonTPTreeTool::FillJetAssocInfo(Probe & probe){

	// distance to clostes jet
	m_deltaR = 99.;
	m_tag_deltaR = 99.;
	const xAOD::JetContainer* jets = 0;
	if (evtStore()->retrieve(jets,"AntiKt4EMTopoJets").isFailure()){
		ATH_MSG_WARNING( "Unable to retrieve Jet Container" );
	}
	else{
		// calculate delta R to the closest jet
		m_deltaR=-1;
		m_tag_deltaR=-1;

		float probe_deltaR = 0;
		float tag_deltaR = 0;
		m_tag_jetpt = -1;
		m_closest_jetpt = -1;
		float ptmax = 0;
		m_sum_jetptrel = -1;
		m_closest_jetpt = -1;
		m_closest_jetptrel = -1;
		m_closest_jetemfrac = -1;
		m_leading_jetptrel = -1;
		m_leading_jetemfrac = -1;

		m_closest_jetDrToFsr = -1;
		m_closest_jetPtRel_fsrfrac = -10000;
		m_closest_jetPt_fsrfrac = -10000;

		m_leading_jetDrToFsr = -1;
		m_leading_jetPtRel_fsrfrac = -10000;
		m_leading_jetPt_fsrfrac = -10000;

		for(auto jet : *jets) {
			if(m_jet_cleaning_tool->keep(*jet) && jet->pt() > 20000.){
				std::vector<float> sumPtTrkvec;
				jet->getAttribute( xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkvec );
				double chf = 0;
				if( ! sumPtTrkvec.empty() ) chf = sumPtTrkvec[0]/ jet->pt();
				m_sum_jetptrel += jet->pt()*cos(probe.probeTrack().p4().Vect().Angle(jet->p4().Vect()));
				if (jet->pt() > ptmax){
					ptmax = jet->pt();
					m_leading_jetptrel = jet->pt()*cos(probe.probeTrack().p4().Vect().Angle(jet->p4().Vect()));
					m_leading_jetemfrac= jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
					m_leading_jetchargedfrac = chf;
					if (m_probe_hasFSRcand){
						m_leading_jetDrToFsr = jet->p4().DeltaR(m_FSR_candidate);
						m_leading_jetPtRel_fsrfrac = m_probe_FSR_Etrel / m_leading_jetptrel ;
						m_leading_jetPt_fsrfrac = m_probe_FSR_Et / jet->p4().Pt();
					}
				}
				probe_deltaR = probe.probeTrack().p4().DeltaR(jet->p4());
				if (probe_deltaR < m_deltaR || m_deltaR < 0){
					m_deltaR = probe_deltaR;
					m_closest_jetpt = jet->pt();
					m_closest_jetptrel = jet->pt()*cos(probe.probeTrack().p4().Vect().Angle(jet->p4().Vect()));
					m_closest_jetemfrac= jet->getAttribute<float>(xAOD::JetAttribute::EMFrac);
					m_closest_jetchargedfrac = chf;
					if (m_probe_hasFSRcand){
						m_closest_jetDrToFsr = jet->p4().DeltaR(m_FSR_candidate);
						m_closest_jetPtRel_fsrfrac = m_probe_FSR_Etrel / m_closest_jetptrel ;
						m_closest_jetPt_fsrfrac = m_probe_FSR_Et / jet->p4().Pt();
					}
				}
				tag_deltaR = probe.tagTrack().p4().DeltaR(jet->p4());
				if (tag_deltaR < m_tag_deltaR || m_tag_deltaR < 0){
					m_tag_deltaR = tag_deltaR;
					m_tag_jetpt = jet->pt();
				}
			}
		}
	}
}
void DiMuonTPTreeTool::FillPromptLeptonTaggerInfo(Probe & probe){

	static SG::AuxElement::Accessor< float > DecTag ("PromptLepton_TagWeight");
	static SG::AuxElement::Accessor< short > DecNtracks ("PromptLepton_TrackJetNTrack");

	const xAOD::Muon* mutag = dynamic_cast<const xAOD::Muon*>(&probe.tagTrack());
	const xAOD::Muon* muprobe = dynamic_cast<const xAOD::Muon*>(&probe.probeTrack());

	m_tag_pl_tagweight = -1.;
	m_tag_pl_trackjetntrack = -1;
	m_probe_pl_tagweight = -1.;
	m_probe_pl_trackjetntrack = -1;
    try {
    	if (mutag && DecTag.isAvailable(*mutag)){
    		m_tag_pl_tagweight = DecTag(*mutag);
    		m_tag_pl_trackjetntrack = DecNtracks(*mutag);
    	}
    	if (muprobe && DecTag.isAvailable(*muprobe)){
    		m_probe_pl_tagweight = DecTag(*muprobe);
    		m_probe_pl_trackjetntrack = DecNtracks(*muprobe);
    	}
    }
    catch (SG::ExcBadAuxVar & err){
        ATH_MSG_WARNING("Caught exception when accessing prompt lepton tagger info"<< err.what()<< "---> Filling dummy info. Should be fixed within the prompt lepton tagger!"); 
        m_tag_pl_tagweight = -1;
        m_tag_pl_trackjetntrack = -1;
        m_probe_pl_tagweight = -1;
        m_probe_pl_trackjetntrack = -1;
    }

}
