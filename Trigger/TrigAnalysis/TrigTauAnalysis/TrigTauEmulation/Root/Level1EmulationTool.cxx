/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
//Local Includes

#include <iomanip>
#include "TMath.h"
#include "AsgTools/ToolStore.h"
#include "TrigTauEmulation/Level1EmulationTool.h"
#include "TrigTauEmulation/ToolsRegistry.h"
#include "TrigTauEmulation/MsgStream.h"
#include "TrigTauEmulation/Utils.h"

#include "xAODCore/ShallowCopy.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "xAODBase/IParticleHelpers.h"

namespace TrigTauEmul {
  // Default constructor
  Level1EmulationTool::Level1EmulationTool(const std::string& name): 
    asg::AsgTool(name)
    //m_l1jet_tools(),
    //m_l1tau_tools(),
    //m_l1xe_tools(),
    //m_l1muon_tools()

  {

    declareProperty("l1_chains", m_l1_chains_vec, "Vector of the L1 chains to be evaluated");
    declareProperty("useShallowCopies", m_useShallowCopies=true, "Internally use shallow copies of the objects");

    //declareProperty("JetTools", m_l1jet_tools);
    //declareProperty("EmTauTools", m_l1tau_tools);
    //declareProperty("XeTools", m_l1xe_tools);
    //declareProperty("MuonTools", m_l1muon_tools);

    // Declaration of all the tool we need
    m_name_parser = new Parser(name + "_ChainParser");
    m_l1orl_tool = new TrigTauORLTool(name + "_orl_tool");
    m_l1topo_tool = new Level1TopoSelectionTool(name + "_topo_tool");
  }

  // Copy constructor
  Level1EmulationTool::Level1EmulationTool(const Level1EmulationTool& other): asg::AsgTool(other.name() + "_copy")
  {
    m_l1_chains_vec = other.m_l1_chains_vec;
    m_name_parser = new Parser(other.name() + "_ChainParser_copy");
    m_l1orl_tool = new TrigTauORLTool(other.name() + "_orl_tool_copy");
    m_l1topo_tool = new Level1TopoSelectionTool(other.name() + "_topo_tool_copy");    
    
    // This is a fallback initialization mostly meant for athena running - in RootCore, TriggerValidation does this already
    if(Utils::toolStoreContains<ToolsRegistry>("ToolsRegistry")) {
      m_registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
    } else {
      m_registry = new ToolsRegistry("ToolsRegistry");
    } 
  }

  void Level1EmulationTool::GetChains() {
    //auto registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
    //std::cout << "CALLING GetChains()" << std::endl;
    
    //m_l1jet_tools = m_registry->GetL1JetTools();
    //m_l1tau_tools = m_registry->GetL1TauTools();
    //m_l1xe_tools = m_registry->GetL1XeTools();
    //_l1muon_tools = m_registry->GetL1MuonTools()
    
    initializeTools();
  }

  Level1EmulationTool::~Level1EmulationTool(){
    delete m_name_parser;
    delete m_l1orl_tool;
    delete m_l1topo_tool;
  }

  // Initialize
  StatusCode Level1EmulationTool::initialize() {
    // This is a fallback initialization mostly meant for athena running - in RootCore, TriggerValidation does this already
    if(Utils::toolStoreContains<ToolsRegistry>("ToolsRegistry")) {
      m_registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
    } else {
      m_registry = new ToolsRegistry("ToolsRegistry");
    } 

    // initialize parser
    ATH_CHECK(m_name_parser->initialize());
    m_name_parser->msg().setLevel(this->msg().level());
  
    ATH_CHECK(initializeTools());
    return StatusCode::SUCCESS;
  }

  StatusCode Level1EmulationTool::initializeTools() {
    
    MY_MSG_INFO("Initializing L1 chains");

    m_registry->msg().setLevel(this->msg().level());
    ATH_CHECK(m_registry->initialize());

    for(const auto &chain: m_l1_chains_vec){
      MY_MSG_INFO("Got L1 chain: " << chain);
      ATH_CHECK(m_name_parser->execute(chain));
      
      unsigned int N = m_name_parser->get_vec_items().size();
      unsigned int i = 0;
  
      MY_MSG_INFO("Will construct " << N << " tools for chain");
      for(const auto &s: m_name_parser->get_vec_items()){
        MY_MSG_INFO("Chain: " << chain << " => constructing tool " << ++i << "/" << N << " " << s);
        MY_MSG_INFO("m_registry = " << m_registry);
        ATH_CHECK(m_registry->initializeTool(s));
      }
    }

    MY_MSG_INFO("Start tool initialization");
    //for (auto it: m_l1tau_tools) {
    for (auto it: m_registry->selectTools<EmTauSelectionTool*>()){
      MY_MSG_INFO("Initializating "<< it->name());
      // ATH_CHECK(it.retrieve());
      ATH_CHECK(it->initialize());
      m_counters[it->name()] = 0;
    }

    // Initialize all the tools we need 
    //for (auto it: m_l1jet_tools) {
    for (auto it: m_registry->selectTools<JetRoISelectionTool*>()){
      MY_MSG_INFO("Initializating "<< it->name());
      // ATH_CHECK(it.retrieve());
      ATH_CHECK(it->initialize());
      m_counters[it->name()] = 0;
    }

    //for (auto it: m_l1xe_tools) {
    for (auto it: m_registry->selectTools<EnergySumSelectionTool*>()){
      MY_MSG_INFO("Initializating "<< it->name());
      // ATH_CHECK(it.retrieve());
      ATH_CHECK(it->initialize());
      m_counters[it->name()] = 0;
    }

    //for (auto it: m_l1muon_tools) {
    for (auto it: m_registry->selectTools<MuonRoISelectionTool*>()){
      MY_MSG_INFO("Initializating "<< it->name());
      // ATH_CHECK(it.retrieve());
      ATH_CHECK(it->initialize());
      m_counters[it->name()] = 0;
    }

    // topological requirement 
    ATH_CHECK(m_l1topo_tool->initialize());

    // OLR requirement
    ATH_CHECK(m_l1orl_tool->initialize());
    m_l1orl_tool->msg().setLevel(this->msg().level());
    
    MY_MSG_INFO("Will consider " << m_registry->getNumberOfTools<EmTauSelectionTool*>() << " EmTau tools");
    MY_MSG_INFO("Will consider " << m_registry->getNumberOfTools<JetRoISelectionTool*>() << " JetRoI tools");
    MY_MSG_INFO("Will consider " << m_registry->getNumberOfTools<MuonRoISelectionTool*>() << " MuonRoI tools");
    MY_MSG_INFO("Will consider " << m_registry->getNumberOfTools<EnergySumSelectionTool*>() << " EnergySum tools");

    return StatusCode::SUCCESS;
  }

  template<typename T> ToolHandleArray<T> Level1EmulationTool::removeTools(const ToolHandleArray<T>& tools, const std::set<std::string>& usedTools){
    ToolHandleArray<T> retval;
    
    // erasing from a ToolHandleArray creates an odd segfault, so write a new one
    for(auto it: tools) {
      if(std::find(usedTools.begin(), usedTools.end(), it->name()) == usedTools.end()){
        MY_MSG_INFO("not using tool " << it->name());
      } else {
        MY_MSG_INFO("will use tool " << it->name());
        retval.push_back(it);
      }
    }

    return retval;
  }


  StatusCode Level1EmulationTool::removeUnusedTools(const std::set<std::string>& usedTools){
    //NOTE: no longer needed with lazy initialization to clean up the separate lists
    
    //ToolHandleArray<IJetRoISelectionTool> l1jet_tools;
    //ToolHandleArray<IEmTauSelectionTool> l1tau_tools;
    //ToolHandleArray<IEnergySumSelectionTool> l1xe_tools;
    //ToolHandleArray<IMuonRoISelectionTool> l1muon_tools;
    
    //m_l1jet_tools = removeTools(m_l1jet_tools, usedTools);
    //m_l1tau_tools = removeTools(m_l1tau_tools, usedTools);
    //m_l1muon_tools = removeTools(m_l1muon_tools, usedTools);
    //m_l1xe_tools = removeTools(m_l1xe_tools, usedTools);

    //m_l1_chains_vec = removeTools(m_l1_chains_vec, usedTools);
    m_l1_chains_vec.clear();
    for(auto it: usedTools){
      m_l1_chains_vec.push_back("L1_"+it);
    }
    
    return StatusCode::SUCCESS;
  }

  // Event calculate -- The meaty part of this algorithm
  // I'm gonna try to keep it as small as possible
  // and write the actual implementations elsewhere but ...
  StatusCode Level1EmulationTool::calculate(const xAOD::EmTauRoIContainer* l1taus_in, 
      const xAOD::JetRoIContainer* l1jets_in,
      const xAOD::MuonRoIContainer* l1muons_in,
      const xAOD::EnergySumRoI* l1xe_in)
  {
    // Reset the counters to 0;
    reset_counters();
    //auto registry = asg::ToolStore::get<ToolsRegistry>("ToolsRegistry");
      
    const xAOD::EmTauRoIContainer* l1taus = l1taus_in; 
    const xAOD::JetRoIContainer* l1jets = l1jets_in; 
    const xAOD::MuonRoIContainer* l1muons = l1muons_in;
    const xAOD::EnergySumRoI* l1xe = l1xe_in;

    xAOD::ShallowAuxContainer* l1taus_aux = nullptr; 
    xAOD::ShallowAuxContainer* l1jets_aux = nullptr; 
    xAOD::ShallowAuxContainer* l1muons_aux = nullptr; 
    xAOD::ShallowAuxInfo* l1xe_aux = nullptr;

    // Build some shallow copies if needed
    if(m_useShallowCopies) { 
      std::pair<xAOD::EmTauRoIContainer*, xAOD::ShallowAuxContainer*> taus_copy = xAOD::shallowCopyContainer(*l1taus_in);
      l1taus = taus_copy.first;
      l1taus_aux = taus_copy.second;
      
      std::pair<xAOD::JetRoIContainer*, xAOD::ShallowAuxContainer*> jets_copy = xAOD::shallowCopyContainer(*l1jets_in);
      l1jets = jets_copy.first;
      l1jets_aux = jets_copy.second;

      std::pair<xAOD::MuonRoIContainer*, xAOD::ShallowAuxContainer*> muons_copy = xAOD::shallowCopyContainer(*l1muons_in);
      l1muons = muons_copy.first;
      l1muons_aux = muons_copy.second;
      
      std::pair<xAOD::EnergySumRoI*, xAOD::ShallowAuxInfo*> xe_copy = xAOD::shallowCopyObject(*l1xe_in);
      l1xe = xe_copy.first;
      l1xe_aux = xe_copy.second;
    }

    // Now actually do something

    for (const auto l1tau : *l1taus) {
      for (auto it: m_registry->selectTools<EmTauSelectionTool*>()){
        MY_MSG_DEBUG("testing L1 tau tool on EmTauRoIContainer " << it->name());
        l1tau->auxdecor<bool>(it->name()) = false;
        MY_MSG_DEBUG("Decorating with " << it->name());
        if (it->accept(*l1tau)) {
          MY_MSG_DEBUG("Accept " << it->name());
        //if (not it->accept(*l1tau)) { //hack
          l1tau->auxdecor<bool>(it->name()) = true;
          m_counters[it->name()] += 1;
        
          MY_MSG_DEBUG("\t passed");
          MY_MSG_DEBUG("\t => L1 TAU: pt = " << l1tau->tauClus() 
              << ", eta = " << l1tau->eta() 
              << ", phi = " << l1tau->phi()
              << ", name = "<< it->name());
        } else {
          MY_MSG_DEBUG("\t rejected");
        }
      }
    }

    for (const auto l1jet : *l1jets) {
      for (auto it: m_registry->selectTools<JetRoISelectionTool*>()){
        l1jet->auxdecor<bool>(it->name()) = false;
        if (it->accept(*l1jet)) {
          l1jet->auxdecor<bool>(it->name()) = true;
          m_counters[it->name()] += 1;
          MY_MSG_DEBUG("L1 JET: pt = " << l1jet->et8x8() 
              << ", eta = " << l1jet->eta() 
              << ", phi = " << l1jet->phi()
              << ", name = "<< it->name());
        }
      }
    }

    for (const auto l1muon : *l1muons) {
      for (auto it: m_registry->selectTools<MuonRoISelectionTool*>()){
        l1muon->auxdecor<bool>(it->name()) = false;
        if (it->accept(*l1muon)) {
          l1muon->auxdecor<bool>(it->name()) = true;
          m_counters[it->name()] += 1;
          MY_MSG_DEBUG("L1 MUON: pt = " << l1muon-> thrValue()
              << ", eta = " << l1muon->eta() 
              << ", phi = " << l1muon->phi()
              << ", name = "<< it->name());
        }
      }
    }

    for (auto it: m_registry->selectTools<EnergySumSelectionTool*>()){
      l1xe->auxdecor<bool>(it->name()) = false;
      if (it->accept(*l1xe)) {
        l1xe->auxdecor<bool>(it->name()) = true;
        m_counters[it->name()] += 1;
        // MP remove
        MY_MSG_DEBUG("L1 XE: MET = " << l1xe->energyT() << ", name = " << it->name());
      }
    }

    // loop over the chains to apply the DR/ORL tools
    for (auto chain: m_l1_chains_vec) {
      // 1- parse the name 
      ATH_CHECK(m_name_parser->execute(chain));
      if (m_name_parser->check_TOPO_type("DR")) {

        std::string tool0 = m_name_parser->get_vec_items("TOPO")[0];
        std::string tool1 = m_name_parser->get_vec_items("TOPO")[1];
        std::string tool2 = m_name_parser->get_vec_items("TOPO")[2];
        std::string decor = m_name_parser->return_TOPO_object_string();

        MY_MSG_DEBUG(chain<<": "<<tool0 << "/" << tool1 << "/" << tool2 << "/" << decor);
        if (tool0.find("TAU") != std::string::npos or tool0.find("EM") != std::string::npos) {
          if (tool1.find("TAU") != std::string::npos or tool1.find("EM") != std::string::npos) {
            if (tool2 == "NULL") 
              ATH_CHECK(m_l1topo_tool->execute(l1taus, l1taus, decor, tool0, tool1));
            else
              ATH_CHECK(m_l1topo_tool->execute(l1taus, l1taus, l1jets, decor, tool0, tool1, tool2));
          } else 
            MY_MSG_WARNING("tool 1 is not a TAU tool !"); 

        } else if (tool0.find("MU") != std::string::npos) {
          if (tool1.find("TAU") != std::string::npos) {
            if (tool2 == "NULL") 
              ATH_CHECK(m_l1topo_tool->execute(l1muons, l1taus, decor, tool0, tool1 ));
            else
              ATH_CHECK(m_l1topo_tool->execute(l1muons, l1taus, l1jets, decor, tool0, tool1, tool2));
          } else 
            MY_MSG_WARNING("tool 1 is not a TAU tool !"); 
        }
      } else if (m_name_parser->check_TOPO_type("OLR")) {
        std::string tool0 = m_name_parser->get_vec_items("TOPO")[0];
        std::string tool1 = m_name_parser->get_vec_items("TOPO")[1];
        std::string tool2 = m_name_parser->get_vec_items("TOPO")[2];
        std::string decor = m_name_parser->return_TOPO_object_string();

        MY_MSG_DEBUG("Will execute ORL tool for chain " << chain);
        MY_MSG_DEBUG("Chain / tool0 / tool1/ tool2");
        MY_MSG_DEBUG(chain<<": "<<tool0 << "/" << tool1 << "/" << tool2 << "/" << decor);
        if (tool0 == "NULL") {
          if (tool1.find("TAU") != std::string::npos or tool1.find("EM") != std::string::npos) {
            if (tool2.find("TAU") != std::string::npos or tool2.find("EM") != std::string::npos) {
              ATH_CHECK(m_l1orl_tool->execute(l1taus, l1taus, tool1, tool2));
            } else if (tool2.find("J") != std::string::npos) {
              ATH_CHECK(m_l1orl_tool->execute(l1taus, l1jets, tool1, tool2));
            } else {
              MY_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
            }
          } else {
            MY_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
          }
        } else {
          if (tool0.find("TAU") != std::string::npos or tool0.find("EM") != std::string::npos)
            if (tool1.find("TAU") != std::string::npos or tool1.find("EM") != std::string::npos)
              if (tool2.find("J") != std::string::npos)
                ATH_CHECK(m_l1orl_tool -> execute(l1taus, l1taus, l1jets, tool0, tool1, tool2));
              else
                MY_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
            else
              MY_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
          else
            MY_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
        }

      }
    }
    
    // Now clean up our shallow copies
    if(m_useShallowCopies) { 
      delete l1taus;
      delete l1taus_aux;
      delete l1jets;
      delete l1jets_aux;
      delete l1muons;
      delete l1muons_aux;
      delete l1xe;
      delete l1xe_aux;
    }
    
    return StatusCode::SUCCESS;
  }


  bool Level1EmulationTool::decision(const std::string & chain_name) {
    MY_MSG_DEBUG("Level1EmulationTool::decision");
    bool found = (std::find(m_l1_chains_vec.begin(), m_l1_chains_vec.end(), chain_name) != m_l1_chains_vec.end());
    if (not found) {
      MY_MSG_ERROR(chain_name << "is not registered in the vector of chains");
      return false;
    }

    MY_MSG_DEBUG("executing " << chain_name);
    ATH_CHECK(m_name_parser->execute(chain_name), false);
    for (auto it: m_name_parser->get_items()) {
      MY_MSG_DEBUG(it.first << " " << m_counters[it.first] << " " << it.second);
      if (m_counters[it.first] < it.second) {
        return false;
      }
    }   

    MY_MSG_DEBUG(chain_name 
        << " / is DR type? " 
        << m_name_parser->check_TOPO_type("DR") 
        << " / topo object string = '"
        << m_name_parser->return_TOPO_object_string() << "'");

    if (m_name_parser->check_TOPO_type("DR")) {
      MY_MSG_DEBUG("returning DR decision");
      return m_l1topo_tool->decision(m_name_parser->return_TOPO_object_string());
    }

    if (m_name_parser->check_TOPO_type("OLR")){
      MY_MSG_DEBUG("returning OLR decision");
      return m_l1orl_tool->decision(m_name_parser->return_TOPO_object_string());
    }

    MY_MSG_DEBUG("returning default value");
    return true;
  }

  StatusCode Level1EmulationTool::PrintCounters() {
    MY_MSG_INFO("");
    MY_MSG_INFO("-- Counts for the considered tools --");
    std::ostringstream headers;
    std::ostringstream line_tools;
    std::ostringstream line_counts;
    headers    <<  "+--------+";
    line_tools  << "| ITEMS  |";
    line_counts << "| COUNTS |";
    for (auto &it: m_counters) {
      headers << "--------+";
      line_tools  << std::setw(6) << it.first << " | ";
      line_counts << std::setw(6) << it.second << " | ";
    }
    MY_MSG_INFO(headers.str());
    MY_MSG_INFO(line_tools.str());
    MY_MSG_INFO(line_counts.str());
    return StatusCode::SUCCESS;
  }



  StatusCode Level1EmulationTool::PrintReport(const xAOD::EmTauRoIContainer* l1taus, 
					      const xAOD::JetRoIContainer* l1jets,
					      const xAOD::MuonRoIContainer* l1muons,
					      const xAOD::EnergySumRoI* l1xe)
  {
    MY_MSG_INFO("");
    MY_MSG_INFO(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    MY_MSG_INFO("");
    MY_MSG_INFO("Container size");
    MY_MSG_INFO("+----------+----+");
    std::ostringstream line_taus;
    line_taus << "| l1 taus  | " << std::setw(3) << l1taus->size() << "|";
    MY_MSG_INFO(line_taus.str());
    std::ostringstream line_jets;
    line_jets << "| l1 jets  | " << std::setw(3) << l1jets->size() << "|";
    MY_MSG_INFO(line_jets.str());
    std::ostringstream line_muons;
    line_muons << "| l1 muons | " << std::setw(3) << l1muons->size() << "|";
    MY_MSG_INFO(line_muons.str());
    MY_MSG_INFO("+----------+----+");
    MY_MSG_INFO("");
    
    MY_MSG_INFO("\t L1 EMTAUS");
    MY_MSG_INFO("\t +-------+----------+-------------+------------+----------+---------+");
    MY_MSG_INFO("\t | Index |  tauClus |     eta     |     phi    |  emIsol  | roiType |");
    for (const auto l1tau: *l1taus) {
      std::ostringstream l1_line;
      l1_line << "\t |" << std::setw(6);
      l1_line << (l1tau->index()) ;
      l1_line << " | " << std::setw(8);
      l1_line << (l1tau->tauClus()) ;
      l1_line << " | " << std::setw(11);
      l1_line << (l1tau->eta()); 
      l1_line << " | " << std::setw(10);
      l1_line << (l1tau->phi()); 
      l1_line << " | " << std::setw(8);
      l1_line << (l1tau->emIsol()); 
      l1_line << " | " << std::setw(7);
      l1_line << (l1tau->roiType()); 
      l1_line << " | ";
      MY_MSG_INFO(l1_line.str());
    }
    MY_MSG_INFO("\t +-------+----------+-------------+------------+----------+---------+");
    MY_MSG_INFO("");

    MY_MSG_INFO("\t L1 jets");
    MY_MSG_INFO("\t +-------+----------+-------+-----------+");
    MY_MSG_INFO("\t | Index |  etLarge |  eta  |    phi    |");
    for (const auto l1jet: *l1jets) {
      std::ostringstream l1_line;
      l1_line << "\t |" << std::setw(6);
      l1_line << (l1jet->index()) ;
      l1_line << " | " << std::setw(8);
      l1_line << (l1jet->etLarge()) ;
      l1_line << " | " << std::setw(5);
      l1_line << (l1jet->eta()); 
      l1_line << " | " << std::setw(9);
      l1_line << (l1jet->phi()); 
      l1_line << " | ";
      MY_MSG_INFO(l1_line.str());
    }
    MY_MSG_INFO("\t +-------+----------+-------+-----------+");
    MY_MSG_INFO("");
    
    MY_MSG_INFO("\t L1 muons");
    MY_MSG_INFO("\t +-------+-----------+-------+-----------+");
    MY_MSG_INFO("\t | Index |  thrValue |  eta  |    phi    |");
    for (const auto l1muon: *l1muons) {
      std::ostringstream l1_line;
      l1_line << "\t |" << std::setw(6);
      l1_line << (l1muon->index()) ;
      l1_line << " | " << std::setw(9);
      l1_line << (l1muon->thrValue()) ;
      l1_line << " | " << std::setw(5);
      l1_line << (l1muon->eta()); 
      l1_line << " | " << std::setw(9);
      l1_line << (l1muon->phi()); 
      l1_line << " | ";
      MY_MSG_INFO(l1_line.str());
    }
    MY_MSG_INFO("\t +-------+-----------+-------+-----------+");
    MY_MSG_INFO("");

    MY_MSG_INFO("\t L1 xe");
    MY_MSG_INFO("\t +---------+---------+----------+");
    MY_MSG_INFO("\t | energyX | energyY |    MET   |");
    std::ostringstream l1xe_line;
    l1xe_line << "\t |" << std::setw(8);
    l1xe_line << (l1xe->energyX()) ;
    l1xe_line << " | " << std::setw(7);
    l1xe_line << (l1xe->energyY()) ;
    l1xe_line << " | " << std::setw(8);
    l1xe_line << (TMath::Sqrt(TMath::Power(l1xe->energyX(), 2) + TMath::Power(l1xe->energyY(), 2))); 
    l1xe_line << " | ";
    MY_MSG_INFO(l1xe_line.str());
    MY_MSG_INFO("\t +---------+---------+----------+");
    MY_MSG_INFO("");
    MY_MSG_INFO("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
    MY_MSG_INFO("");

    return StatusCode::SUCCESS;
  }


  // Finalize
  StatusCode Level1EmulationTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void Level1EmulationTool::reset_counters() {
    for (auto &it: m_counters) {
      it.second = 0;
    }
  }

}// end of the namespace
