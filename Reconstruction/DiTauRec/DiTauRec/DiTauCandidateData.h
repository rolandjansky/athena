/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUREC_CANDIDATE_DATA_H
#define DITAUREC_CANDIDATE_DATA_H

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODTau/DiTauJet.h"
#include "xAODTau/DiTauJetContainer.h"
#include "xAODTau/DiTauJetAuxContainer.h"
#include "fastjet/PseudoJet.hh"

#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"





class DiTauCandidateData {
public:
	xAOD::DiTauJet* xAODDiTau;
	xAOD::DiTauJetContainer* xAODDiTauContainer;
	xAOD::DiTauJetAuxContainer* diTauAuxContainer;

	const xAOD::Jet* seed;
	const xAOD::JetContainer* seedContainer;
	std::vector<fastjet::PseudoJet> subjets;
	std::vector<const CaloCell*> subjetCells;

	std::vector<const xAOD::Electron*> electrons;
	std::vector<const xAOD::Muon*> muons;

	float Rjet;
	float Rsubjet;
	float Rcore;

};

#endif // DITAU_CANDIDATE_DATA