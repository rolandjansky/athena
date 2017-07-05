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
  declareProperty("release_series", m_release_series );
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
  std::set<std::string> allowed_OR_procedures = {"recommended", "jetmuApplyRelPt", "harmonized",
                                                 "Boosted", "BoostedSlidingDREl", "BoostedSlidingDRMu", "BoostedSlidingDRElMu", "noTauJetOLR"};
  std::string OR_procedure = m_config->overlapRemovalProcedure();

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

  const float floatMax = std::numeric_limits<float>::max();

  top::check(ORUtils::recommendedTools(OR_flags, m_ORtoolBox),
              "Failed to setup OR Tool box");
  if (OR_procedure == "harmonized") {
    if (m_config->useMuons() && m_config->useElectrons())
      top::check(m_ORtoolBox.eleMuORT.setProperty("RemoveCaloMuons", false),
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
  }
  else if (OR_procedure == "jetmuApplyRelPt") {
      top::check(m_ORtoolBox.muJetORT.setProperty("ApplyRelPt", true),
                  "Failed to set ApplyRelPt in muJetORT");
  }
  else if ( OR_procedure == "BoostedSlidingDREl" ||
            OR_procedure == "BoostedSlidingDRMu" ||
            OR_procedure == "BoostedSlidingDRElMu") {
    if ( m_config->useElectrons() && m_config->useJets() &&
         (OR_procedure == "BoostedSlidingDREl" || OR_procedure == "BoostedSlidingDRElMu") ) {
      top::check(m_ORtoolBox.eleJetORT.setProperty("UseSlidingDR", true),
                "Failed to setSliding DR in ElJetORT");
      top::check(m_ORtoolBox.eleJetORT.setProperty("InnerDR", 0.0),
                "Failed to setting inner radius equal to 0.0 in EleJetORT");
    }
    if ( m_config->useMuons() && m_config->useJets() &&
         (OR_procedure == "BoostedSlidingDRMu" || OR_procedure == "BoostedSlidingDRElMu") ) {
      top::check(m_ORtoolBox.muJetORT.setProperty("UseSlidingDR", true),
                "Failed to setSliding DR in MuJetORT");
      top::check(m_ORtoolBox.muJetORT.setProperty("InnerDR", 0.0),
                "Failed to setting inner radius equal to 0.0 in MuJetORT");
    }
  }
  else if( OR_procedure == "noTauJetOLR"){
    top::check(m_ORtoolBox.tauJetORT.setProperty("DR", 0.0),
                "Failed to set DR in TauJetORT to zero");
  }
  top::check(m_ORtoolBox.initialize(),
              "Failed to initialize overlap removal tools");
  m_overlapRemovalTool = std::move(m_ORtoolBox.masterTool);

  OR_flags.masterName = "OverlapRemovalToolLoose";
//     OR_flags.inputLabel = "passPreORSelectionLoose";
  OR_flags.inputLabel = "ORToolDecorationLoose";

  top::check(ORUtils::recommendedTools(OR_flags, m_ORtoolBox_Loose),
              "Failed to setup OR Tool box");
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
  }
  else if (OR_procedure == "jetmuApplyRelPt") {
    top::check(m_ORtoolBox_Loose.muJetORT.setProperty("ApplyRelPt", false),
                  "Failed to set ApplyRelPt in muJetORT");
  }
  else if ( OR_procedure == "BoostedSlidingDREl" ||
            OR_procedure == "BoostedSlidingDRMu" ||
            OR_procedure == "BoostedSlidingDRElMu") {
    if ( m_config->useElectrons() && m_config->useJets() &&
         (OR_procedure == "BoostedSlidingDREl" || OR_procedure == "BoostedSlidingDRElMu") ) {
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("UseSlidingDR", true),
                "Failed to setSliding DR in ElJetORT");
      top::check(m_ORtoolBox_Loose.eleJetORT.setProperty("InnerDR", 0.0),
                "Failed to setting inner radius equal to 0.0 in EleJetORT");
    }
    if ( m_config->useMuons() && m_config->useJets() &&
         (OR_procedure == "BoostedSlidingDRMu" || OR_procedure == "BoostedSlidingDRElMu") ) {
      top::check(m_ORtoolBox_Loose.muJetORT.setProperty("UseSlidingDR", true),
                "Failed to setSliding DR in MuJetORT");
      top::check(m_ORtoolBox_Loose.muJetORT.setProperty("InnerDR", 0.0),
                "Failed to setting inner radius equal to 0.0 in MuJetORT");
    }
  }
  else if( OR_procedure == "noTauJetOLR"){
    top::check(m_ORtoolBox_Loose.tauJetORT.setProperty("DR", 0.0),
                "Failed to set DR in TauJetORT to zero");
  }
  top::check(m_ORtoolBox_Loose.initialize(),
              "Failed to initialize loose overlap removal tools");
  m_overlapRemovalTool_Loose = std::move(m_ORtoolBox_Loose.masterTool);

  return StatusCode::SUCCESS;
}

}  // namespace top
