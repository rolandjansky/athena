/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonScaleFactorCalculator.cxx 805964 2017-06-05 19:27:16Z iconnell $
#include "TopCorrections/MuonScaleFactorCalculator.h"

#include <string>
#include <vector>
#include <set>

#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODMuon/MuonContainer.h"

namespace top {

  MuonScaleFactorCalculator::MuonScaleFactorCalculator(const std::string& name) :
    asg::AsgTool(name),
    m_config(nullptr),
    m_systNominal(CP::SystematicSet()),

    m_muonEfficiencyCorrectionsTool("CP::MuonEfficiencyScaleFactorsTool"),
    m_muonEfficiencyCorrectionsToolLoose("CP::MuonEfficiencyScaleFactorsToolLoose"),
    m_muonEfficiencyCorrectionsToolIso("CP::MuonEfficiencyScaleFactorsToolIso"),
    m_muonEfficiencyCorrectionsToolLooseIso("CP::MuonEfficiencyScaleFactorsToolLooseIso"),
    m_muonEfficiencyCorrectionsToolTTVA("CP::MuonEfficiencyScaleFactorsToolTTVA"),

    m_decor_triggerEff("SetMe"), m_decor_triggerEff_loose("SetMe"),
    m_decor_triggerSF("SetMe"), m_decor_triggerSF_loose("SetMe"),
    m_decor_idSF("SetMe"), m_decor_idSF_loose("SetMe"),
    m_decor_isoSF("SetMe"), m_decor_isoSF_loose("SetMe"),
    m_decor_TTVA("MU_SF_TTVA"),

    m_do_muon_isolation_SFs(true),
    m_do_muon_isolation_SFs_loose(true),
    // The systematics are defined here...
    m_trig_sf_stat_UP("MUON_EFF_TrigStatUncertainty__1up"),
    m_trig_sf_stat_DOWN("MUON_EFF_TrigStatUncertainty__1down"),
    m_trig_sf_syst_UP("MUON_EFF_TrigSystUncertainty__1up"),
    m_trig_sf_syst_DOWN("MUON_EFF_TrigSystUncertainty__1down"),

    m_reco_stat_UP("MUON_EFF_STAT__1up"),
    m_reco_stat_DOWN("MUON_EFF_STAT__1down"),
    m_reco_syst_UP("MUON_EFF_SYS__1up"),
    m_reco_syst_DOWN("MUON_EFF_SYS__1down"),

    m_reco_stat_lowpt_UP("MUON_EFF_STAT_LOWPT__1up"),
    m_reco_stat_lowpt_DOWN("MUON_EFF_STAT_LOWPT__1down"),
    m_reco_syst_lowpt_UP("MUON_EFF_SYS_LOWPT__1up"),
    m_reco_syst_lowpt_DOWN("MUON_EFF_SYS_LOWPT__1down"),

    m_iso_stat_UP("MUON_ISO_STAT__1up"),
    m_iso_stat_DOWN("MUON_ISO_STAT__1down"),
    m_iso_syst_UP("MUON_ISO_SYS__1up"),
    m_iso_syst_DOWN("MUON_ISO_SYS__1down"),

    m_TTVA_stat_UP("MUON_TTVA_STAT__1up"),
    m_TTVA_stat_DOWN("MUON_TTVA_STAT__1down"),
    m_TTVA_syst_UP("MUON_TTVA_SYS__1up"),
    m_TTVA_syst_DOWN("MUON_TTVA_SYS__1down") {
    declareProperty("config", m_config);
  }

  StatusCode MuonScaleFactorCalculator::initialize() {
    ATH_MSG_INFO(" top::MuonScaleFactorCalculator initialize");
    
    // Different tools to retrieve, depending on the release
    if(m_config->getReleaseSeries() == 24){
      m_muonTriggerScaleFactors_2015      = ToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors_2015");
      m_muonTriggerScaleFactorsLoose_2015 = ToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactorsLoose_2015");
      m_muonTriggerScaleFactors_2016      = ToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors_2016");
      m_muonTriggerScaleFactorsLoose_2016 = ToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactorsLoose_2016");
    }
    if(m_config->getReleaseSeries() == 25){
      m_muonTriggerScaleFactors_R21      = ToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactors_R21");
      m_muonTriggerScaleFactorsLoose_R21 = ToolHandle<CP::IMuonTriggerScaleFactors>("CP::MuonTriggerScaleFactorsLoose_R21");
    }

    std::set<std::string> implemented_systematics;
    // Default release 20.7 implemented systematic names
    if(m_config->getReleaseSeries() == 24){
      implemented_systematics = {"MUON_EFF_TrigStatUncertainty",
				 "MUON_EFF_TrigSystUncertainty",
				 "MUON_EFF_STAT",
				 "MUON_EFF_SYS",
				 "MUON_EFF_STAT_LOWPT",
				 "MUON_EFF_SYS_LOWPT",
				 "MUON_ISO_STAT",
				 "MUON_ISO_SYS",
				 "MUON_TTVA_STAT",
				 "MUON_TTVA_SYS"};
    }
    // Updated systematic names for release 21
    else if(m_config->getReleaseSeries() == 25){
      implemented_systematics = {"MUON_EFF_RECO_STAT",
				 "MUON_EFF_RECO_STAT_LOWPT",
				 "MUON_EFF_RECO_SYS",
				 "MUON_EFF_RECO_SYS_LOWPT",
				 "MUON_EFF_TrigStatUncertainty",
				 "MUON_EFF_TrigSystUncertainty",
				 "MUON_EFF_ISO_STAT",
				 "MUON_EFF_ISO_SYS",
				 "MUON_EFF_TTVA_STAT",
				 "MUON_EFF_TTVA_SYS"};
    }
    else{
      throw std::runtime_error("TopCorrections :: MuonScaleFactorCalculator :: Unable to identify expected release series from configuration");
    }

    std::set<std::string> recommended_systematics;
    // For R20.7, tool for each year
    if(m_config->getReleaseSeries() == 24){
      this->retrieveSystematicTool(m_muonTriggerScaleFactors_2015,
				   recommended_systematics);
      this->retrieveSystematicTool(m_muonTriggerScaleFactorsLoose_2015,
				   recommended_systematics);
      this->retrieveSystematicTool(m_muonTriggerScaleFactors_2016,
				   recommended_systematics);
      this->retrieveSystematicTool(m_muonTriggerScaleFactorsLoose_2016,
				   recommended_systematics);
    }
    // For R21, a single tool
    if(m_config->getReleaseSeries() == 25){

      this->retrieveSystematicTool(m_muonTriggerScaleFactors_R21,
                                   recommended_systematics);
      this->retrieveSystematicTool(m_muonTriggerScaleFactorsLoose_R21,
                                   recommended_systematics);
    }

    this->retrieveSystematicTool(m_muonEfficiencyCorrectionsTool,
				 recommended_systematics);
    this->retrieveSystematicTool(m_muonEfficiencyCorrectionsToolLoose,
				 recommended_systematics);


    if (asg::ToolStore::contains<CP::IMuonEfficiencyScaleFactors>("CP::MuonEfficiencyScaleFactorsToolIso")) {
      this->retrieveSystematicTool(m_muonEfficiencyCorrectionsToolIso,
                                   recommended_systematics);
    } else {
      m_do_muon_isolation_SFs = false;
    }
    if (asg::ToolStore::contains<CP::IMuonEfficiencyScaleFactors>("CP::MuonEfficiencyScaleFactorsToolLooseIso")) {
      this->retrieveSystematicTool(m_muonEfficiencyCorrectionsToolLooseIso,
                                   recommended_systematics);
    } else {
      m_do_muon_isolation_SFs_loose = false;
    }

    ///-- Not a good reason for this happening at the moment --///
    if (m_config->muonIsolation() != "None" && !m_do_muon_isolation_SFs) {
      ATH_MSG_WARNING("No muon isolation SFs available."
                      " You can run with MuonIsolation set to"
                      " None if you don't want isolation.");
      return StatusCode::FAILURE;
    }
    ///-- Not a good reason for this happening at the moment --///
    if (m_config->muonIsolationLoose() != "None" && !m_do_muon_isolation_SFs_loose) {
      ATH_MSG_WARNING("No loose muon isolation SFs available."
                      " You can run with MuonIsolationLoose set to"
                      " None if you don't want isolation.");
      return StatusCode::FAILURE;
    }

    this->retrieveSystematicTool(m_muonEfficiencyCorrectionsToolTTVA,
                                 recommended_systematics);

    top::check(this->checkSystematicsImplemented(recommended_systematics,
                                                 implemented_systematics),
               "There are differences between the recommended and implemented"
               " muon SF systematic uncertainties");


    m_decor_triggerEff       = "MU_EFF_Trigger_"  + m_config->muonQuality();
    m_decor_triggerEff_loose = "MU_EFF_Trigger_"  + m_config->muonQualityLoose();
    m_decor_triggerSF        = "MU_SF_Trigger_"   + m_config->muonQuality();
    m_decor_triggerSF_loose  = "MU_SF_Trigger_"   + m_config->muonQualityLoose();
    m_decor_idSF             = "MU_SF_ID_"        + m_config->muonQuality();
    m_decor_idSF_loose       = "MU_SF_ID_"        + m_config->muonQualityLoose();
    m_decor_isoSF            = "MU_SF_Isol_"      + m_config->muonIsolation();
    m_decor_isoSF_loose      = "MU_SF_Isol_"      + m_config->muonIsolationLoose();

    return StatusCode::SUCCESS;
  }


  StatusCode MuonScaleFactorCalculator::execute() {
    /*************************************************************
     *
     *  The muon trigger SF tool wants a random run number,
     *  which we can get from the PRW tool.
     *
     *************************************************************/

    if (m_config->doPileupReweighting()) {
      const xAOD::EventInfo* eventInfo(nullptr);
      top::check(evtStore()->retrieve(eventInfo, m_config->sgKeyEventInfo()),
                 "Failed to retrieve EventInfo");

      // This is the default value in the MuonTriggerScaleFactors tool
      unsigned int runNumber = 267639;
      if (eventInfo->isAvailable<unsigned int>("RandomRunNumber"))
        runNumber = eventInfo->auxdataConst<unsigned int>("RandomRunNumber");
      else
        ATH_MSG_WARNING("Event has not been decorated with RandomRunNumber");

      if(m_config->getReleaseSeries() == 24){
	if (runNumber > 284484 || runNumber == 0) {
	  m_muonTriggerScaleFactors = m_muonTriggerScaleFactors_2016;
	  m_muonTriggerScaleFactorsLoose = m_muonTriggerScaleFactors_2016;
	  m_muon_trigger_sf_config = "HLT_mu26_ivarmedium_OR_HLT_mu50";
	} else {
	  m_muonTriggerScaleFactors = m_muonTriggerScaleFactors_2015;
	  m_muonTriggerScaleFactorsLoose = m_muonTriggerScaleFactors_2015;
	  m_muon_trigger_sf_config = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
	}
      }

      if(m_config->getReleaseSeries() == 25){
	// Keep an eye here for trigger strings
	// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15#Supported_triggers
	m_muonTriggerScaleFactors      = m_muonTriggerScaleFactors_R21;
	m_muonTriggerScaleFactorsLoose = m_muonTriggerScaleFactorsLoose_R21;
	m_muon_trigger_sf_config       = "";

	if(runNumber == 0){
	  m_muon_trigger_sf_config = "HLT_mu26_ivarmedium_OR_HLT_mu50";
	}
	// 2015
	else if(runNumber > 0 && runNumber <= 284484){
	  m_muon_trigger_sf_config = "HLT_mu20_iloose_L1MU15_OR_HLT_mu50";
	}
	// 2016 (set to a large value but split as we may need to have strings for 2016 -> certain 2017 run)
	else if (runNumber > 284484 && runNumber < 324320) {
	  m_muon_trigger_sf_config = "HLT_mu26_ivarmedium_OR_HLT_mu50";
	}
	// 2017+ (324320+)
	else{
	  m_muon_trigger_sf_config = "HLT_mu26_ivarmedium_OR_HLT_mu50"; 
	}
      }
      ATH_MSG_DEBUG("Muon trigger scale factor config is : "+m_muon_trigger_sf_config);
      ATH_MSG_DEBUG("RunNumber (0 < 2015 < 284484 < 2016 < 324320 < 2017) : ");
      ATH_MSG_DEBUG(runNumber);

      // This is now only needed in R20.7
      // R21+ will be smarter
      if(m_config->getReleaseSeries() == 24){
	// The PRW tool can give run numbers of zero for unrepresented
	// mu values. If we give the muon tool a run number of zero it
	// complains with the message:
	// "WARNING I am using run #0 but I cannot find corresponding run period.
	// Now setting to use 2016 period B.
	// This might give problems! Please check which year and mc you
	// have set up".
	// We do this by hand to avoid the messages.
	if (runNumber == 0) runNumber = 300345;	

	// <iconnell@cern.ch> - The function was removed from R21 but we will use preprocessor
	// statements to protect the compilation until we are happy for the two releases to be out of sync
        #if ROOTCORE_RELEASE_SERIES < 25 // Release 2.4.X
	top::check(m_muonTriggerScaleFactors->setRunNumber(runNumber),
		   "Failed to set run number for muon trigger SFs");
	top::check(m_muonTriggerScaleFactorsLoose->setRunNumber(runNumber),
		   "Failed to set run number for (loose) muon trigger SFs");
        #endif // Release 2.4.X

      }
    }

    ///-- Loop over all muon collections --///
    for (auto currentSystematic : *m_config->systSgKeyMapMuons()) {
      const xAOD::MuonContainer* muons(nullptr);
      top::check(evtStore()->retrieve(muons, currentSystematic.second),
                 "Failed to retrieve muons");

      /// -- Loop over all muons in each collection --///
      for (auto muonPtr : *muons) {
        /// -- Does the muon pass object selection? --///
        bool passSelection(false);
        if (muonPtr->isAvailable<char>("passPreORSelection")) {
          if (muonPtr->auxdataConst<char>("passPreORSelection") == 1) {
            passSelection = true;
          }
        }
        if (muonPtr->isAvailable<char>("passPreORSelectionLoose")) {
          if (muonPtr->auxdataConst<char>("passPreORSelectionLoose") == 1) {
            passSelection = true;
          }
        }

        if (!passSelection) continue;

        // Tell the SF tools to use the nominal systematic
        // To be extra sure we do this when we try and get a SF too!
        this->applySystematicVariation(m_muonTriggerScaleFactors, m_systNominal);
        this->applySystematicVariation(m_muonTriggerScaleFactorsLoose, m_systNominal);
        this->applySystematicVariation(m_muonEfficiencyCorrectionsTool, m_systNominal);
        this->applySystematicVariation(m_muonEfficiencyCorrectionsToolLoose, m_systNominal);
        this->applySystematicVariation(m_muonEfficiencyCorrectionsToolTTVA, m_systNominal);

        //-- Only do isolation if we actually get the tools --//
        if (m_do_muon_isolation_SFs)
          this->applySystematicVariation(m_muonEfficiencyCorrectionsToolIso, m_systNominal);
        if (m_do_muon_isolation_SFs_loose)
          this->applySystematicVariation(m_muonEfficiencyCorrectionsToolLooseIso, m_systNominal);

        // The nominal SFs/efficiencies.
        // Include are::
        // 1. ID SF (no reco Eff since 2.4.22)
        // 2. Isolation SF
        // 3. TTVA SF
        // 4. Trigger SF and efficiency

        // Reco efficiency and ID SF decorations
        static SG::AuxElement::Decorator<float> id_sf_decor(m_decor_idSF);
        static SG::AuxElement::Decorator<float> id_sf_loose_decor(m_decor_idSF_loose);

        this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                     m_systNominal, *muonPtr,
                                     id_sf_decor);
        this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                     m_systNominal, *muonPtr,
                                     id_sf_loose_decor);

        // Isolation SF decorations. If we aren't using isolation for
        // either tight or loose muons then decorate anyway with 1.0.
        static SG::AuxElement::Decorator<float> iso_sf_decor(m_decor_isoSF);
        static SG::AuxElement::Decorator<float> iso_sf_loose_decor(m_decor_isoSF_loose);

        if (m_do_muon_isolation_SFs) {
          this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolIso,
                                     m_systNominal, *muonPtr,
                                     iso_sf_decor);
        } else {
          iso_sf_decor(*muonPtr) = 1.0;
        }
        if (m_do_muon_isolation_SFs_loose) {
          this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolLooseIso,
                                     m_systNominal, *muonPtr,
                                     iso_sf_loose_decor);
        } else {
          iso_sf_loose_decor(*muonPtr) = 1.0;
        }

        // TTVA decoration
        static SG::AuxElement::Decorator<float> TTVA_decor(m_decor_TTVA);
        this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolTTVA,
                                   m_systNominal, *muonPtr, TTVA_decor);

        /**********************************************************************
         * Muon Trigger Scale Factors
         * TOM : The MCP tool takes a container of muons. The examples seem to
         *       suggest these should somehow be selected muons but it isn't clear
         *       to me what level of selection should be applied.
         *
         * *** To be followed up with MCP people by Jay ***
         *
         * TOM : We trick it by creating a view container
         *       with a single muon in.
         * NOTE: This also happens later on for the systematics so if we ever
         *       get a nice function make sure we change it there too!
         *
         **********************************************************************/

        ConstDataVector<xAOD::MuonContainer> trigger_SF_muon(SG::VIEW_ELEMENTS);
        trigger_SF_muon.push_back(muonPtr);

        static SG::AuxElement::Decorator<float> trig_sf_decor(m_decor_triggerSF);
        static SG::AuxElement::Decorator<float> trig_sf_loose_decor(m_decor_triggerSF_loose);
        static SG::AuxElement::Decorator<float> trig_eff_decor(m_decor_triggerEff);
        static SG::AuxElement::Decorator<float> trig_eff_loose_decor(m_decor_triggerEff_loose);

        this->decorateTrigSFandEff(m_muonTriggerScaleFactors,
                                   m_systNominal, trigger_SF_muon,
                                   trig_sf_decor, trig_eff_decor);
        this->decorateTrigSFandEff(m_muonTriggerScaleFactorsLoose,
                                   m_systNominal, trigger_SF_muon,
                                   trig_sf_loose_decor, trig_eff_decor);

        // If we are running on the nominal tree, then do the
        // SF systematic variations too.
        // Otherwise just move onto the next muon...
        if (currentSystematic.first != m_config->nominalHashValue()) continue;

        // Trigger systematics
        this->decorateTriggerSystematics(trigger_SF_muon);
        // ID systematics
        this->decorateIDSFandRecoEffSystematics(*muonPtr);
        // Isolation systematics
        this->decorateIsolationSystematics(*muonPtr);
        // TTVA muon SFs- track-to-vertex association
        this->decorateTTVASystematics(*muonPtr);
      }
    }
    return StatusCode::SUCCESS;
  }

  // End of public functions

  template<typename T>
  void MuonScaleFactorCalculator::retrieveSystematicTool(const ToolHandle<T>& tool,
                                                         std::set<std::string>& recommended_systematics) {
    top::check(tool.retrieve(), "Failed to retrieve " + tool.name());
    // Add all recommended systematics to set
    for (auto s : tool->recommendedSystematics().getBaseNames())
      recommended_systematics.insert(std::string(s));
  }


  template<typename T>
  void MuonScaleFactorCalculator::applySystematicVariation(ToolHandle<T>& tool,
                                                           const CP::SystematicSet& systematic) {
    top::check(tool->applySystematicVariation(systematic),
               "Failed to set " + tool.name()
               + " to " + (systematic.empty()
                           ? "Nominal" : systematic.name()));
  }

  StatusCode MuonScaleFactorCalculator::checkSystematicsImplemented(const std::set<std::string>& recommended,
                                                                    const std::set<std::string>& implemented) {
    std::vector<std::string> different_systematics = {};
    std::set_difference(recommended.begin(),
                        recommended.end(),
                        implemented.begin(),
                        implemented.end(),
                        std::back_inserter(different_systematics));
    if (different_systematics.size()) {
      ATH_MSG_WARNING("WE ARE EXPECTING THE FOLLOWING SYSTEMATICS:");
      for (auto syst : recommended)
        ATH_MSG_WARNING("\t" << syst);
      ATH_MSG_WARNING("WE HAVE THE FOLLOWING SYSTEMATICS:");
      for (auto syst : implemented)
        ATH_MSG_WARNING("\t" << syst);
      ATH_MSG_WARNING("WE ARE MISSING THE FOLLOWING SYSTEMATICS:");
      for (auto syst : different_systematics)
        ATH_MSG_WARNING("\t" << syst);
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  void MuonScaleFactorCalculator::decorateTriggerEfficiency(ToolHandle<CP::IMuonTriggerScaleFactors>& tool,
                                                            const xAOD::Muon& muon,
                                                            const SG::AuxElement::Decorator<float>& decor) {
    double trigEff = 1.0;
    top::check(tool->getTriggerEfficiency(muon, trigEff,
                                          m_muon_trigger_sf_config,
                                          !m_config->isMC()),
               "Failed to get muon trigger efficiency");
    decor(muon) = trigEff;
  }

  void MuonScaleFactorCalculator::decorateTriggerSF(ToolHandle<CP::IMuonTriggerScaleFactors>& tool,
                                                    const ConstDataVector<xAOD::MuonContainer>& muon_cont,
                                                    const SG::AuxElement::Decorator<float>& decor) {
    double trigSF = 1.0;
    top::check(tool->getTriggerScaleFactor(*muon_cont.asDataVector(), trigSF,
                                           m_muon_trigger_sf_config),
               "Failed to get muon trigger scale factor");
    decor(*muon_cont[0]) = trigSF;
  }

  void MuonScaleFactorCalculator::decorateTrigSFandEff(ToolHandle<CP::IMuonTriggerScaleFactors>& tool,
                                                       const CP::SystematicSet& systematic,
                                                       const ConstDataVector<xAOD::MuonContainer>& muon_cont,
                                                       const SG::AuxElement::Decorator<float>& sf_decor,
                                                       const SG::AuxElement::Decorator<float>& eff_decor) {
    // Set tool to systematic
    this->applySystematicVariation(tool, systematic);
    // Decorate with Trigger SF
    this->decorateTriggerSF(tool, muon_cont, sf_decor);
    // Decorate with Trigger Efficiency
    this->decorateTriggerEfficiency(tool, *muon_cont[0], eff_decor);
  }

  void MuonScaleFactorCalculator::decorateTriggerSystematics(const ConstDataVector<xAOD::MuonContainer>& trigger_SF_muon) {
    // Perform all trigger systematic variations, for loose and tight,
    // (stat/syst up/down)
    static SG::AuxElement::Decorator<float> trig_eff_decor_stat_up(m_decor_triggerEff + "_STAT_UP");
    static SG::AuxElement::Decorator<float> trig_eff_loose_decor_stat_up(m_decor_triggerEff_loose + "_STAT_UP");
    static SG::AuxElement::Decorator<float> trig_sf_decor_stat_up(m_decor_triggerSF + "_STAT_UP");
    static SG::AuxElement::Decorator<float> trig_sf_loose_decor_stat_up(m_decor_triggerSF_loose + "_STAT_UP");

    static SG::AuxElement::Decorator<float> trig_eff_decor_stat_down(m_decor_triggerEff + "_STAT_DOWN");
    static SG::AuxElement::Decorator<float> trig_eff_loose_decor_stat_down(m_decor_triggerEff_loose + "_STAT_DOWN");
    static SG::AuxElement::Decorator<float> trig_sf_decor_stat_down(m_decor_triggerSF + "_STAT_DOWN");
    static SG::AuxElement::Decorator<float> trig_sf_loose_decor_stat_down(m_decor_triggerSF_loose + "_STAT_DOWN");

    static SG::AuxElement::Decorator<float> trig_eff_decor_syst_up(m_decor_triggerEff + "_SYST_UP");
    static SG::AuxElement::Decorator<float> trig_eff_loose_decor_syst_up(m_decor_triggerEff_loose + "_SYST_UP");
    static SG::AuxElement::Decorator<float> trig_sf_decor_syst_up(m_decor_triggerSF + "_SYST_UP");
    static SG::AuxElement::Decorator<float> trig_sf_loose_decor_syst_up(m_decor_triggerSF_loose + "_SYST_UP");

    static SG::AuxElement::Decorator<float> trig_eff_decor_syst_down(m_decor_triggerEff + "_SYST_DOWN");
    static SG::AuxElement::Decorator<float> trig_eff_loose_decor_syst_down(m_decor_triggerEff_loose + "_SYST_DOWN");
    static SG::AuxElement::Decorator<float> trig_sf_decor_syst_down(m_decor_triggerSF + "_SYST_DOWN");
    static SG::AuxElement::Decorator<float> trig_sf_loose_decor_syst_down(m_decor_triggerSF_loose + "_SYST_DOWN");

    // Trigger variations
    //
    // Stat UP
    this->decorateTrigSFandEff(m_muonTriggerScaleFactors,
                               m_trig_sf_stat_UP, trigger_SF_muon,
                               trig_sf_decor_stat_up,
                               trig_eff_decor_stat_up);

    this->decorateTrigSFandEff(m_muonTriggerScaleFactorsLoose,
                               m_trig_sf_stat_UP, trigger_SF_muon,
                               trig_sf_loose_decor_stat_up,
                               trig_eff_loose_decor_stat_up);

    // Stat DOWN
    this->decorateTrigSFandEff(m_muonTriggerScaleFactors,
                               m_trig_sf_stat_DOWN, trigger_SF_muon,
                               trig_sf_decor_stat_down,
                               trig_eff_decor_stat_down);

    this->decorateTrigSFandEff(m_muonTriggerScaleFactorsLoose,
                               m_trig_sf_stat_DOWN, trigger_SF_muon,
                               trig_sf_loose_decor_stat_down,
                               trig_eff_loose_decor_stat_down);

    // Syst UP
    this->decorateTrigSFandEff(m_muonTriggerScaleFactors,
                               m_trig_sf_syst_UP, trigger_SF_muon,
                               trig_sf_decor_syst_up,
                               trig_eff_decor_syst_up);

    this->decorateTrigSFandEff(m_muonTriggerScaleFactorsLoose,
                               m_trig_sf_syst_UP, trigger_SF_muon,
                               trig_sf_loose_decor_syst_up,
                               trig_eff_loose_decor_syst_up);

    // Syst DOWN
    this->decorateTrigSFandEff(m_muonTriggerScaleFactors,
                               m_trig_sf_syst_DOWN, trigger_SF_muon,
                               trig_sf_decor_syst_down,
                               trig_eff_decor_syst_down);

    this->decorateTrigSFandEff(m_muonTriggerScaleFactorsLoose,
                               m_trig_sf_syst_DOWN, trigger_SF_muon,
                               trig_sf_loose_decor_syst_down,
                               trig_eff_loose_decor_syst_down);
  }

  void MuonScaleFactorCalculator::decorateEfficiencySF(ToolHandle<CP::IMuonEfficiencyScaleFactors>& tool,
                                                       const CP::SystematicSet& systematic,
                                                       const xAOD::Muon& muon,
                                                       const SG::AuxElement::Decorator<float>& decor) {
    // 1. Set tool to systematic
    this->applySystematicVariation(tool, systematic);

    float efficiency_SF = 1.0;
    // 2. Decorate muon with efficiency SF
    top::check(tool->getEfficiencyScaleFactor(muon, efficiency_SF),
               "Failed to get " + tool.name()
               + " efficiency SF for " + (systematic.empty()
                                          ? "Nominal" : systematic.name()));
    decor(muon) = efficiency_SF;
  }

  void MuonScaleFactorCalculator::decorateIsolationSystematics(const xAOD::Muon& muon) {
    // Decorate 'tight' isolation SFs
    static SG::AuxElement::Decorator<float> iso_sf_decor_stat_up(m_decor_isoSF+"_STAT_UP");
    static SG::AuxElement::Decorator<float> iso_sf_decor_stat_down(m_decor_isoSF+"_STAT_DOWN");
    static SG::AuxElement::Decorator<float> iso_sf_decor_syst_up(m_decor_isoSF+"_SYST_UP");
    static SG::AuxElement::Decorator<float> iso_sf_decor_syst_down(m_decor_isoSF+"_SYST_DOWN");

    // Decorate 'loose' isolation SFs
    static SG::AuxElement::Decorator<float> iso_sf_loose_decor_stat_up(m_decor_isoSF_loose+"_STAT_UP");
    static SG::AuxElement::Decorator<float> iso_sf_loose_decor_stat_down(m_decor_isoSF_loose+"_STAT_DOWN");
    static SG::AuxElement::Decorator<float> iso_sf_loose_decor_syst_up(m_decor_isoSF_loose+"_SYST_UP");
    static SG::AuxElement::Decorator<float> iso_sf_loose_decor_syst_down(m_decor_isoSF_loose+"_SYST_DOWN");

    if (m_do_muon_isolation_SFs) {
      ///-- Stat UP --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolIso,
                                 m_iso_stat_UP, muon,
                                 iso_sf_decor_stat_up);
      ///-- Stat DOWN --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolIso,
                                 m_iso_stat_DOWN, muon,
                                 iso_sf_decor_stat_down);
      ///-- Syst UP --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolIso,
                                 m_iso_syst_UP, muon,
                                 iso_sf_decor_syst_up);
      ///-- Syst DOWN --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolIso,
                                 m_iso_syst_DOWN, muon,
                                 iso_sf_decor_syst_down);
    } else {
      // If we aren't using muon isolation then still
      // decorate with isolation SFs of 1.0
      iso_sf_decor_stat_up(muon) = 1.0;
      iso_sf_decor_stat_down(muon) = 1.0;
      iso_sf_decor_syst_up(muon) = 1.0;
      iso_sf_decor_syst_down(muon) = 1.0;
    }

    if (m_do_muon_isolation_SFs_loose) {
      ///-- Stat UP --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolLooseIso,
                                 m_iso_stat_UP, muon,
                                 iso_sf_loose_decor_stat_up);
      ///-- Stat DOWN --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolLooseIso,
                                 m_iso_stat_DOWN, muon,
                                 iso_sf_loose_decor_stat_down);
      ///-- Syst UP --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolLooseIso,
                                 m_iso_syst_UP, muon,
                                 iso_sf_loose_decor_syst_up);
      ///-- Syst DOWN --///
      this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolLooseIso,
                                 m_iso_syst_DOWN, muon,
                                 iso_sf_loose_decor_syst_down);
    } else {
      // If we aren't using muon isolation then still
      // decorate with isolation SFs of 1.0
      iso_sf_loose_decor_stat_up(muon) = 1.0;
      iso_sf_loose_decor_stat_down(muon) = 1.0;
      iso_sf_loose_decor_syst_up(muon) = 1.0;
      iso_sf_loose_decor_syst_down(muon) = 1.0;
    }
  }

  void MuonScaleFactorCalculator::decorateTTVASystematics(const xAOD::Muon& muon) {
    static SG::AuxElement::Decorator<float> TTVA_decor_stat_up(m_decor_TTVA+"_STAT_UP");
    static SG::AuxElement::Decorator<float> TTVA_decor_stat_down(m_decor_TTVA+"_STAT_DOWN");
    static SG::AuxElement::Decorator<float> TTVA_decor_syst_up(m_decor_TTVA+"_SYST_UP");
    static SG::AuxElement::Decorator<float> TTVA_decor_syst_down(m_decor_TTVA+"_SYST_DOWN");

    this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolTTVA,
                               m_TTVA_stat_UP, muon, TTVA_decor_stat_up);
    this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolTTVA,
                               m_TTVA_stat_DOWN, muon, TTVA_decor_stat_down);
    this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolTTVA,
                               m_TTVA_syst_UP, muon, TTVA_decor_syst_up);
    this->decorateEfficiencySF(m_muonEfficiencyCorrectionsToolTTVA,
                               m_TTVA_syst_DOWN, muon, TTVA_decor_syst_down);
  }

  void MuonScaleFactorCalculator::decorateIDSFandRecoEff(ToolHandle<CP::IMuonEfficiencyScaleFactors>& tool,
                                                         const CP::SystematicSet& systematic,
                                                         const xAOD::Muon& muon,
                                                         const SG::AuxElement::Decorator<float>& id_sf_decor) {
    // 1. Set the tool to apply systematic and
    // decorate the ID efficiency using decorateEfficiencySF()
    this->decorateEfficiencySF(tool, systematic, muon, id_sf_decor);
    // 2. No reco Eff since 2.4.22
  }

  void MuonScaleFactorCalculator::decorateIDSFandRecoEffSystematics(const xAOD::Muon& muon) {
    static SG::AuxElement::Decorator<float> id_sf_decor_stat_up(m_decor_idSF + "_STAT_UP");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_stat_up(m_decor_idSF_loose + "_STAT_UP");

    static SG::AuxElement::Decorator<float> id_sf_decor_stat_down(m_decor_idSF + "_STAT_DOWN");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_stat_down(m_decor_idSF_loose + "_STAT_DOWN");

    static SG::AuxElement::Decorator<float> id_sf_decor_syst_up(m_decor_idSF + "_SYST_UP");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_syst_up(m_decor_idSF_loose + "_SYST_UP");

    static SG::AuxElement::Decorator<float> id_sf_decor_syst_down(m_decor_idSF + "_SYST_DOWN");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_syst_down(m_decor_idSF_loose + "_SYST_DOWN");


    ///-- Stat UP --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_stat_UP, muon,
                                 id_sf_decor_stat_up);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_stat_UP, muon,
                                 id_sf_loose_decor_stat_up);

    ///-- Stat DOWN --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_stat_DOWN, muon,
                                 id_sf_decor_stat_down);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_stat_DOWN, muon,
                                 id_sf_loose_decor_stat_down);
    ///-- Syst UP --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_syst_UP, muon,
                                 id_sf_decor_syst_up);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_syst_UP, muon,
                                 id_sf_loose_decor_syst_up);

    ///-- Syst DOWN --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_syst_DOWN, muon,
                                 id_sf_decor_syst_down);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_syst_DOWN, muon,
                                 id_sf_loose_decor_syst_down);

    static SG::AuxElement::Decorator<float> id_sf_decor_stat_lowpt_up(m_decor_idSF + "_STAT_LOWPT_UP");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_stat_lowpt_up(m_decor_idSF_loose + "_STAT_LOWPT_UP");

    static SG::AuxElement::Decorator<float> id_sf_decor_stat_lowpt_down(m_decor_idSF + "_STAT_LOWPT_DOWN");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_stat_lowpt_down(m_decor_idSF_loose + "_STAT_LOWPT_DOWN");

    static SG::AuxElement::Decorator<float> id_sf_decor_syst_lowpt_up(m_decor_idSF + "_SYST_LOWPT_UP");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_syst_lowpt_up(m_decor_idSF_loose + "_SYST_LOWPT_UP");

    static SG::AuxElement::Decorator<float> id_sf_decor_syst_lowpt_down(m_decor_idSF + "_SYST_LOWPT_DOWN");
    static SG::AuxElement::Decorator<float> id_sf_loose_decor_syst_lowpt_down(m_decor_idSF_loose + "_SYST_LOWPT_DOWN");

    ///-- Stat UP --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_stat_lowpt_UP, muon,
                                 id_sf_decor_stat_lowpt_up);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_stat_lowpt_UP, muon,
                                 id_sf_loose_decor_stat_lowpt_up);

    ///-- Stat DOWN --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_stat_lowpt_DOWN, muon,
                                 id_sf_decor_stat_lowpt_down);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_stat_lowpt_DOWN, muon,
                                 id_sf_loose_decor_stat_lowpt_down);
    ///-- Syst UP --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_syst_lowpt_UP, muon,
                                 id_sf_decor_syst_lowpt_up);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_syst_lowpt_UP, muon,
                                 id_sf_loose_decor_syst_lowpt_up);

    ///-- Syst DOWN --///
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsTool,
                                 m_reco_syst_lowpt_DOWN, muon,
                                 id_sf_decor_syst_lowpt_down);
    this->decorateIDSFandRecoEff(m_muonEfficiencyCorrectionsToolLoose,
                                 m_reco_syst_lowpt_DOWN, muon,
                                 id_sf_loose_decor_syst_lowpt_down);
  }
}  // namespace top
