/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This source file implements all of the functions related to <OBJECT>
// in the SUSYObjDef_xAOD class

// Local include(s):
#include "SUSYTools/SUSYObjDef_xAOD.h"

#include "METInterface/IMETMaker.h"
#include "METInterface/IMETSystematicsTool.h"
#include "xAODMissingET/MissingETAuxContainer.h"
#include "METUtilities/METHelpers.h"

#ifndef XAOD_STANDALONE // For now metadata is Athena-only
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

namespace ST {

  const static SG::AuxElement::ConstAccessor<char> cacc_baseline("baseline");

StatusCode SUSYObjDef_xAOD::GetMET(xAOD::MissingETContainer &met,
                                   const xAOD::JetContainer* jet,
                                   const xAOD::ElectronContainer* elec,
                                   const xAOD::MuonContainer* muon,
                                   const xAOD::PhotonContainer* gamma,
                                   const xAOD::TauJetContainer* taujet,
                                   bool doTST, bool doJVTCut,
                                   const xAOD::IParticleContainer* invis) {
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return StatusCode::FAILURE;
  }

  const xAOD::MissingETContainer* metcore(0);
  if ( evtStore()->retrieve( metcore, m_inputMETCore ).isFailure() ) {
    ATH_MSG_WARNING( "Unable to retrieve MET core container: " << m_inputMETCore );
    return StatusCode::FAILURE;
  }
  const xAOD::MissingETAssociationMap* metMap(0);
  if ( evtStore()->retrieve(metMap, m_inputMETMap).isFailure() ) {
    ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_inputMETMap);
    return StatusCode::FAILURE;
  }

  std::string softTerm = "SoftClus";
  if (doTST) {
    softTerm = "PVSoftTrk";
  } else if (doJVTCut) {
    ATH_MSG_WARNING( "Requested CST MET and a JVT cut.  This is not a recommended configuration - please consider switching to TST." );
  }

  metMap->resetObjSelectionFlags();

  // allow creation of proxy MET by flagging objects for "neutrino/ification" as already selected
  if (invis) {
    ATH_CHECK( m_metMaker->markInvisible(invis, metMap, &met) );
  }

  if (elec) {
    ATH_MSG_VERBOSE("Build electron MET");
    ConstDataVector<xAOD::ElectronContainer> metelectron(SG::VIEW_ELEMENTS);
    for (const auto& el : *elec) {
      if (cacc_baseline(*el)) {
        bool veto(false);
        if (invis) {
          for (const auto& ipart : *invis) {
            if (ipart == el) {veto = true; break;}
          }
        }
        if (!veto) metelectron.push_back(el);
      }
    }
    ATH_CHECK( m_metMaker->rebuildMET(m_eleTerm, xAOD::Type::Electron, &met, metelectron.asDataVector(), metMap) );
  }

  if (gamma) {
    ATH_MSG_VERBOSE("Build photon MET");
    ConstDataVector<xAOD::PhotonContainer> metgamma(SG::VIEW_ELEMENTS);
    for (const auto& ph : *gamma) {
      if (cacc_baseline(*ph)) {
        bool veto(false);
        if (invis) {
          for (const auto& ipart : *invis) {
            if (ipart == ph) {veto = true; break;}
          }
        }
        if (!veto) metgamma.push_back(ph);
      }
    }
    ATH_CHECK( m_metMaker->rebuildMET(m_gammaTerm, xAOD::Type::Photon, &met, metgamma.asDataVector(), metMap) );
  }

  if (taujet) {
    ATH_MSG_VERBOSE("Build tau MET");
    ConstDataVector<xAOD::TauJetContainer> mettau(SG::VIEW_ELEMENTS);
    for (const auto& tau : *taujet) {
      if (cacc_baseline(*tau)) {
        bool veto(false);
        if (invis) {
          for (const auto& ipart : *invis) {
            if (ipart == tau) {veto = true; break;}
          }
        }
        if (!veto) mettau.push_back(tau);
      }
    }
    ATH_CHECK( m_metMaker->rebuildMET(m_tauTerm, xAOD::Type::Tau, &met, mettau.asDataVector(), metMap) );
  }

  if (muon) {
    ATH_MSG_VERBOSE("Build muon MET");
    ConstDataVector<xAOD::MuonContainer> metmuon(SG::VIEW_ELEMENTS);
    for (const auto& mu : *muon) {
      bool veto(false);
      if (cacc_baseline(*mu)) {
        if (invis) {
          for (const auto& ipart : *invis) {
            if (ipart == mu) {veto = true; break;}
          }
        }
        if (!veto) metmuon.push_back(mu);
      }
    }
    ATH_CHECK( m_metMaker->rebuildMET(m_muonTerm, xAOD::Type::Muon, &met, metmuon.asDataVector(), metMap) );
  }

  if (!jet) {
    ATH_MSG_WARNING("Invalid jet container specified for MET rebuilding!");
    return StatusCode::SUCCESS;
  }

  ATH_MSG_VERBOSE("Build jet/soft MET");
  ATH_CHECK( m_metMaker->rebuildJetMET(m_jetTerm, softTerm, &met, jet, metcore, metMap, doJVTCut) );

  if (!isData()) {
    ATH_MSG_VERBOSE("Original soft term " << met[softTerm]->name() << " (met: " << met[softTerm]->met() << ")" );
    if ( m_metSystTool->applyCorrection(*met[softTerm]) != CP::CorrectionCode::Ok ) {
      ATH_MSG_WARNING("GetMET: Failed to apply MET soft term systematics.");
    }
    ATH_MSG_VERBOSE("New soft term value: " << met[softTerm]->met() );
  }

  ATH_MSG_VERBOSE("Build MET sum");
  ATH_CHECK( m_metMaker->buildMETSum(m_outMETTerm, &met, met[softTerm]->source()) );
  ATH_MSG_VERBOSE( "Done rebuilding MET." );

  ATH_MSG_VERBOSE( "Rebuilt MET: Missing Et (x,y): (" << met[m_outMETTerm]->mpx() << "," <<  met[m_outMETTerm]->mpy() << ")");

  return StatusCode::SUCCESS;

}


StatusCode SUSYObjDef_xAOD::GetTrackMET(xAOD::MissingETContainer &met,
                                        const xAOD::JetContainer* jet,
                                        const xAOD::ElectronContainer* elec,
                                        const xAOD::MuonContainer* muon) {
  if (!m_tool_init) {
    ATH_MSG_ERROR("SUSYTools was not initialized!!");
    return StatusCode::FAILURE;
  }

  const xAOD::MissingETContainer* metcore(0);
  if ( evtStore()->retrieve( metcore, m_inputMETCore ).isFailure() ) {
    ATH_MSG_WARNING( "Unable to retrieve MET core container: " << m_inputMETCore );
    return StatusCode::FAILURE;
  }
  const xAOD::MissingETAssociationMap* metMap(0);
  if ( evtStore()->retrieve(metMap, m_inputMETMap).isFailure() ) {
    ATH_MSG_WARNING("Unable to retrieve MissingETAssociationMap: " << m_inputMETMap);
    return StatusCode::FAILURE;
  }

  metMap->resetObjSelectionFlags();

  if (elec) {
    ATH_MSG_VERBOSE("Build electron MET");
    ConstDataVector<xAOD::ElectronContainer> metelectron(SG::VIEW_ELEMENTS);
    for (const auto& el : *elec) {
      if (cacc_baseline(*el)) metelectron.push_back(el);
    }
    ATH_CHECK( m_metMaker->rebuildMET(m_eleTerm, xAOD::Type::Electron, &met, metelectron.asDataVector(), metMap) );
  }

  if (muon) {
    ATH_MSG_VERBOSE("Build muon MET");
    ConstDataVector<xAOD::MuonContainer> metmuon(SG::VIEW_ELEMENTS);
    for (const auto& mu : *muon) {
      if (cacc_baseline(*mu)) metmuon.push_back(mu);
    }
    ATH_CHECK( m_metMaker->rebuildMET(m_muonTerm, xAOD::Type::Muon, &met, metmuon.asDataVector(), metMap) );
  }

  if (!jet) {
    ATH_MSG_WARNING("Invalid jet container specified for MET rebuilding!");
    return StatusCode::SUCCESS;
  }
  ATH_MSG_VERBOSE("Build jet/soft MET_Track");

  std::string softTerm = "PVSoftTrk";
  ATH_CHECK( m_metMaker->rebuildTrackMET(m_jetTerm, softTerm, &met, jet, metcore, metMap, true) );

  if (!isData()) {
    ATH_MSG_VERBOSE("Apply MET systematics");
    if ( m_metSystTool->applyCorrection(*met[softTerm],
                                        metMap) != CP::CorrectionCode::Ok ) {
      ATH_MSG_WARNING("GetMET: Failed to apply MET track systematics.");
    }
  }

  ATH_MSG_VERBOSE("Build MET sum");
  ATH_CHECK( m_metMaker->buildMETSum("Track", &met, met[softTerm]->source()) );
  ATH_MSG_VERBOSE( "Done rebuilding MET." );

  ATH_MSG_VERBOSE( "Track MET: Missing Et (x,y): (" << met["Track"]->mpx() << "," << met["Track"]->mpy() << ")");

  return StatusCode::SUCCESS;

}


}
