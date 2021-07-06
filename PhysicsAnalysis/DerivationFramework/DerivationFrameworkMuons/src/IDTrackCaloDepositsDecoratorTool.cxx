/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/IDTrackCaloDepositsDecoratorTool.h"
#include "muonEvent/DepositInCalo.h"
#include "CaloEvent/CaloCellContainer.h"


#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"

IDTrackCaloDepositsDecoratorTool::IDTrackCaloDepositsDecoratorTool(const std::string& myname) :
		AsgTool(myname),
		m_trkDepositInCalo("TrackDepositInCaloTool/TrackDepositInCaloTool"),
		m_dec_EMB1_dep("EMB1_dep"),
		m_dec_EMB1_eloss("EMB1_eloss"),
		m_dec_EMB2_dep("EMB2_dep"),
		m_dec_EMB2_eloss("EMB2_eloss"),
		m_dec_EMB3_dep("EMB3_dep"),
		m_dec_EMB3_eloss("EMB3_eloss"),
		m_dec_EME1_dep("EME1_dep"),
		m_dec_EME1_eloss("EME1_eloss"),
		m_dec_EME2_dep("EME2_dep"),
		m_dec_EME2_eloss("EME2_eloss"),
		m_dec_EME3_dep("EME3_dep"),
		m_dec_EME3_eloss("EME3_eloss"),
		m_dec_TileBar0_dep("TileBar0_dep"),
		m_dec_TileBar0_eloss("TileBar0_eloss"),
		m_dec_TileBar1_dep("TileBar1_dep"),
		m_dec_TileBar1_eloss("TileBar1_eloss"),
		m_dec_TileBar2_dep("TileBar2_dep"),
		m_dec_TileBar2_eloss("TileBar2_eloss"),
		m_dec_TileExt0_dep("TileExt0_dep"),
		m_dec_TileExt0_eloss("TileExt0_eloss"),
		m_dec_TileExt1_dep("TileExt1_dep"),
		m_dec_TileExt1_eloss("TileExt1_eloss"),
		m_dec_TileExt2_dep("TileExt2_dep"),
		m_dec_TileExt2_eloss("TileExt2_eloss"),
		m_dec_HEC0_dep("HEC0_dep"),
		m_dec_HEC0_eloss("HEC0_eloss"),
		m_dec_HEC1_dep("HEC1_dep"),
		m_dec_HEC1_eloss("HEC1_eloss"),
		m_dec_HEC2_dep("HEC2_dep"),
		m_dec_HEC2_eloss("HEC2_eloss"),
		m_dec_HEC3_dep("HEC3_dep"),
		m_dec_HEC3_eloss("HEC3_eloss"){
	declareProperty("TrackDepositInCaloTool", m_trkDepositInCalo);
}

StatusCode IDTrackCaloDepositsDecoratorTool::initialize() {
	ATH_CHECK(m_trkDepositInCalo.retrieve());
	return StatusCode::SUCCESS;

}

StatusCode IDTrackCaloDepositsDecoratorTool::decorate(const xAOD::IParticle* part) const 
{
	static const  SG::AuxElement::ConstAccessor< bool > appliedDec( "AppliedIso" );

	// remember if we already decorated a track, saves CPU time
	if (part->isAvailable<bool>("AppliedCaloDep") && part->auxdataConst<bool>("AppliedCaloDep")){
		ATH_MSG_DEBUG("Already decorated this track!");
		return StatusCode::SUCCESS;
	}

	return recompute_and_decorate(part);
}

StatusCode IDTrackCaloDepositsDecoratorTool::recompute_and_decorate(const xAOD::IParticle* part) const
{
	static const SG::AuxElement::Decorator< bool > appliedDec( "AppliedCaloDep" );
	ATH_MSG_DEBUG("Recomputing calo deposition by hand");

	const xAOD::TrackParticle* tp = dynamic_cast<const xAOD::TrackParticle*>(part);
	if(!tp) {
		const xAOD::Muon* muon = dynamic_cast<const xAOD::Muon*>(part);
		if (muon) {
			tp = muon->trackParticle(xAOD::Muon::Primary);
		}
	}

	if(!tp) {
		ATH_MSG_ERROR("Unable to retrieve xAOD::TrackParticle from probe object");
		return StatusCode::FAILURE;
	}

	const CaloCellContainer* caloCellCont = nullptr;
	std::vector<DepositInCalo> deposits = m_trkDepositInCalo->getDeposits(&(tp->perigeeParameters()), caloCellCont);
	appliedDec(*part) = true;
	
	for (const auto& it : deposits)  {
		CaloCell_ID::CaloSample sample = it.subCaloId();

		float dep   = it.energyDeposited();
		float eloss = it.muonEnergyLoss();

		if (sample==CaloCell_ID::EMB1) {
			m_dec_EMB1_dep(*part) = dep;
			m_dec_EMB1_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::EMB2) {
			m_dec_EMB2_dep(*part) = dep;
			m_dec_EMB2_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::EMB3) {
			m_dec_EMB3_dep(*part) = dep;
			m_dec_EMB3_eloss(*part) = eloss;
		}

		else if (sample==CaloCell_ID::EME1) {
			m_dec_EME1_dep(*part) = dep;
			m_dec_EME1_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::EME2) {
			m_dec_EME2_dep(*part) = dep;
			m_dec_EME2_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::EME3) {
			m_dec_EME3_dep(*part) = dep;
			m_dec_EME3_eloss(*part) = eloss;
		}


		else if (sample==CaloCell_ID::TileBar0) {
			m_dec_TileBar0_dep(*part) = dep;
			m_dec_TileBar0_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::TileBar1) {
			m_dec_TileBar1_dep(*part) = dep;
			m_dec_TileBar1_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::TileBar2) {
			m_dec_TileBar2_dep(*part) = dep;
			m_dec_TileBar2_eloss(*part) = eloss;
		}


		else if (sample==CaloCell_ID::TileExt0) {
			m_dec_TileExt0_dep(*part) = dep;
			m_dec_TileExt0_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::TileExt1) {
			m_dec_TileExt1_dep(*part) = dep;
			m_dec_TileExt1_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::TileExt2) {
			m_dec_TileExt2_dep(*part) = dep;
			m_dec_TileExt2_eloss(*part) = eloss;
		}


		else if (sample==CaloCell_ID::HEC0) {
			m_dec_HEC0_dep(*part) = dep;
			m_dec_HEC0_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::HEC1) {
			m_dec_HEC1_dep(*part) = dep;
			m_dec_HEC1_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::HEC2) {
			m_dec_HEC2_dep(*part) = dep;
			m_dec_HEC2_eloss(*part) = eloss;
		}
		else if (sample==CaloCell_ID::HEC3) {
			m_dec_HEC3_dep(*part) = dep;
			m_dec_HEC3_eloss(*part) = eloss;
		}
	}
	return StatusCode::SUCCESS;
}
