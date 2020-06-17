/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
    m_jvt_tool("JetJvtEfficiencyTool"), 
    m_fjvt_tool("JetForwardJvtEfficiencyTool") {
    declareProperty("config", m_config);
  }

  StatusCode JetScaleFactorCalculator::initialize() {
    ATH_MSG_INFO("top::JetScaleFactorCalculator initialize");

    top::check(m_jvt_tool.retrieve(), "Failed to retrieve JVT tool");

    m_systUP.insert(CP::SystematicVariation(CP::JvtEfficiencyUp));
    m_systDOWN.insert(CP::SystematicVariation(CP::JvtEfficiencyDown));

    // fJVT Efficiency tool is only set up if user requests it
    if (m_config->getfJVTWP() != "None") {
      top::check(m_fjvt_tool.retrieve(), "Failed to retrieve fJVT tool");

      m_systUP.insert(CP::SystematicVariation(CP::fJvtEfficiencyUp));
      m_systDOWN.insert(CP::SystematicVariation(CP::fJvtEfficiencyDown));
    }


    return StatusCode::SUCCESS;
  }

  StatusCode JetScaleFactorCalculator::execute() {
    ///-- Loop over all jet collections --///
  
    for (auto currentSystematic : *m_config->systSgKeyMapJets(false)) {
      const xAOD::JetContainer* jets(nullptr);
      top::check(evtStore()->retrieve(jets, currentSystematic.second), "failed to retrieve jets in JetScaleFactorCalculator::execute()");
      top::check(this->decorateJets(jets,currentSystematic.first == m_config->nominalHashValue()),"failed to decorate jets in JetScaleFactorCalculator::execute()");
    }
    
    if(m_config->doLooseEvents() && m_config->applyElectronInJetSubtraction())
    {
      for (auto currentSystematic : *m_config->systSgKeyMapJets(true)) {
        const xAOD::JetContainer* jets(nullptr);
        top::check(evtStore()->retrieve(jets, currentSystematic.second), "failed to retrieve loose jets in JetScaleFactorCalculator::execute()");
        top::check(this->decorateJets(jets,currentSystematic.first == m_config->nominalHashValue()),"failed to decorate loose jets in JetScaleFactorCalculator::execute()");
      }
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode JetScaleFactorCalculator::decorateJets(const xAOD::JetContainer* jets, bool isNominal){
    
    /// -- Loop over all jets in each collection --///
      for (const xAOD::Jet *jetPtr : *jets) {
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
	  ///-- JVT first --///
          // Set to nominal first...
          top::check(m_jvt_tool->applySystematicVariation(m_systNominal),
                     "Failed to set JVT nominal SF");

          float jvtSF(1.);

          if (!jetPtr->isAvailable<char>("passJVT")) {
            ATH_MSG_ERROR(" Can't find jet decoration \"passJVT\" - we need it to calculate the jet scale-factors!");
            return StatusCode::FAILURE;
          }

          int passes_jvt = jetPtr->auxdataConst< char >("passJVT");
          if (passes_jvt < 0) continue;

          if (passes_jvt) top::check(m_jvt_tool->getEfficiencyScaleFactor(*jetPtr, jvtSF),
                                     "Failed to getEfficiencyScaleFactor for JVT");
          else {
            top::check(m_jvt_tool->getInefficiencyScaleFactor(*jetPtr, jvtSF),
                       "Failed to getInefficiencyScaleFactor for JVT");
          }

          jetPtr->auxdecor<float>("JET_SF_jvt") = jvtSF;

          ///-- For nominal calibration, vary the SF systematics --///
          if (isNominal) {
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
            } else {
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
	
	  ///-- Then fJVT --///
	  if (m_config->getfJVTWP() != "None") {
	    top::check(m_fjvt_tool->applySystematicVariation(m_systNominal),
		       "Failed to set fJVT nominal SF");
	    float fjvtSF(1.);

	    if (!jetPtr->isAvailable<char>("AnalysisTop_fJVTdecision")) {
	      ATH_MSG_ERROR(" Can't find jet decoration \"AnalysisTop_fJVTdecision\" - we need it to calculate the jet fJVT scale-factors!");
	      return StatusCode::FAILURE;
	    }
	    int passes_fjvt = jetPtr->auxdataConst< char >("AnalysisTop_fJVTdecision");
	    if (passes_fjvt < 0) continue;
	    if (passes_fjvt) top::check(m_fjvt_tool->getEfficiencyScaleFactor(*jetPtr, fjvtSF),
				       "Failed to getEfficiencyScaleFactor for fJVT");
	    else {
	      top::check(m_fjvt_tool->getInefficiencyScaleFactor(*jetPtr, fjvtSF),
			 "Failed to getInefficiencyScaleFactor for fJVT");
	    }
	    jetPtr->auxdecor<float>("JET_SF_fjvt") = fjvtSF;
	  
	    if (isNominal) {
	      float fjvtSF_up(1.), fjvtSF_down(1.);  // made up values
	      if (passes_fjvt) {
		top::check(m_fjvt_tool->applySystematicVariation(m_systUP),
			   "Failed to applySystematicVariation up for fJVT");
		top::check(m_fjvt_tool->getEfficiencyScaleFactor(*jetPtr, fjvtSF_up),
			   "Failed to get fJVT SF (systematic up)");
		top::check(m_fjvt_tool->applySystematicVariation(m_systDOWN),
			   "Failed to applySystematicVariation down for fJVT");
		top::check(m_fjvt_tool->getEfficiencyScaleFactor(*jetPtr, fjvtSF_down),
			   "Failed to get fJVT SF (systematic down)");
	      } else {
		top::check(m_fjvt_tool->applySystematicVariation(m_systUP),
			   "Failed to applySystematicVariation up for fJVT");
		top::check(m_fjvt_tool->getInefficiencyScaleFactor(*jetPtr, fjvtSF_up),
			   "Failed to get fJVT SF (systematic up)");
		top::check(m_fjvt_tool->applySystematicVariation(m_systDOWN),
			   "Failed to applySystematicVariation down for fJVT");
		top::check(m_fjvt_tool->getInefficiencyScaleFactor(*jetPtr, fjvtSF_down),
			   "Failed to get fJVT SF (systematic down)");
	      }
	      ///-- Decorate --///
	      jetPtr->auxdecor<float>("JET_SF_fjvt_UP") = fjvtSF_up;
	      jetPtr->auxdecor<float>("JET_SF_fjvt_DOWN") = fjvtSF_down;
	
	    } // Calibration systematic is nominal, so calculate SF systematics
	  }
        }
      }
    return StatusCode::SUCCESS;
  }
}  // namespace top
