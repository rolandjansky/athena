/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopCPTools/TopOverlapRemovalCPTools.h"

#include <map>
#include <string>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// Overlap removal includes:
#include "AssociationUtils/OverlapRemovalInit.h"
#include "AssociationUtils/OverlapRemovalTool.h"

namespace top {
  OverlapRemovalCPTools::OverlapRemovalCPTools(const std::string& name) :
    asg::AsgTool(name) {
    declareProperty("config", m_config);
  }

  StatusCode OverlapRemovalCPTools::initialize() {
    ATH_MSG_INFO("top::OverlapRemovalCPTools initialize...");

    if (m_config->isTruthDxAOD()) {
      ATH_MSG_INFO("top::OverlapRemovalCPTools: no need to initialise anything on truth DxAOD");
      return StatusCode::SUCCESS;
    }

    if (!m_config->makeAllCPTools()) {
      ATH_MSG_INFO("top::OverlapRemovalCPTools: no need to initialise for mini-xAOD");
      return StatusCode::SUCCESS;
    }

    top::check(setupOverlapRemoval(), "Failed to setup Overlap Removal");
    return StatusCode::SUCCESS;
  }

  StatusCode OverlapRemovalCPTools::setupOverlapRemoval() {
    // Allowed overlap removal options
    std::set<std::string> allowed_OR_procedures = {
      "recommended", "jetmuApplyRelPt", "harmonized",
      "Boosted", "BoostedSlidingDREl", "BoostedSlidingDRMu", "BoostedSlidingDRElMu", "noTauJetOLR"
    };
    std::string OR_procedure = m_config->overlapRemovalProcedure();

    float overlapRemovalSlidingInnerDRel = m_config->overlapRemovalSlidingInnerDRel();
    float overlapRemovalSlidingInnerDRmu = m_config->overlapRemovalSlidingInnerDRmu();

    // if this is set, turns OFF ele-jet OR completely (it's taken care of by the electronInJetSubtraction tool)
    bool applyElectronInJetSubtraction = m_config->applyElectronInJetSubtraction();

    // If the requested OR procedure is unknown then fail.
    // This is mostly to avoid me writing 'harmonised' by mistake ;)
    if (allowed_OR_procedures.find(OR_procedure)
        == allowed_OR_procedures.end()) {
      ATH_MSG_ERROR(OR_procedure << " is not a valid option.\n"
                                    "Valid options are currently: "
                                    "recommended [default], jetmuApplyRelPt, harmonized, "
                                    "Boosted, BoostedSlidingDREl, BoostedSlidingDRMu, BoostedSlidingDRElMu, noTauJetOLR");
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Overlap removal procedure = " << OR_procedure);

//     ORUtils::ORFlags OR_flags("OverlapRemovalTool",
//                               "passPreORSelection");
    ORUtils::ORFlags OR_flags("OverlapRemovalTool",
                              "ORToolDecoration");

    OR_flags.doElectrons = m_config->useElectrons();
    OR_flags.doMuons = m_config->useMuons();
    OR_flags.doJets = m_config->useJets();
    OR_flags.doTaus = m_config->useTaus();
    OR_flags.doPhotons = m_config->usePhotons();
    if (OR_procedure == "Boosted" ||
        OR_procedure == "BoostedSlidingDREl" ||
        OR_procedure == "BoostedSlidingDRMu" ||
        OR_procedure == "BoostedSlidingDRElMu") {
      OR_flags.boostedLeptons = true;
    }
    OR_flags.doFatJets = (m_config->useLargeRJets() && m_config->doLargeJetOverlapRemoval());
    OR_flags.doMuPFJetOR = (m_config->useParticleFlowJets());

    const float floatMax = std::numeric_limits<float>::max();

    top::check(ORUtils::recommendedTools(OR_flags, m_ORtoolBox),
               "Failed to setup OR Tool box");
    for (auto&& tool : m_ORtoolBox.getOverlapTools()) {
      top::check(tool->setProperty("EnableUserPriority", true),
                 "Failed to set EnableUserPriority");
    }
    if (OR_procedure == "harmonized") {
      if (m_config->useMuons() &&
          m_config->useElectrons()) top::check(m_ORtoolBox.eleMuORT.setProperty("RemoveCaloMuons", false),
                                               "Failed to set RemoveCaloMuons in eleMuORT");
      if (m_config->useMuons() && m_config->useJets()) {
        top::check(m_ORtoolBox.muJetORT.setProperty("MuJetPtRatio", floatMax),
                   "Failed to set MuJetPtRatio in muJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("MuJetTrkPtRatio", floatMax),
                   "Failed to set MuJetTrkPtRatio in muJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("UseGhostAssociation", false),
                   "Failed to set UseGhostAssociation in muJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("InnerDR", 0.4),
                   "Failed to set InnerDR in muJetORT");
      }
    } else if (OR_procedure == "jetmuApplyRelPt") {
      top::check(m_ORtoolBox.muJetORT.setProperty("ApplyRelPt", true),
                 "Failed to set ApplyRelPt in muJetORT");
    } else if (OR_procedure == "BoostedSlidingDREl" ||
               OR_procedure == "BoostedSlidingDRMu" ||
               OR_procedure == "BoostedSlidingDRElMu") {
      if (m_config->useElectrons() && m_config->useJets() &&
          OR_procedure == "BoostedSlidingDREl") {
        top::check(m_ORtoolBox.eleJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in ElJetORT");
        top::check(m_ORtoolBox.eleJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRel),
                   "Failed to setting inner radius equal to SlidingInnerDRel in EleJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("UseSlidingDR", false),
                   "Failed to setSliding DR in MuJetORT");
      }
      if (m_config->useMuons() && m_config->useJets() &&
          OR_procedure == "BoostedSlidingDRMu") {
        top::check(m_ORtoolBox.eleJetORT.setProperty("UseSlidingDR", false),
                   "Failed to setSliding DR in ElJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in MuJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRmu),
                   "Failed to setting inner radius equal to SlidingInnerDRmu in MuJetORT");
      }
      if (m_config->useElectrons() && m_config->useMuons() && m_config->useJets() &&
          OR_procedure == "BoostedSlidingDRElMu") {
        top::check(m_ORtoolBox.eleJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in ElJetORT");
        top::check(m_ORtoolBox.eleJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRel),
                   "Failed to setting inner radius equal to SlidingInnerDRel in EleJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in MuJetORT");
        top::check(m_ORtoolBox.muJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRmu),
                   "Failed to setting inner radius equal to SlidingInnerDRmu in MuJetORT");
      }
    } else if (OR_procedure == "noTauJetOLR") {
      top::check(m_ORtoolBox.tauJetORT.setProperty("DR", 0.0),
                 "Failed to set DR in TauJetORT to zero");
    }
    if (applyElectronInJetSubtraction) {
      top::check(m_ORtoolBox.eleJetORT.setProperty("UseSlidingDR", true),
                 "Failed to setSliding DR in ElJetORT");
      top::check(m_ORtoolBox.eleJetORT.setProperty("InnerDR", 0),
                 "Failed to setting inner radius equal to SlidingInnerDRel in EleJetORT");
      top::check(m_ORtoolBox.eleJetORT.setProperty("SlidingDRC1", 0),
                 "Failed to set SlidingDRC1");
      top::check(m_ORtoolBox.eleJetORT.setProperty("SlidingDRC2", 0),
                 "Failed to set SlidingDRC2");
      top::check(m_ORtoolBox.eleJetORT.setProperty("SlidingDRMaxCone", 0),
                 "Failed to set SlidingDRMaxCone");
    }
    top::check(m_ORtoolBox.initialize(),
               "Failed to initialize overlap removal tools");
    m_overlapRemovalTool = std::move(m_ORtoolBox.masterTool);

    OR_flags.masterName = "OverlapRemovalToolLoose";
//     OR_flags.inputLabel = "passPreORSelectionLoose";
    OR_flags.inputLabel = "ORToolDecorationLoose";

    top::check(ORUtils::recommendedTools(OR_flags, m_ORtoolBox_Loose),
               "Failed to setup OR Tool box");
    for (auto&& tool : m_ORtoolBox_Loose.getOverlapTools()) {
      top::check(tool->setProperty("EnableUserPriority", true),
                 "Failed to set EnableUserPriority");
    }
    if (OR_procedure == "harmonized") {
      if (m_config->useMuons() && m_config->useElectrons())
        top::check(m_ORtoolBox_Loose.eleMuORT.setProperty("RemoveCaloMuons",
                                                          false),
                   "Failed to set RemoveCaloMuons in eleMuORT");
      if (m_config->useMuons() && m_config->useJets()) {
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("MuJetPtRatio", floatMax),
                   "Failed to set MuJetPtRatio in muJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("MuJetTrkPtRatio", floatMax),
                   "Failed to set MuJetTrkPtRatio in muJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("UseGhostAssociation", false),
                   "Failed to set UseGhostAssociation in muJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("InnerDR", 0.4),
                   "Failed to set InnerDR in muJetORT");
      }
    } else if (OR_procedure == "jetmuApplyRelPt") {
      top::check(m_ORtoolBox_Loose.muJetORT.setProperty("ApplyRelPt", false),
                 "Failed to set ApplyRelPt in muJetORT");
    } else if (OR_procedure == "BoostedSlidingDREl" ||
               OR_procedure == "BoostedSlidingDRMu" ||
               OR_procedure == "BoostedSlidingDRElMu") {
      if (m_config->useElectrons() && m_config->useJets() &&
          OR_procedure == "BoostedSlidingDREl") {
        top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in ElJetORT");
        top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRel),
                   "Failed to setting inner radius equal to SlidingInnerDRel in EleJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("UseSlidingDR", false),
                   "Failed to setSliding DR in MuJetORT");
      }
      if (m_config->useMuons() && m_config->useJets() &&
          OR_procedure == "BoostedSlidingDRMu") {
        top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("UseSlidingDR", false),
                   "Failed to setSliding DR in ElJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in MuJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRmu),
                   "Failed to setting inner radius equal to SlidingInnerDRmu in MuJetORT");
      }
      if (m_config->useElectrons() && m_config->useMuons() && m_config->useJets() &&
          OR_procedure == "BoostedSlidingDRElMu") {
        top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in ElJetORT");
        top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRel),
                   "Failed to setting inner radius equal to SlidingInnerDRel in EleJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("UseSlidingDR", true),
                   "Failed to setSliding DR in MuJetORT");
        top::check(m_ORtoolBox_Loose.muJetORT.setProperty("InnerDR", overlapRemovalSlidingInnerDRmu),
                   "Failed to setting inner radius equal to SlidingInnerDRmu in MuJetORT");
      }
    } else if (OR_procedure == "noTauJetOLR") {
      top::check(m_ORtoolBox_Loose.tauJetORT.setProperty("DR", 0.0),
                 "Failed to set DR in TauJetORT to zero");
    }
    if (applyElectronInJetSubtraction) {
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("UseSlidingDR", true),
                 "Failed to setSliding DR in ElJetORT");
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("InnerDR", 0),
                 "Failed to setting inner radius equal to SlidingInnerDRel in EleJetORT");
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("SlidingDRC1", 0),
                 "Failed to set SlidingDRC1");
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("SlidingDRC2", 0),
                 "Failed to set SlidingDRC2");
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("SlidingDRMaxCone", 0),
                 "Failed to set SlidingDRMaxCone");
    }
    top::check(m_ORtoolBox_Loose.initialize(),
               "Failed to initialize loose overlap removal tools");
    m_overlapRemovalTool_Loose = std::move(m_ORtoolBox_Loose.masterTool);

    return StatusCode::SUCCESS;
  }
}  // namespace top
