/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////
// 
// Author   : 
//              Timothée Theveneaux-Pelzer
// Creation : 
// Purpose  : helper to be used in analyses
//
// Updated  : 
//           2016-01-29 : FD: set default values for Run2 and 1st efficiencies
//////////////////////////////////////////////////////////////////////

#include "TopFakes/TopFakesMMWeightCalculator.h"
#include "TopFakes/TopFakesxAODUtils.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/ConfigurationSettings.h"
// #include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopEvent/TopEventMaker.h"
#include "TopFakes/FakesWeights.h"
#include "TopFakes/MMEfficiency.h"
#include "TopFakes/MatrixUtils.h"
#include "TopFakes/MMEffSet.h"
#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl; 

namespace top{
  
  TopFakesMMWeightCalculator::TopFakesMMWeightCalculator( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr),
    m_dir(""),
    m_debug(false)//,
//     m_leptonDefs(fakes::LeptonDefinitions::Standard)
  {
    ATH_MSG_INFO(" constructor of top::TopFakesMMWeightCalculator "+name );
    declareProperty( "config" , m_config );
  }
  
  TopFakesMMWeightCalculator::~TopFakesMMWeightCalculator()
  {
  }

  // =============================================================
  StatusCode TopFakesMMWeightCalculator::initialize()
  {
    ATH_MSG_INFO(" top::TopFakesMMWeightCalculator initialize" );
    
    m_dir = m_config->FakesMMDir();
    m_debug = m_config->FakesMMDebug();
    ATH_MSG_INFO(" Directory for efficiencies: "+m_dir );
    if (std::find(m_config->bTagWP_available().begin(), m_config->bTagWP_available().end(), "FixedCutBEff_77") == m_config->bTagWP_available().end()) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator initialize" );
      std::cout<<"B-tagging WP  FixedCutBEff_77 is needed to parametrise efficiencies."<<std::endl;
      std::cout<<"Please set it up by writing \"BTaggingWP FixedCutBEff_77\" or \"BTaggingWP %77\" in your configuration file."<<std::endl;
      return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode TopFakesMMWeightCalculator::setSelectionConfigs(std::string selection, std::vector<std::string> configs)
  {
    ATH_MSG_INFO(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
    
    auto* const settings = top::ConfigurationSettings::get();
    const std::vector<top::SelectionConfigurationData> selections = settings->selections();
    for(std::string config : configs){
      std::istringstream ss(config);
      std::vector<std::string> listofkeys;
      std::string token;
      while(std::getline(ss, token, ',')) {//parameters are separated by commas
	listofkeys.push_back(token);
      }
      // check if number of parameters is ok
      if (listofkeys.size()<3 || listofkeys.size()>7) {//need at least 3 and no more than 7 parameters
	ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	std::cout<<"The following configuration is ill-defined:"<<std::endl;
	std::cout<<config<<std::endl;
	return StatusCode::FAILURE;
      }

      // check if configuration already defined for another selection
      if (m_fakesWeightTools.count(selection) !=0) {
	if (m_fakesWeightTools[selection].count(listofkeys.at(0)) !=0) {
	  ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	  std::cout<<"The configuration "<<listofkeys.at(0)<<"is already defined for selection "<<selection<<std::endl;
	  std::cout<<"Exiting.... "<<std::endl;
	  return StatusCode::FAILURE;
	}
      }

      // first argument must define the period - check if it's valid
      int period = getPeriod(listofkeys.at(1));
      if (period == -1) {
        ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
        std::cout<<"Can't recognise period "<<listofkeys.at(1)<<std::endl;
        std::cout<<"Exiting.... "<<std::endl;
      }
      
      // second argument is the channel
      if (listofkeys.at(2)=="EJETS") {
	if (listofkeys.size()<3 || listofkeys.size()>5) {
	  ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	  std::cout<<"The following "<<listofkeys.at(2)<<" configuration is ill-defined:"<<std::endl;
	  std::cout<<config<<std::endl;
	  std::cout<<"Correct syntax (1): <config name>,EJETS"<<std::endl;
	  std::cout<<"Correct syntax (2): <config name>,EJETS,<systematic variation>"<<std::endl;
	  std::cout<<"Correct syntax (3): <config name>,EJETS,<real eff configuration>,<fake eff configuration>"<<std::endl;
	  std::cout<<"Exiting.... "<<std::endl;
	  return StatusCode::FAILURE;
	}
	else {
	  m_fakesWeightTools[selection][listofkeys.at(0)] = std::shared_ptr<FakesWeights>(new FakesWeights());
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDebug(m_debug);
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDataPath(m_dir);
          m_fakesWeightTools[selection][listofkeys.at(0)]->SetPeriod(period);
	  if (listofkeys.size()==3) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::EJETS);
	  }
	  else if (listofkeys.size()==4) {
	    if (listofkeys.at(3)!="MCup" && listofkeys.at(3)!="MCdown" && listofkeys.at(3)!="MCupel" && listofkeys.at(3)!="MCdownel" && listofkeys.at(3)!="CRfake" && listofkeys.at(3)!="CRfakeel" && listofkeys.at(3)!="CRreal" && listofkeys.at(3)!="CRrealel" && listofkeys.at(3)!="EffPar") {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	      std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	      std::cout<<config<<std::endl;
	      std::cout<<"Systematic variation "<<listofkeys.at(2)<<" not recognised."<<std::endl;
	      std::cout<<"Exiting.... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	    else {
	      m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::EJETS,listofkeys.at(3));
	    }
	  }
	  else if (listofkeys.size()==5) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighter(FakesWeights::EJETS,listofkeys.at(3),listofkeys.at(4));
	  }
	}
      }
      else if (listofkeys.at(2)=="MUJETS") {
	if (listofkeys.size()<3 || listofkeys.size()>5) {
	  ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	  std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	  std::cout<<config<<std::endl;
	  std::cout<<"Correct syntax (1): <config name>,MUJETS"<<std::endl;
	  std::cout<<"Correct syntax (2): <config name>,MUJETS,<systematic variation>"<<std::endl;
	  std::cout<<"Correct syntax (3): <config name>,MUJETS,<real eff configuration>,<fake eff configuration>"<<std::endl;
	  std::cout<<"Exiting.... "<<std::endl;
	  return StatusCode::FAILURE;
	}
	else {
	  m_fakesWeightTools[selection][listofkeys.at(0)] = std::shared_ptr<FakesWeights>(new FakesWeights());
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDebug(m_debug);
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDataPath(m_dir);
          m_fakesWeightTools[selection][listofkeys.at(0)]->SetPeriod(period);
	  if (listofkeys.size()==3) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::MUJETS);
	  }
	  else if (listofkeys.size()==4) {
	    if (listofkeys.at(3)!="MCup" && listofkeys.at(3)!="MCdown" && listofkeys.at(3)!="MCupmu" && listofkeys.at(3)!="MCdownmu" && listofkeys.at(3)!="CRfake" && listofkeys.at(3)!="CRfakemu" && listofkeys.at(3)!="CRreal" && listofkeys.at(3)!="CRrealmu" && listofkeys.at(3)!="EffPar") {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	      std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	      std::cout<<config<<std::endl;
	      std::cout<<"Systematic variation "<<listofkeys.at(3)<<" not recognised."<<std::endl;
	      std::cout<<"Exiting.... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	    else {
              m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::MUJETS,listofkeys.at(3));
	    }
	  }
	  else if (listofkeys.size()==5) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighter(FakesWeights::MUJETS,listofkeys.at(3),listofkeys.at(4));
	  }
	}
      }
      else if (listofkeys.at(2)=="EE") {
	if (listofkeys.size()<3 || listofkeys.size()>5) {
	  ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	  std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	  std::cout<<config<<std::endl;
	  std::cout<<"Correct syntax (1): <config name>,EE"<<std::endl;
	  std::cout<<"Correct syntax (2): <config name>,EE,<systematic variation>"<<std::endl;
	  std::cout<<"Correct syntax (3): <config name>,EE,<real eff configuration>,<fake eff configuration>"<<std::endl;
	  std::cout<<"Exiting.... "<<std::endl;
	  return StatusCode::FAILURE;
	}
	else {
	  m_fakesWeightTools[selection][listofkeys.at(0)] = std::shared_ptr<FakesWeights>(new FakesWeights());
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDebug(m_debug);
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDataPath(m_dir);
          m_fakesWeightTools[selection][listofkeys.at(0)]->SetPeriod(period);
	  if (listofkeys.size()==3) {
            m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::EE);
	  }
	  else if (listofkeys.size()==4) {
	    if (listofkeys.at(3)!="MCup" && listofkeys.at(3)!="MCdown" && listofkeys.at(3)!="MCupel" && listofkeys.at(3)!="MCdownel" && listofkeys.at(3)!="CRfake" && listofkeys.at(3)!="CRfakeel" && listofkeys.at(3)!="CRreal" && listofkeys.at(3)!="CRrealel" && listofkeys.at(3)!="EffPar") {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	      std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	      std::cout<<config<<std::endl;
	      std::cout<<"Systematic variation "<<listofkeys.at(3)<<" not recognised."<<std::endl;
	      std::cout<<"Exiting.... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	    else {
              m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::EE,listofkeys.at(3));
	    }
	  }
	  else if (listofkeys.size()==4) {
            m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighter(FakesWeights::EE,listofkeys.at(3),listofkeys.at(4));
	  }
	}
      }
      else if (listofkeys.at(2)=="EMU") {
	if (listofkeys.size()!=3 && listofkeys.size()!=4 && listofkeys.size()!=7) {
	  ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	  std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	  std::cout<<config<<std::endl;
	  std::cout<<"Correct syntax (1): <config name>,EMU"<<std::endl;
	  std::cout<<"Correct syntax (2): <config name>,EMU,<systematic variation>"<<std::endl;
	  std::cout<<"Correct syntax (3): <config name>,EMU,<electron real eff configuration>,<electron fake eff configuration>,<muon real eff configuration>,<muon fake eff configuration>"<<std::endl;
	  std::cout<<"Exiting.... "<<std::endl;
	  return StatusCode::FAILURE;
	}
	else {
	  m_fakesWeightTools[selection][listofkeys.at(0)] = std::shared_ptr<FakesWeights>(new FakesWeights());
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDebug(m_debug);
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDataPath(m_dir);
          m_fakesWeightTools[selection][listofkeys.at(0)]->SetPeriod(period);
	  if (listofkeys.size()==3) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::EMU);
	  }
	  else if (listofkeys.size()==4) {
	    if (listofkeys.at(3)!="MCup" && listofkeys.at(3)!="MCdown" && listofkeys.at(3)!="MCupel" && listofkeys.at(3)!="MCdownel" && listofkeys.at(3)!="MCupmu" && listofkeys.at(3)!="MCdownmu" && listofkeys.at(3)!="CRfake" && listofkeys.at(3)!="CRfakeel" && listofkeys.at(3)!="CRfakemu" && listofkeys.at(3)!="CRreal" && listofkeys.at(3)!="CRrealel" && listofkeys.at(3)!="CRrealmu" && listofkeys.at(3)!="EffPar") {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	      std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	      std::cout<<config<<std::endl;
	      std::cout<<"Systematic variation "<<listofkeys.at(3)<<" not recognised."<<std::endl;
	      std::cout<<"Exiting.... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	    else {
	      m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::EMU,listofkeys.at(3));
	    }
	  }
	  else if (listofkeys.size()==7) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighter(FakesWeights::EMU,listofkeys.at(3),listofkeys.at(4),listofkeys.at(5),listofkeys.at(6));
	  }
	}
      }
      else if (listofkeys.at(2)=="MUMU") {
	if (listofkeys.size()<3 || listofkeys.size()>5) {
	  ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	  std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	  std::cout<<config<<std::endl;
	  std::cout<<"Correct syntax (1): <config name>,MUMU"<<std::endl;
	  std::cout<<"Correct syntax (2): <config name>,MUMU,<systematic variation>"<<std::endl;
	  std::cout<<"Correct syntax (3): <config name>,MUMU,<real eff configuration>,<fake eff configuration>"<<std::endl;
	  std::cout<<"Exiting.... "<<std::endl;
	  return StatusCode::FAILURE;
	}
	else {
	  m_fakesWeightTools[selection][listofkeys.at(0)] = std::shared_ptr<FakesWeights>(new FakesWeights());
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetDebug(m_debug);
          m_fakesWeightTools[selection][listofkeys.at(0)]->SetDataPath(m_dir);
	  m_fakesWeightTools[selection][listofkeys.at(0)]->SetPeriod(period);
	  if (listofkeys.size()==3) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::MUMU);
	  }
	  else if (listofkeys.size()==4) {
	    if (listofkeys.at(3)!="MCup" && listofkeys.at(3)!="MCdown" && listofkeys.at(3)!="MCupmu" && listofkeys.at(3)!="MCdownmu" && listofkeys.at(3)!="CRfake" && listofkeys.at(3)!="CRfakemu" && listofkeys.at(3)!="CRreal" && listofkeys.at(3)!="CRrealmu" && listofkeys.at(3)!="EffPar") {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	      std::cout<<"The following "<<listofkeys.at(1)<<" configuration is ill-defined:"<<std::endl;
	      std::cout<<config<<std::endl;
	      std::cout<<"Systematic variation "<<listofkeys.at(3)<<" not recognised."<<std::endl;
	      std::cout<<"Exiting.... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	    else {
	      m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighterDefault(FakesWeights::MUMU,listofkeys.at(3));
	    }
	  }
	  else if (listofkeys.size()==5) {
	    m_fakesWeightTools[selection][listofkeys.at(0)]->SetupWeighter(FakesWeights::MUMU,listofkeys.at(3),listofkeys.at(4));
	  }
	}
      }
      else {
	ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator setSelectionConfigs" );
	std::cout<<"Unknown configuration type "<<listofkeys.at(1)<<" of configuration "<<listofkeys.at(0)<<" for selection "<<selection<<std::endl;
	return StatusCode::FAILURE;
      }
    }
    return StatusCode::SUCCESS;
  }

  // ===========================================================  
  StatusCode TopFakesMMWeightCalculator::execute(top::Event* topEvent)
  {
    const xAOD::EventInfo* event_info = topEvent->m_info;
    const xAOD::ElectronContainer* electrons = &topEvent->m_electrons;
    const xAOD::MuonContainer* muons = &topEvent->m_muons;
    const xAOD::JetContainer* jets = &topEvent->m_jets;
    const xAOD::MissingET* MET = topEvent->m_met;
    
    if (event_info->eventType(xAOD::EventInfo::IS_SIMULATION)) {// no MM weight to be calculated for MC events
      return StatusCode::SUCCESS;
    }
    else {
      auto* const settings = top::ConfigurationSettings::get();
      const std::vector<top::SelectionConfigurationData> selections = settings->selections();
      for(auto scd : selections){//loop on selections
	bool pass = false;
	if (event_info->isAvailable<int>(scd.m_name))
	  pass = event_info->auxdataConst<int>(scd.m_name);
	if (!pass)
	  continue;// if the selection is not passed, don't decorate the event with MM weight
	for(auto configs : m_fakesWeightTools[scd.m_name]) {// loop on the configurations associated to this selection
	  std::string config_name = configs.first;
	  std::string decorName = "MMWeight_"; decorName += scd.m_name; decorName += "_"; decorName += config_name;
	  if (event_info->isAvailable<float>(decorName)) {
	    ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	    std::cout<<"Current event already decorated with "<<decorName<<std::endl;
	    std::cout<<"Don't want to re-calculate it. Exiting... "<<std::endl;
	    return StatusCode::FAILURE;
	  }
	  float mmweight = -999.;
	  auto weighter = m_fakesWeightTools[scd.m_name][config_name];
	  if (weighter==nullptr) {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	      std::cout<<"Weighter is not defined for "<<scd.m_name<<" "<<config_name<<std::endl;
	      std::cout<<"Exiting... "<<std::endl;
	      return StatusCode::FAILURE;
	  }
	  if (m_debug) {
	    std::cout<<"=====================================>Selection: "<<scd.m_name<<" Configuration: "<<config_name<<std::endl;
	  }
	  if (weighter->Channel()==FakesWeights::EJETS) {
	    try {
	      mmweight = getMMweightE(weighter, *MET, *electrons, *jets, event_info);
	    }
	    catch(std::exception& e)  {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	      std::cout<<"Could not calculate EJETS MM weight for "<<decorName<<std::endl;
	      std::cout<<"Exiting... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	  }
	  else if (weighter->Channel()==FakesWeights::MUJETS) {
	    try {
	      mmweight = getMMweightM(weighter, *MET, *muons, *jets, event_info);
	    }
	    catch(std::exception& e)  {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	      std::cout<<"Could not calculate MUJETS weight for "<<decorName<<std::endl;
	      std::cout<<"Exiting... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	  }
	  else if (weighter->Channel()==FakesWeights::EE) {
	    try {
	      mmweight = getMMweightEE(weighter, *MET, *electrons, *jets, event_info);
	    }
	    catch(std::exception& e)  {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	      std::cout<<"Could not calculate EE weight for "<<decorName<<std::endl;
	      std::cout<<"Exiting... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	  }
	  else if (weighter->Channel()==FakesWeights::MUMU) {
	    try {
	      mmweight = getMMweightMM(weighter, *MET, *muons, *jets, event_info);
	    }
	    catch(std::exception& e)  {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	      std::cout<<"Could not calculate MUMU weight for "<<decorName<<std::endl;
	      std::cout<<"Exiting... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	  }
	  else if (weighter->Channel()==FakesWeights::EMU) {
	    try {
	      mmweight = getMMweightEM(weighter, *MET, *electrons, *muons, *jets, event_info);
	    }
	    catch(std::exception& e)  {
	      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	      std::cout<<"Could not calculate EMU weight for "<<decorName<<std::endl;
	      std::cout<<"Exiting... "<<std::endl;
	      return StatusCode::FAILURE;
	    }
	  }
	  else {
	    ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator execute" );
	    std::cout<<"Unable to retrieve the channel of FakesWeights object while calculating weight "<<decorName<<std::endl;
	    std::cout<<"Exiting... "<<std::endl;
	    return StatusCode::FAILURE;
	  }
	  event_info->auxdecor<float>(decorName) = mmweight;
	}
      }
    }
    return StatusCode::SUCCESS;
  }

  // ==================================================================
  float TopFakesMMWeightCalculator::getMMweightE(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::ElectronContainer& els, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info)
  {
    //
    // Compute weight for e+jets channel
    //
    float mmWeightE = 0;
    
    /// not used for the moment
//     std::vector<unsigned int> good_els_index = fakes::GetGoodHighPtLeptons(els);
    
    if (els.size()<1) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator::getMMweightE" );
      throw std::runtime_error("Need at least one electron to calculate EJETS matrix-method weight");
    }
    
    // if more than 1 electron, just take the leading one (i.e. the first in the container)
    MMLepton *lept = getLeptonMMInfo(*els[0], jets, met, event_info);
    MMEvent  *evt  = getEventMMInfo(jets, met);
    
    if (lept==0 || evt==0) {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator::getMMweightE" );
      ATH_MSG_INFO(" Unable to define MMLepton or MMEvent object" );
      mmWeightE = -999.;
    }
    else {
      weighter->SetLepton(*evt,*lept);
      mmWeightE = weighter->GetFakesWeightLJets(els[0]->auxdataConst<char>("passPreORSelection"));
    }
  
    delete lept;
    delete evt;
    
    if (m_debug) {
      std::cout<<"==============>mmWeightE="<<mmWeightE<<std::endl;
      std::cout<<"====>r1="<<weighter->GetRealEff()<<" f1="<<weighter->GetFakeEff()<<std::endl;
    }
    
    return mmWeightE;
  }
  
  // ==================================================================
  float TopFakesMMWeightCalculator::getMMweightM(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::MuonContainer& mus, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info)
  {
    //
    // Compute weight for m+jets channel
    //
    float mmWeightM = 0;
    
    /// not used for the moment
//     std::vector<unsigned int> good_mus_index = fakes::GetGoodHighPtLeptons(mus);
    
    if (mus.size()<1) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator::getMMweightM" );
      throw std::runtime_error("Need at least one muon to calculate MUJETS matrix-method weight");
    }
    
    // if more than 1 muon, just take the leading one (i.e. the first in the container)
    MMLepton *lept = getLeptonMMInfo(*mus[0], jets, met, event_info);
    MMEvent  *evt  = getEventMMInfo(jets, met);
  
    if (lept==0 || evt==0) {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator::getMMweightM" );
      ATH_MSG_INFO(" Unable to define MMLepton or MMEvent object" );
      mmWeightM = -999.;
    }
    else {
      weighter->SetLepton(*evt,*lept);
      mmWeightM = weighter->GetFakesWeightLJets(mus[0]->auxdataConst<char>("passPreORSelection"));
    }
  
    delete lept;
    delete evt;
    
    if (m_debug) {
      std::cout<<"==============>mmWeightM="<<mmWeightM<<std::endl;
      std::cout<<"====>r1="<<weighter->GetRealEff()<<" f1="<<weighter->GetFakeEff()<<std::endl;
    }
    
    return mmWeightM;
  }
  
  // ==================================================================
  float TopFakesMMWeightCalculator::getMMweightEE(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::ElectronContainer& els, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info)
  {
    //
    // Compute weight for ee channel
    //
    float mmWeightEE = 0;
    
    /// not used for the moment
//     std::vector<unsigned int> good_els_index = fakes::GetGoodHighPtLeptons(els);
    
    if (els.size()<2) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator::getMMweightEE" );
      throw std::runtime_error("Need at least two electrons to calculate EE matrix-method weight");
    }
    
    // if more than 2 electrons, just take the two leading ones (i.e. the first in the container)
    MMLepton *lept_e1 = getLeptonMMInfo(*els[0], jets, met, event_info);
    MMLepton *lept_e2 = getLeptonMMInfo(*els[1], jets, met, event_info);
    MMEvent  *evt  = getEventMMInfo(jets, met);
    
    if (lept_e1==0 || lept_e2==0 || evt==0) {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator::getMMweightEE" );
      ATH_MSG_INFO(" Unable to define MMLepton or MMEvent object" );
      mmWeightEE = 0.;
    }
    else {
      weighter->SetLeptons(*evt,*lept_e1,*lept_e2);
      mmWeightEE = weighter->GetFakesWeightDilep(els[0]->auxdataConst<char>("passPreORSelection"),els[1]->auxdataConst<char>("passPreORSelection"));
    }
    
    delete lept_e1;
    delete lept_e2;
    delete evt;
    
    if (m_debug) {
      std::cout<<"==============>mmWeightEE="<<mmWeightEE<<std::endl;
      std::cout<<"====>r1="<<weighter->GetRealEff()<<" f1="<<weighter->GetFakeEff()<<" r2="<<weighter->GetRealEff2()<<" f2="<<weighter->GetFakeEff2()<<std::endl;
    }
    
    return mmWeightEE;
  }
  
  // ==================================================================
  float TopFakesMMWeightCalculator::getMMweightMM(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::MuonContainer& mus, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info)
  {
    //
    // Compute weight for mm channel
    //
    float mmWeightMM = 0;
    
    /// not used for the moment
//     std::vector<unsigned int> good_mus_index = fakes::GetGoodHighPtLeptons(mus);
    
    if (mus.size()<2) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator::getMMweightMM" );
      throw std::runtime_error("Need at least two muons to calculate MUMU matrix-method weight");
    }
    
    // if more than 2 muons, just take the two leading ones (i.e. the first in the container)
    MMLepton *lept_m1 = getLeptonMMInfo(*mus[0], jets, met, event_info);
    MMLepton *lept_m2 = getLeptonMMInfo(*mus[1], jets, met, event_info);
    MMEvent  *evt  = getEventMMInfo(jets, met);
    
    if (lept_m1==0 || lept_m2==0 || evt==0) {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator::getMMweightMM" );
      ATH_MSG_INFO(" Unable to define MMLepton or MMEvent object" );
      mmWeightMM = 0.;
    }
    else {
      weighter->SetLeptons(*evt,*lept_m1,*lept_m2);
      mmWeightMM = weighter->GetFakesWeightDilep(mus[0]->auxdataConst<char>("passPreORSelection"),mus[1]->auxdataConst<char>("passPreORSelection"));
    }
    
    delete lept_m1;
    delete lept_m2;
    delete evt;
    
    if (m_debug) {
      std::cout<<"==============>mmWeightMM="<<mmWeightMM<<std::endl;
      std::cout<<"====>r1="<<weighter->GetRealEff()<<" f1="<<weighter->GetFakeEff()<<" r2="<<weighter->GetRealEff2()<<" f2="<<weighter->GetFakeEff2()<<std::endl;
    }
    
    return mmWeightMM;
  }
  
  // ==================================================================
  float TopFakesMMWeightCalculator::getMMweightEM(std::shared_ptr<FakesWeights> weighter, const xAOD::MissingET& met, const xAOD::ElectronContainer& els, const xAOD::MuonContainer& mus, const xAOD::JetContainer jets, const xAOD::EventInfo* event_info)
  {
    //
    // Compute weight for em channel
    //
    float mmWeightEM = 0;
    
    /// not used for the moment
//     std::vector<unsigned int> good_els_index = fakes::GetGoodHighPtLeptons(els);
//     std::vector<unsigned int> good_mus_index = fakes::GetGoodHighPtLeptons(mus);
    
    if (els.size()<1) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator::getMMweightEM" );
      throw std::runtime_error("Need at least one electron to calculate EMU matrix-method weight");
    }
    if (mus.size()<1) {
      ATH_MSG_ERROR(" top::TopFakesMMWeightCalculator::getMMweightEM" );
      throw std::runtime_error("Need at least one muon to calculate EMU matrix-method weight");
    }
    
    // if more than 1 electron or 1 muon, just take the two leading ones (i.e. the first in the container)
    MMLepton *lept_e = getLeptonMMInfo(*els[0], jets, met, event_info);
    MMLepton *lept_m = getLeptonMMInfo(*mus[0], jets, met, event_info);
    MMEvent  *evt  = getEventMMInfo(jets, met);
    
    if (lept_e==0 || lept_m==0 || evt==0) {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator::getMMweightEMU" );
      ATH_MSG_INFO(" Unable to define MMLepton or MMEvent object" );
      mmWeightEM = 0.;
    }
    else {
      weighter->SetLeptons(*evt,*lept_e,*lept_m);
      mmWeightEM = weighter->GetFakesWeightDilep(els[0]->auxdataConst<char>("passPreORSelection"),mus[0]->auxdataConst<char>("passPreORSelection"));
    }
    
    delete lept_e;
    delete lept_m;
    delete evt;
    
    if (m_debug) {
      std::cout<<"==============>mmWeightEM="<<mmWeightEM<<std::endl;
      std::cout<<"====>r1="<<weighter->GetRealEff()<<" f1="<<weighter->GetFakeEff()<<" r2="<<weighter->GetRealEff2()<<" f2="<<weighter->GetFakeEff2()<<std::endl;
    }
    
    return mmWeightEM;
  }

  // ===============================================================
  MMLepton* TopFakesMMWeightCalculator::getLeptonMMInfo(const xAOD::Electron& el, const xAOD::JetContainer& jets, const xAOD::MissingET& met, const xAOD::EventInfo* event_info)
  {
    float eta_cl = el.caloCluster()->etaBE(2);
//     float eta = el.eta();
//     float phi = el.phi();
    float pt  = el.pt()/1.e3; // tool needs GeV
    unsigned int runNumber = event_info -> runNumber();
    if (m_config->isMC() && m_config->doPileupReweighting()) {
      if (event_info->isAvailable<unsigned int>("RandomRunNumber"))
        runNumber = event_info->auxdataConst<unsigned int>("RandomRunNumber");
    }
    int trigger = 0.;
    top::fakes::GetTrigMatchVariable(el, trigger, runNumber);

    const xAOD::TrackParticle* eltrack = el.trackParticle();
    float d0 = eltrack->d0();
    const xAOD::ParametersCovMatrix_t elcov = eltrack->definingParametersCovMatrix();
    float d0signif = d0/sqrt(elcov(0,0));
    
    float dR;
    float pTj;
    float dRCentral;
    float pTjCentral;
    float dRpt;
    float dRptCentral;
    fakes::GetClosestJet (el, jets, dR, pTj, dRpt, dRCentral, pTjCentral, dRptCentral);
    
    float dphilmet;
    fakes::GetdPhiLepMET (el, met, dphilmet);
    
    MMLepton* lept = new MMLepton();
    lept->pt      = pt;
    lept->eta     = std::fabs(eta_cl);
    lept->dR      = dR;
    lept->dPhi    = dphilmet;
    lept->trigger = trigger;
    lept->d0sig  = d0signif;

    if (m_debug) {
      std::cout<<"Electron";
      std::cout<<" pt="<<pt;
      std::cout<<" eta="<<std::fabs(eta_cl);
      std::cout<<" dR="<<dR;
      std::cout<<" dPhi="<<dphilmet;
      std::cout<<" trigger="<<trigger;
      std::cout<<" d0sig="<<d0signif;
      std::cout<<std::endl;
    }
  
    return lept;
  }

  // ===========================================================
  MMLepton* TopFakesMMWeightCalculator::getLeptonMMInfo(const xAOD::Muon& mu, const xAOD::JetContainer& jets, const xAOD::MissingET& met, const xAOD::EventInfo* event_info)
  {
    float eta = mu.eta();
//     float phi = mu.phi();
    float pt  = mu.pt()/1.e3; // tool needs GeV
    unsigned int runNumber = event_info -> runNumber();
    if (m_config->isMC() && m_config->doPileupReweighting()) {
      if (event_info->isAvailable<unsigned int>("RandomRunNumber"))
        runNumber = event_info->auxdataConst<unsigned int>("RandomRunNumber");
    }
    int trigger = 0.;
    top::fakes::GetTrigMatchVariable(mu, trigger, runNumber);

    const xAOD::TrackParticle* mutrack = mu.primaryTrackParticle();
    float d0 = mutrack->d0();
    const xAOD::ParametersCovMatrix_t mucov=mutrack->definingParametersCovMatrix();
    float d0signif = d0/sqrt(mucov(0,0));
    
    float dR;
    float pTj;
    float dRCentral;
    float pTjCentral;
    float dRpt;
    float dRptCentral;
    fakes::GetClosestJet (mu, jets, dR, pTj, dRpt, dRCentral, pTjCentral, dRptCentral);
   
    float dphilmet;
    fakes::GetdPhiLepMET (mu, met, dphilmet);
  
    MMLepton* lept = new MMLepton();
    lept->pt      = pt;
    lept->eta     = std::fabs(eta);
    lept->dR      = dR;
    lept->dPhi    = dphilmet;
    lept->trigger = trigger;
    lept->d0sig  = d0signif;

    if (m_debug) {
      std::cout<<"Muon";
      std::cout<<" pt="<<pt;
      std::cout<<" eta="<<std::fabs(eta);
      std::cout<<" dR="<<dR;
      std::cout<<" dRpt="<<dRpt;
      std::cout<<" dPhi="<<dphilmet;
      std::cout<<" trigger="<<trigger;
      std::cout<<" d0sig="<<d0signif;
      std::cout<<std::endl;
    }
  
    return lept;
  }

  // ============================================================
  MMEvent* TopFakesMMWeightCalculator::getEventMMInfo(const xAOD::JetContainer& jets, const xAOD::MissingET& met)
  {
    float leadingJetpT = -999.;
    float leadingCentralJetpT = -999.;
    unsigned int Njets = 0;
    unsigned int NCentraljets = 0;
    unsigned int nbtag = 0;
    for (const auto* const jetPtr : jets) {
      if (jetPtr->pt()<25000.) continue;
      Njets++;
      if (jetPtr->pt()>leadingJetpT) leadingJetpT = jetPtr->pt();
      if ( std::fabs(jetPtr->eta()) < 2.5 ){
	NCentraljets++;
	if (jetPtr->pt()>leadingCentralJetpT) leadingCentralJetpT = jetPtr->pt();
      }

      if (jetPtr->isAvailable<char>("isbtagged_FixedCutBEff_77")) {
        if (jetPtr->auxdataConst<char>("isbtagged_FixedCutBEff_77")) nbtag++;
      }
    }
  
    MMEvent* evnt = new MMEvent();
    evnt->njets = NCentraljets;
    evnt->ntag = nbtag;
    evnt->jetpt = leadingCentralJetpT/1e3; // Requires GeV? 
    evnt->sumet = met.sumet()/1e3;         //tool needs GeV

    if (m_debug) {
      std::cout<<"Event";
      std::cout<<" njets="<<NCentraljets;
      std::cout<<" ntag="<<nbtag;
      std::cout<<" jetpt="<<leadingCentralJetpT;
      std::cout<<" sumet="<<met.sumet()/1e3;//tool needs GeV
      std::cout<<" met="<<met.met()/1e3;//tool needs GeV
      std::cout<<" metx="<<met.mpx()/1e3;//tool needs GeV
      std::cout<<" mety="<<met.mpy()/1e3;//tool needs GeV
      std::cout<<" metphi="<<met.phi();
      std::cout<<std::endl;
    }
  
    return evnt;
  }
  
  std::vector<std::string> TopFakesMMWeightCalculator::GetFakesMMConfigNames(std::string selection) const
  {
    std::vector<std::string> configs;
    if (m_fakesWeightTools.count(selection) !=0) {
      for (auto config : m_fakesWeightTools.at(selection))
	configs.push_back(config.first);
    }
    return configs;
  }
  
  int TopFakesMMWeightCalculator::getPeriod( const std::string& period_string )
  {
    if (period_string == "2012") {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator getPeriod: can't use 2012 fakes in xAOD framework - returning -1" );
      return -1;
    }
    else if (period_string == "2015") {
      return MMEffSet::Period::Year2015;
    }
    else if (period_string == "2016") {
      return MMEffSet::Period::Year2016;
    }
    else {
      ATH_MSG_INFO(" top::TopFakesMMWeightCalculator getPeriod: can't recognise any valid period - returning -1" );
      return -1;
    }
  }
  
  // =============================================================
  ///This function checks if the lepton ID, isolation, etc... match one of those supported by Top-Fakes
  ///Currently only the "standard" definition is supported
  ///The list of definitions could be expanded in TopFakesxAODUtils (the top::fakes::LeptonDefinitions enum class)
  ///The function also checks the pT range of the leptons - this allows users to have leptons with pT<25GeV (they are ignored) - and the eta range of the jets - this allows users to have forward jets  (they are ignored too)
//   void TopFakesMMWeightCalculator::CheckConfiguration()
//   {
//     if (m_config == nullptr) {
//       m_leptonDefs = fakes::LeptonDefinitions::Other;
//       std::cout<<"Warning: TopFakesMMWeightCalculator::CheckLeptonDefs() was unable to assess the configuration; therefore dummy matrix-method weights will be returned"<<std::endl;
//     }
//     else {
//       if (  m_config->electronID() == "TightLH"
// 	    && m_config->electronIDLoose() == "MediumLH"
// 	    && m_config->electronIsolation() == "Gradient"
// 	    && m_config->electronIsolationLoose() == "None"
// 	    && m_config->muonQuality() == "Medium"
// 	    && m_config->muonQualityLoose() == "Loose"
// 	    && m_config->muonIsolation() == "Gradient"
// 	    && m_config->muonIsolationLoose() == "None" ) {
// 	m_leptonDefs = fakes::LeptonDefinitions::Standard;
// 	//now do checks on object definitions
// 	//electrons
// 	  if (m_config->electronPtcut() < 25000.) {
// 	    std::cout<<"Warning: electrons below pT=25GeV are used (pT cut is at "<<m_config->electronPtcut()/1000.<<"GeV). This is non-standard and not yet supported by TopFakes; therefore all electrons below this threshold will be ignored in the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  else if (m_config->electronPtcut() != 25000.) {
// 	    std::cout<<"Warning: non-standard electron pT cut of "<<m_config->electronPtcut()/1000.<<"GeV is used (standard is 25GeV). It may have unexpected behaviour for the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  if (!m_config->electronVetoLArCrack()) {
// 	    std::cout<<"Warning: veto of electrons on LAr crack was disabled; this is non-standard and not yet supported by TopFakes"<<std::endl;
// 	  }
// 	  
// 	  //muons
// 	  if (m_config->muonPtcut() < 25000.) {
// 	    std::cout<<"Warning: muons below pT=25GeV are used (pT cut is at "<<m_config->muonPtcut()/1000.<<"GeV). This is non-standard and not yet supported by TopFakes; therefore all muons below this threshold will be ignored in the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  else if (m_config->muonPtcut() != 25000.) {
// 	    std::cout<<"Warning: non-standard muon pT cut of "<<m_config->muonPtcut()/1000.<<"GeV is used (standard is 25GeV). It may have unexpected behaviour for the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  if (m_config->muonEtacut() != 2.5) {
// 	    std::cout<<"Warning: non-standard muon eta cut of "<<m_config->jetEtacut()<<" is used (standard is 2.5). It may have unexpected behaviour for the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  
// 	  //jets
// 	  if (m_config->jetEtacut() > 2.5) {
// 	    std::cout<<"Warning: forward jets are used (eta cut is at "<<m_config->jetEtacut()<<"). This is non-standard and not yet supported by TopFakes; therefore all jets above |eta|=2.5 will be ignored in the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  else if (m_config->jetEtacut() != 2.5) {
// 	    std::cout<<"Warning: non-standard jet eta cut of "<<m_config->jetEtacut()<<" is used (standard is 2.5). It may have unexpected behaviour for the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	  if (m_config->jetPtcut() != 25000.) {
// 	    std::cout<<"Warning: non-standard jet pT cut of "<<m_config->jetPtcut()/1000.<<"GeV is used (standard is 25GeV). It may have unexpected behaviour for the calculation of matrix-method weights."<<std::endl;
// 	  }
// 	}
// 	else {
// 	  std::cout<<"Warning: the used lepton definitions are not yet supported by TopFakes; therefore dummy matrix-method weights will be returned"<<std::endl;
// 	}
// 	
// 	
//     }
//     
//   }
}
