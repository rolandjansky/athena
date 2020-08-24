/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System includes
#include <limits>

// ROOT includes
#include "TError.h"

// Local includes
#include "AssociationUtils/OverlapRemovalInit.h"
#include "AssociationUtils/DeltaROverlapTool.h"
#include "AssociationUtils/EleMuSharedTrkOverlapTool.h"
#include "AssociationUtils/EleJetOverlapTool.h"
#include "AssociationUtils/MuJetOverlapTool.h"
#include "AssociationUtils/MuPFJetOverlapTool.h"
#include "AssociationUtils/TauLooseEleOverlapTool.h"
#include "AssociationUtils/TauLooseMuOverlapTool.h"
#include "AssociationUtils/OverlapRemovalTool.h"

// Error checking macro
#define ORT_CHECK( ARG )                        \
  do {                                          \
    if(!static_cast<bool>(ARG)) {               \
      ::Error("OverlapRemovalInit",             \
              "Failed to execute: \"%s\"",      \
              #ARG );                           \
      return StatusCode::FAILURE;               \
    }                                           \
  } while( false )


namespace ORUtils
{

  //---------------------------------------------------------------------------
  // ORFlags constructor
  //---------------------------------------------------------------------------
  ORFlags::ORFlags(const std::string& masterToolName,
                   const std::string& theInputLabel,
                   const std::string& theOutputLabel)
    : masterName(masterToolName),
      inputLabel(theInputLabel),
      outputLabel(theOutputLabel)
  {}

  //---------------------------------------------------------------------------
  // Preconfigured recommended tools
  //---------------------------------------------------------------------------
  StatusCode recommendedTools(const ORFlags& flags, ToolBox& tbox)
  {
    // Master tool
    tbox.masterTool.setTypeAndName("ORUtils::OverlapRemovalTool/" +
                                   flags.masterName);

    // Muon-PFlow fake-jet
    if(flags.doMuPFJetOR && tbox.muPFJetORT.empty()) {
      tbox.muPFJetORT.setTypeAndName("ORUtils::MuPFJetOverlapTool/" +
                                   flags.masterName + ".MuPFJetORT");
      ORT_CHECK( tbox.muPFJetORT.setProperty("BJetLabel", flags.bJetLabel) );
    }

    // El-el
    if(flags.doElectrons && flags.doEleEleOR && tbox.eleEleORT.empty()) {
      tbox.eleEleORT.setTypeAndName("ORUtils::EleEleOverlapTool/" +
                                    flags.masterName + ".EleEleORT");
    }

    // El-mu
    if(flags.doElectrons && flags.doMuons && tbox.eleMuORT.empty()) {
      tbox.eleMuORT.setTypeAndName("ORUtils::EleMuSharedTrkOverlapTool/" +
                                   flags.masterName + ".EleMuORT");
    }
    // El-jet
    if(flags.doElectrons && flags.doJets && tbox.eleJetORT.empty()) {
      tbox.eleJetORT.setTypeAndName("ORUtils::EleJetOverlapTool/" +
                                    flags.masterName + ".EleJetORT");
      ORT_CHECK( tbox.eleJetORT.setProperty("BJetLabel", flags.bJetLabel) );
      ORT_CHECK( tbox.eleJetORT.setProperty("MaxElePtForBJetAwareOR", flags.maxElePtForBJetAwareOR) );
      ORT_CHECK( tbox.eleJetORT.setProperty("UseSlidingDR", flags.boostedLeptons) );
    }
    // Mu-jet
    if(flags.doMuons && flags.doJets && tbox.muJetORT.empty()) {
      tbox.muJetORT.setTypeAndName("ORUtils::MuJetOverlapTool/" +
                                   flags.masterName + ".MuJetORT");
      ORT_CHECK( tbox.muJetORT.setProperty("BJetLabel", flags.bJetLabel) );
      ORT_CHECK( tbox.muJetORT.setProperty("UseSlidingDR", flags.boostedLeptons) );
    }

    // Tau-ele overlap tool
    if(flags.doTaus && flags.doElectrons && tbox.tauEleORT.empty()) {
      tbox.tauEleORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                    flags.masterName + ".TauEleORT");
      ORT_CHECK( tbox.tauEleORT.setProperty("DR", 0.2) );
    }
    // Tau-mu overlap tool
    if(flags.doTaus && flags.doMuons && tbox.tauMuORT.empty()) {
      tbox.tauMuORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                   flags.masterName + ".TauMuORT");
      ORT_CHECK( tbox.tauMuORT.setProperty("DR", 0.2) );
    }
    // Tau-jet overlap tool
    if(flags.doTaus && flags.doJets && tbox.tauJetORT.empty()) {
      tbox.tauJetORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                    flags.masterName + ".TauJetORT");
      ORT_CHECK( tbox.tauJetORT.setProperty("DR", 0.2) );
    }

    // Pho-ele overlap tool
    if(flags.doPhotons && flags.doElectrons && tbox.phoEleORT.empty()) {
      tbox.phoEleORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                    flags.masterName + ".PhoEleORT");
    }
    // Pho-mu overlap tool
    if(flags.doPhotons && flags.doMuons && tbox.phoMuORT.empty()) {
      tbox.phoMuORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                   flags.masterName + ".PhoMuORT");
    }
    // Pho-jet overlap tool
    if(flags.doPhotons && flags.doJets && tbox.phoJetORT.empty()) {
      tbox.phoJetORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                    flags.masterName + ".PhoJetORT");
    }

    // Ele-fatjet overlap tool
    if(flags.doElectrons && flags.doFatJets && tbox.eleFatJetORT.empty()) {
      tbox.eleFatJetORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                       flags.masterName + ".EleFatJetORT");
      ORT_CHECK( tbox.eleFatJetORT.setProperty("DR", 1.0) );
    }

    // Jet-fatjet overlap tool
    if(flags.doJets && flags.doFatJets && tbox.jetFatJetORT.empty()) {
      tbox.jetFatJetORT.setTypeAndName("ORUtils::DeltaROverlapTool/" +
                                       flags.masterName + ".JetFatJetORT");
      ORT_CHECK( tbox.jetFatJetORT.setProperty("DR", 1.0) );
    }

    // Overlap tool common properties
    for(auto hptr : tbox.getOverlapTools()) {
      ORT_CHECK( hptr->setProperty("LinkOverlapObjects", flags.linkOverlapObjects) );
    }

    // Global common properties
    ORT_CHECK( tbox.setGlobalProperty("InputLabel", flags.inputLabel) );
    ORT_CHECK( tbox.setGlobalProperty("OutputLabel", flags.outputLabel) );
    ORT_CHECK( tbox.setGlobalProperty("OutputPassValue", flags.outputPassValue) );

    return StatusCode::SUCCESS;
  }

}

#undef ORT_CHECK
