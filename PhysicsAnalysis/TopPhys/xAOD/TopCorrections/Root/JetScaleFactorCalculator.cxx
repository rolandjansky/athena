/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetScaleFactorCalculator.cxx 794672 2017-01-31 00:41:04Z tpelzer $
#include "TopCorrections/JetScaleFactorCalculator.h"
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODJet/JetContainer.h"


namespace top {

  JetScaleFactorCalculator::JetScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),

    m_systNominal(CP::SystematicSet()),
    m_systUP(CP::SystematicSet()),
    m_systDOWN(CP::SystematicSet()),
    m_jvt_tool("JetJvtEfficiencyTool") {
    declareProperty("config" , m_config);
  }

  StatusCode JetScaleFactorCalculator::initialize() {
    ATH_MSG_INFO("top::JetScaleFactorCalculator initialize");

    top::check(m_jvt_tool.retrieve(), "Failed to retrieve JVT tool");

    m_systUP.insert(CP::SystematicVariation(CP::JvtEfficiencyUp));
    m_systDOWN.insert(CP::SystematicVariation(CP::JvtEfficiencyDown));

    return StatusCode::SUCCESS;
  }


  StatusCode JetScaleFactorCalculator::execute() {
    ///-- Loop over all jet collections --///
    ///-- Lets assume that we're not doing ElectronInJet subtraction --///
    for (auto currentSystematic : *m_config->systSgKeyMapJets(false)) {
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets, currentSystematic.second), "failed to retrieve jets");

      ///-- Tell the SF tools to use the nominal systematic --///

      /// -- Loop over all jets in each collection --///
      for (auto jetPtr : *jets) {
        /// -- Does the jet pass object selection? --///
        // WARNING: this doesn't include the JVT cut anymore
        bool passSelection(false);
        if (jetPtr->isAvailable<char>("passPreORSelection")) {
          if (jetPtr->auxdataConst<char>("passPreORSelection") == 1) {
            passSelection = true;
          }
        }
        if (jetPtr->isAvailable<char>("passPreORSelectionLoose")) {
          if (jetPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
            passSelection = true;
          }
        }

        if (passSelection) {
          // Set to nominal first...
          top::check(m_jvt_tool->applySystematicVariation(m_systNominal),
                     "Failed to set JVT nominal SF");

          float jvtSF(1.);

          if (!jetPtr->isAvailable<char>("passJVT")) {
            ATH_MSG_ERROR(" Can't find jet decoration \"passJVT\" - we need it to calculate the jet scale-factors!");
            return StatusCode::FAILURE;
          }

          bool passes_jvt = jetPtr->auxdataConst< char >("passJVT");

          if (passes_jvt)
            top::check(m_jvt_tool->getEfficiencyScaleFactor(*jetPtr, jvtSF),
                       "Failed to getEfficiencyScaleFactor for JVT");
          else {
            top::check(m_jvt_tool->getInefficiencyScaleFactor(*jetPtr, jvtSF),
                       "Failed to getInefficiencyScaleFactor for JVT");
          }

          jetPtr->auxdecor<float>("JET_SF_jvt") = jvtSF;

          ///-- For nominal calibration, vary the SF systematics --///
          if (currentSystematic.first == m_config->nominalHashValue()) {

            float jvtSF_up(1.), jvtSF_down(1.);  // made up values

            if (passes_jvt) {
              top::check(m_jvt_tool->applySystematicVariation(m_systUP),
                         "Failed to applySystematicVariation up for JVT");
              top::check(m_jvt_tool->getEfficiencyScaleFactor(*jetPtr, jvtSF_up),
                         "Failed to get JVT SF (systematic up)");
              top::check(m_jvt_tool->applySystematicVariation(m_systDOWN),
                         "Failed to applySystematicVariation down for JVT");
              top::check(m_jvt_tool->getEfficiencyScaleFactor(*jetPtr, jvtSF_down),
                         "Failed to get JVT SF (systematic down)");
            }
            else {
              top::check(m_jvt_tool->applySystematicVariation(m_systUP),
                         "Failed to applySystematicVariation up for JVT");
              top::check(m_jvt_tool->getInefficiencyScaleFactor(*jetPtr, jvtSF_up),
                         "Failed to get JVT SF (systematic up)");
              top::check(m_jvt_tool->applySystematicVariation(m_systDOWN),
                         "Failed to applySystematicVariation down for JVT");
              top::check(m_jvt_tool->getInefficiencyScaleFactor(*jetPtr, jvtSF_down),
                         "Failed to get JVT SF (systematic down)");
            }

            ///-- Decorate --///
            jetPtr->auxdecor<float>("JET_SF_jvt_UP") = jvtSF_up;
            jetPtr->auxdecor<float>("JET_SF_jvt_DOWN") = jvtSF_down;
          } // Calibration systematic is nominal, so calculate SF systematics

        }
      }
    }


    return StatusCode::SUCCESS;
  }

}  // namespace top
