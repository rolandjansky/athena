///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xExampleEDM.h:
// This file has EDM includes, EDM collections and CP tools for xExample.
// Keep all this in a separate header file to avoid CINT conflicts:
// https://twiki.cern.ch/twiki/bin/view/AtlasComputing/SoftwareTutorialxAODAnalysisInROOT
// Author: Frank Paige <paige@bnl.gov>
///////////////////////////////////////////////////////////////////

#ifndef XEXAMPLE_XEXAMPLEEDM_H
#define XEXAMPLE_XEXAMPLEEDM_H

// Rationale: temporary suppression of
// unused-local-typedef warning in Boost header
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#include "boost/tuple/tuple.hpp"
#pragma GCC diagnostic pop

// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetTypes.h"
#include "xAODBTagging/BTagging.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

#include "xAODCore/ShallowCopy.h"

// CP tool includes
#ifdef ROOTCORE

#include "JetCalibTools/JetCalibrationTool.h"
#include "JetSelectorTools/JetCleaningTool.h"
#include "METUtilities/METRebuilder.h"

#else
//in athena, we include the interfaces, NOT the concrete classes
#include "JetCalibTools/IJetCalibrationTool.h" //interface for JetCalibrationTool
#include "JetInterface/IJetSelector.h"  // JetCleaningTool
#include "METInterface/IMETRebuilder.h" //METRebuilder

#endif

#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#endif // ROOTCORE


// EDM containers
const xAOD::MuonContainer* m_muons;
const xAOD::ElectronContainer* m_electrons;
const xAOD::TauJetContainer* m_tauJets;
const xAOD::JetContainer* m_akt4Jets;
const xAOD::JetContainer* m_akt4TruthJets;
const xAOD::JetContainer* m_aktem4Jets;
const xAOD::MissingETContainer* m_metFinals;
const xAOD::MissingETContainer* m_metTruths;

// xAOD supports deep (complete) and shallow copies.
// Choose deep copies for selected e/mu/tau. Current selection just uses
// "tight" flags; eventually might apply CP tools.
// Create container and aux container in initialize() and link them.
// Clear container at end of execute().

xAOD::MuonContainer* m_newMuons;
xAOD::MuonAuxContainer* m_newMuonsAux;

xAOD::ElectronContainer* m_newElectrons;
xAOD::ElectronAuxContainer* m_newElectronsAux;

xAOD::TauJetContainer* m_newTauJets;
xAOD::TauJetAuxContainer* m_newTauJetsAux;

// Shallow-copy containers for AntiKt4LCTopo selected, recalibrated jets.
// Jet container made in initialize() as view container. Holds selected
// jets but does not own them.
// Pair holds two containers made by shallowCopyContainer as shallow copies
// of all original jets. Must be deleted for each event.
xAOD::JetContainer* m_akt4ViewJets;
std::pair< xAOD::JetContainer*, xAOD::ShallowAuxContainer* > m_akt4CopyJets;

// Deep-copy containers for AntiKt4LCTopo jets.
// These are initialized and connected in initialize()
xAOD::JetContainer* m_aktem4NewJets;
xAOD::JetAuxContainer* m_aktem4NewJetsAux;

// Bad jet flag
float m_badJetPtMax;

// Containers for rebuilt MET
// These get initialized by METRebuilder
const xAOD::MissingETContainer* m_metCalibs;
const xAOD::MissingETAuxContainer* m_metCalibsAux;
// Needed for METRebuilder constituent map
SG::AuxElement::Accessor< ElementLink<xAOD::IParticleContainer> > m_objLinkAcc("originalObjectLink");

#ifdef ROOTCORE

// Transient object store. Needed for the CP tools.

xAOD::TStore* m_store;

// Pointers to CP tools set up in initialize()
JetCleaningTool* m_jetCleaningTool;
JetCalibrationTool* m_akt4LCTopoCalibrationTool;
JetCalibrationTool* m_akt4EMTopoCalibrationTool;
met::METRebuilder* m_metRebuilder;

#else
#include "GaudiKernel/ToolHandle.h"

//ToolHandles to CP tools, set up in job options
ToolHandle<IJetSelector> m_jetCleaningTool;
ToolHandle<IJetCalibrationTool> m_akt4LCTopoCalibrationTool;
ToolHandle<IJetCalibrationTool> m_akt4EMTopoCalibrationTool;
ToolHandle<IMETRebuilder> m_metRebuilder;

#endif

#endif //XEXAMPLE_XEXAMPLEEDM_H
