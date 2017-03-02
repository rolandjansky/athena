// System includes
#include <limits>

// ROOT includes
#include "TError.h"

// Framework includes
#include "CxxUtils/make_unique.h"

// Local includes
#include "AssociationUtils/OverlapRemovalInit.h"
#include "AssociationUtils/DeltaROverlapTool.h"
#include "AssociationUtils/EleMuSharedTrkOverlapTool.h"
#include "AssociationUtils/EleJetOverlapTool.h"
#include "AssociationUtils/MuJetOverlapTool.h"
#include "AssociationUtils/TauLooseEleOverlapTool.h"
#include "AssociationUtils/TauLooseMuOverlapTool.h"
#include "AssociationUtils/OverlapRemovalTool.h"

// Error checking macro
#define ORT_CHECK( ARG )                        \
  do {                                          \
    const bool result = ARG;                    \
    if(!result) {                               \
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

#ifdef ROOTCORE

  //---------------------------------------------------------------------------
  // Preconfigured harmonization tools
  //---------------------------------------------------------------------------
  StatusCode recommendedTools(ORToolBox& tbox,
                              const std::string& name,
                              const std::string& inputLabel,
                              const std::string& outputLabel,
                              const std::string& bJetLabel,
                              bool boostedLeptons,
                              bool outputPassValue,
                              bool doTaus, bool doPhotons)
  {
    using CxxUtils::make_unique;

    // The master tool
    tbox.setMasterTool( make_unique<OverlapRemovalTool>(name) );

    // El-mu overlap
    auto eleMuORT = make_unique<EleMuSharedTrkOverlapTool>(name + "_EleMuORT");
    ORT_CHECK( tbox.addOverlapTool(std::move(eleMuORT), "EleMuORT") );
    // El-jet overlap tool
    auto eleJetORT = make_unique<EleJetOverlapTool>(name + "_EleJetORT");
    ORT_CHECK( eleJetORT->setProperty("BJetLabel", bJetLabel) );
    ORT_CHECK( eleJetORT->setProperty("UseSlidingDR", boostedLeptons) );
    ORT_CHECK( tbox.addOverlapTool(std::move(eleJetORT), "EleJetORT") );
    // Mu-jet overlap tool
    auto muJetORT = make_unique<MuJetOverlapTool>(name + "_MuJetORT");
    ORT_CHECK( muJetORT->setProperty("BJetLabel", bJetLabel) );
    ORT_CHECK( muJetORT->setProperty("UseSlidingDR", boostedLeptons) );
    ORT_CHECK( tbox.addOverlapTool(std::move(muJetORT), "MuJetORT") );

    if(doTaus) {

      // Tau-ele overlap tool
      auto tauEleORT = make_unique<DeltaROverlapTool>(name + "_TauEleORT");
      ORT_CHECK( tauEleORT->setProperty("DR", 0.2) );
      ORT_CHECK( tbox.addOverlapTool(std::move(tauEleORT), "TauEleORT") );

      // Tau-mu overlap tool
      auto tauMuORT = make_unique<DeltaROverlapTool>(name + "_TauMuORT");
      ORT_CHECK( tauMuORT->setProperty("DR", 0.2) );
      ORT_CHECK( tbox.addOverlapTool(std::move(tauMuORT), "TauMuORT") );

      // Tau-jet overlap tool
      auto tauJetORT = make_unique<DeltaROverlapTool>(name + "_TauJetORT");
      ORT_CHECK( tauJetORT->setProperty("DR", 0.2) );
      ORT_CHECK( tbox.addOverlapTool(std::move(tauJetORT), "TauJetORT") );

    }

    if(doPhotons) {
      // Pho-ele overlap tool
      auto phoEleORT = make_unique<DeltaROverlapTool>(name + "_PhoEleORT");
      ORT_CHECK( tbox.addOverlapTool(std::move(phoEleORT), "PhoEleORT") );
      // Pho-mu overlap tool
      auto phoMuORT = make_unique<DeltaROverlapTool>(name + "_PhoMuORT");
      ORT_CHECK( tbox.addOverlapTool(std::move(phoMuORT), "PhoMuORT") );
      // Pho-jet overlap tool
      auto phoJetORT = make_unique<DeltaROverlapTool>(name + "_PhoJetORT");
      ORT_CHECK( tbox.addOverlapTool(std::move(phoJetORT), "PhoJetORT") );
    }

    // For now, input and output labels are set on all tools equally
    for(auto& keyToolPair : tbox) {
      ORT_CHECK( keyToolPair.second->setProperty("InputLabel", inputLabel) );
      ORT_CHECK( keyToolPair.second->setProperty("OutputLabel", outputLabel) );
      ORT_CHECK( keyToolPair.second->setProperty("OutputPassValue", outputPassValue) );
    }

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  // Preconfigured harmonization tools
  //---------------------------------------------------------------------------
  StatusCode harmonizedTools(ORToolBox& tbox,
                             const std::string& name,
                             const std::string& inputLabel,
                             const std::string& outputLabel,
                             bool outputPassValue,
                             bool doTaus, bool doPhotons)
  {
    // Get the standard pre-configured tools
    const std::string bJetLabel = "";
    const bool boostedLeptons = false;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    ORT_CHECK( recommendedTools(tbox, name, inputLabel, outputLabel, bJetLabel,
                                boostedLeptons, outputPassValue, false, doPhotons) );
#pragma GCC diagnostic pop

    // Override some properties to disable features
    auto eleMuORT = tbox.getTool("EleMuORT");
    ORT_CHECK( eleMuORT != nullptr );
    ORT_CHECK( eleMuORT->setProperty("RemoveCaloMuons", false) );

    auto eleJetORT = tbox.getTool("EleJetORT");
    ORT_CHECK( eleJetORT != nullptr );

    auto muJetORT = tbox.getTool("MuJetORT");
    ORT_CHECK( muJetORT != nullptr );
    ORT_CHECK( muJetORT->setProperty("ApplyRelPt", false) );
    ORT_CHECK( muJetORT->setProperty("UseGhostAssociation", false) );
    ORT_CHECK( muJetORT->setProperty("InnerDR", 0.4) );

    // Fill the tau loose-lepton OR recommendations
    if(doTaus) {
      using CxxUtils::make_unique;
      // Tau-ele overlap tool
      auto tauEleORT = make_unique<TauLooseEleOverlapTool>(name + "_TauEleORT");
      ORT_CHECK( tauEleORT->setProperty("InputLabel", inputLabel) );
      ORT_CHECK( tauEleORT->setProperty("OutputLabel", outputLabel) );
      ORT_CHECK( tauEleORT->setProperty("OutputPassValue", outputPassValue) );
      ORT_CHECK( tbox.addOverlapTool(std::move(tauEleORT), "TauEleORT") );
      // Tau-mu overlap tool
      auto tauMuORT = make_unique<TauLooseMuOverlapTool>(name + "_TauMuORT");
      ORT_CHECK( tauMuORT->setProperty("InputLabel", inputLabel) );
      ORT_CHECK( tauMuORT->setProperty("OutputLabel", outputLabel) );
      ORT_CHECK( tauMuORT->setProperty("OutputPassValue", outputPassValue) );
      ORT_CHECK( tbox.addOverlapTool(std::move(tauMuORT), "TauMuORT") );
      // Tau-jet overlap tool
      auto tauJetORT = make_unique<DeltaROverlapTool>(name + "_TauJetORT");
      ORT_CHECK( tauJetORT->setProperty("InputLabel", inputLabel) );
      ORT_CHECK( tauJetORT->setProperty("OutputLabel", outputLabel) );
      ORT_CHECK( tauJetORT->setProperty("OutputPassValue", outputPassValue) );
      ORT_CHECK( tauJetORT->setProperty("DR", 0.2) );
      ORT_CHECK( tbox.addOverlapTool(std::move(tauJetORT), "TauJetORT") );
    }

    return StatusCode::SUCCESS;
  }

#endif // ROOTCORE

}

#undef ORT_CHECK
