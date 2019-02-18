/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TopCPTools/TopOtherCPTools.h"

#include <map>
#include <string>
#include <cstdio>

// Top includes
#include "TopConfiguration/TopConfig.h"
#include "TopEvent/EventTools.h"

// PathResolver include(s):
#include "PathResolver/PathResolver.h"

// GRL include(s):
#include "GoodRunsLists/GoodRunsListSelectionTool.h"

// Pileup Reweighting include(s):
#include "PileupReweighting/PileupReweightingTool.h"

// PMG include(s):
#include "PMGTools/PMGTruthWeightTool.h"

namespace top {

OtherCPTools::OtherCPTools(const std::string& name) :
    asg::AsgTool(name) {
  declareProperty("config", m_config);
  declareProperty("release_series", m_release_series );

  declareProperty( "GRLTool" , m_grlTool );
}

StatusCode OtherCPTools::initialize() {
  ATH_MSG_INFO("top::OtherCPTools initialize...");

  if (!m_config->isMC() || m_config->PileupUseGRLTool())
    top::check(setupGRL(), "Failed to setup GRL tool");

  if (m_config->doPileupReweighting())
    top::check(setupPileupReweighting(), "Failed to setup pile-up reweighting");

  if (m_config->isMC())
    top::check(setupPMGTools(), "Failed to setup PMG tools");

  return StatusCode::SUCCESS;
}

StatusCode OtherCPTools::setupGRL() {
  using IGRLTool = IGoodRunsListSelectionTool;
  const std::string grl_tool_name = "GoodRunsListSelectionTool";
  if (asg::ToolStore::contains<IGRLTool>(grl_tool_name)) {
    m_grlTool = asg::ToolStore::get<IGRLTool>(grl_tool_name);
  } else {
    if (!m_config->grlFile().size()) {
      // do not configure GRL tool
      ATH_MSG_WARNING("No GRL files provided -> Will NOT setup GoodRunsListSelectionTool.");
      return StatusCode::SUCCESS;
    }
    
    IGRLTool* grlTool =  new GoodRunsListSelectionTool(grl_tool_name);
    std::vector<std::string> names = {};
    std::string grlDir = m_config->grlDir();
    if (grlDir.size()) grlDir += "/";
    for (const std::string& grl : m_config->grlFile()) {
      names.push_back(PathResolverFindCalibFile(grlDir + grl));
    }
    
    top::check(asg::setProperty(grlTool, "GoodRunsListVec", names),
                "GRLSelector failed to set GRL");
    top::check( grlTool->initialize() ,
                "Failed to initialize GRLSelectionTool" );
    m_grlTool = grlTool;
  }
  return StatusCode::SUCCESS;
}

StatusCode OtherCPTools::setupPileupReweighting() {
  const std::string prw_tool_name = "CP::PileupReweightingTool";
  if (asg::ToolStore::contains<CP::IPileupReweightingTool>(prw_tool_name)) {
    m_pileupReweightingTool = asg::ToolStore::get<CP::IPileupReweightingTool>(prw_tool_name);
  } else {
    CP::IPileupReweightingTool* pileupReweightingTool
      = new CP::PileupReweightingTool(prw_tool_name);

    std::vector<std::string> pileup_config = m_config->PileupConfig();
    for (std::string& s : pileup_config)
      s = PathResolverFindCalibFile(s);
    
    std::vector<std::string> pileup_lumi_calc = m_config->PileupLumiCalc();
    for (std::string& s : pileup_lumi_calc)
      s = PathResolverFindCalibFile(s);

    std::vector<std::string> pileup_config_FS = m_config->PileupConfig_FS();
    for (std::string& s : pileup_config_FS)
      s = PathResolverFindCalibFile(s);

    std::vector<std::string> pileup_config_AF = m_config->PileupConfig_AF();
    for (std::string& s : pileup_config_AF)
      s = PathResolverFindCalibFile(s);

    std::vector<std::string> actual_mu_FS = m_config->PileupActualMu_FS();
    for (std::string& s : actual_mu_FS)
      s = PathResolverFindCalibFile(s);

    std::vector<std::string> actual_mu_AF = m_config->PileupActualMu_AF();
    for (std::string& s : actual_mu_AF)
      s = PathResolverFindCalibFile(s);

    // New checks - If FS or AF size != 0, then the general option should be empty
    if( (pileup_config_AF.size() > 0 || pileup_config_FS.size() > 0) && (pileup_config.size() > 0) ){
      ATH_MSG_ERROR("You have used PRWConfigFiles as well as PRWConfigFiles_FS and/or PRWConfigFiles_AF");
      ATH_MSG_ERROR("We do not know how to configure with all these options");
      return StatusCode::FAILURE;
    }    

    if (m_config->PileupUseGRLTool())
      top::check(asg::setProperty(pileupReweightingTool, "GRLTool", m_grlTool),
                "Failed to give GRLtool to pileup reweighting tool");

    // Config file is not needed on Data
    // see "Just random run numbers and lumi corrections"
    // case in twiki page below
    // However, the tool would spit out warnings for Data if we didn't supply ConfigFiles.   
    if(pileup_config.size() > 0){
      ATH_MSG_INFO("PRW tool is being configured without any FS/AF information");
      top::check(asg::setProperty(pileupReweightingTool, "ConfigFiles", pileup_config),
		 "Failed to set pileup reweighting config files");
    }
    else if(m_config->isAFII() && pileup_config_AF.size() > 0){
      // concatenate the config and the actual mu files
      std::vector<std::string> final_config_AF = pileup_config_AF;
      final_config_AF.insert(final_config_AF.end(), actual_mu_AF.begin(), actual_mu_AF.end());
      ATH_MSG_INFO("This sample is fast sim");
      ATH_MSG_INFO("PRW tool is being configured only with fast simulation (AF) config files");
      top::check(asg::setProperty(pileupReweightingTool, "ConfigFiles", final_config_AF),
                 "Failed to set pileup reweighting config files");
    }
    else if(!m_config->isAFII() && pileup_config_FS.size() > 0){
      // concatenate the config and the actual mu files
      std::vector<std::string> final_config_FS = pileup_config_FS;
      final_config_FS.insert(final_config_FS.end(), actual_mu_FS.begin(), actual_mu_FS.end());
      ATH_MSG_INFO("This sample is full sim");
      ATH_MSG_INFO("PRW tool is being configured only with full simulation (FS) config files");
      top::check(asg::setProperty(pileupReweightingTool, "ConfigFiles", final_config_FS),
                 "Failed to set pileup reweighting config files");
    }
    else{
      ATH_MSG_ERROR("There are not any PRW config files provided with any allowed options");
      return StatusCode::FAILURE;
    }
    // data scale-factors, initialised to recommended values
    // can also be customised, thanks to PRWCustomScaleFactor option
    double SF_nominal = 1.0/1.03;
    double SF_up = 1.0/0.99;
    double SF_down = 1.0/1.07;

    // if custom data SFs
    if (m_config->PileUpCustomScaleFactors().size()!=0) {
      SF_nominal = m_config->PileUpCustomScaleFactors()[0];
      SF_up = m_config->PileUpCustomScaleFactors()[1];
      SF_down = m_config->PileUpCustomScaleFactors()[2];
      ATH_MSG_INFO("Using custom Data Scale-Factors for pile-up reweighting");
      std::ostringstream oss;
      oss << "Nominal:" << SF_nominal << " up:"<< SF_up << " down:" << SF_down  << std::endl;
      ATH_MSG_INFO(oss.str());
    }

    top::check(asg::setProperty(pileupReweightingTool, "LumiCalcFiles", pileup_lumi_calc),
              "Failed to set pileup reweighting lumicalc files");
    // see [http://cern.ch/go/hx7d]
    top::check(asg::setProperty(pileupReweightingTool, "DataScaleFactor", static_cast<Float_t>(SF_nominal)),
              "Failed to set pileup reweighting data scale factor");
    top::check(asg::setProperty(pileupReweightingTool, "DataScaleFactorUP", SF_up),
              "Failed to set pileup reweighting data scale factor up");
    top::check(asg::setProperty(pileupReweightingTool, "DataScaleFactorDOWN", SF_down),
              "Failed to set pileup reweighting data scale factor down");
    // Set the unrepresented data tolerence (default is 5% same as the PRW tool)
    top::check(asg::setProperty(pileupReweightingTool, "UnrepresentedDataThreshold", m_config->PileupDataTolerance()),
	       "Failed to set pileup reweighting data tolerance");
    if (m_config->PileupPeriodAssignments().size() > 0){
      // Set the period assignments associated with different running periods
      top::check(asg::setProperty(pileupReweightingTool, "PeriodAssignments", m_config->PileupPeriodAssignments()),
	         "Failed to set pileup reweighting period assignments");
    }
    top::check(pileupReweightingTool->initialize(),
              "Failed to initialize pileup reweighting tool");

    m_pileupReweightingTool = pileupReweightingTool;
  }
  return StatusCode::SUCCESS;
}


StatusCode OtherCPTools::setupPMGTools() {
  // Setup any PMG tools

  // Sherpa 2.2 V+jets samples based on the number of truth jets
  const std::string name = "PMGSherpa22VJetsWeightTool";
  PMGTools::PMGSherpa22VJetsWeightTool* tool = nullptr;
  if (asg::ToolStore::contains<PMGTools::PMGSherpa22VJetsWeightTool>(name)) {
    tool = asg::ToolStore::get<PMGTools::PMGSherpa22VJetsWeightTool>(name);
  } else {
    tool = new PMGTools::PMGSherpa22VJetsWeightTool(name);
    top::check(asg::setProperty(tool, "TruthJetContainer",
                                m_config->sgKeyTruthJets()),
                "Failed to set TruthJetContainer to " + name);
    top::check(tool->initialize(), "Failed to initialize " + name);
  }
  m_pmg_sherpa22_vjets_tool = tool;

  // Tool for access truth weights via names
  const std::string truthWeightToolName = "PMGTruthWeightTool";
  PMGTools::PMGTruthWeightTool* truthweightTool = nullptr;
  if(asg::ToolStore::contains<PMGTools::PMGTruthWeightTool>(truthWeightToolName)) {
    truthweightTool = asg::ToolStore::get<PMGTools::PMGTruthWeightTool>(truthWeightToolName);
  } else {
    truthweightTool = new PMGTools::PMGTruthWeightTool(truthWeightToolName);
    top::check(truthweightTool->initialize(), "Failed to initialize "+truthWeightToolName);
  }
  m_pmg_weightTool = truthweightTool; 

  return StatusCode::SUCCESS;
}

}  // namespace top
