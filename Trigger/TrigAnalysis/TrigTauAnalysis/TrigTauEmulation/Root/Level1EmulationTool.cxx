/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Local Includes

#include "TrigTauEmulation/Level1EmulationTool.h"
#include "AsgTools/ToolStore.h"

namespace TrigTauEmul {
// Default constructor
  Level1EmulationTool::Level1EmulationTool(const std::string& name): 
    asg::AsgTool(name),
    m_l1jet_tools(),
    m_l1tau_tools(),
    m_l1xe_tools(),
    m_l1muon_tools()

{

  declareProperty("l1_chains", m_l1_chains_vec, "Vector of the L1 chains to be evaluated");

  declareProperty("JetTools", m_l1jet_tools);
  declareProperty("EmTauTools", m_l1tau_tools);
  declareProperty("XeTools", m_l1xe_tools);
  declareProperty("MuonTools", m_l1muon_tools);

  // Declaration of all the tool we need
  m_name_parser = new Parser(name + "_ChainParser");
  m_l1orl_tool = new TrigTauORLTool(name + "_orl_tool");
  m_l1topo_tool = new Level1TopoSelectionTool(name + "_topo_tool");

}

// Copy constructor
Level1EmulationTool::Level1EmulationTool(const Level1EmulationTool& other): asg::AsgTool(other.name() + "_copy")
{}

// Initialize
StatusCode Level1EmulationTool::initialize()

{
  // initialize parser
  ATH_CHECK(m_name_parser->initialize());


  ATH_MSG_INFO("Start tool initialization");
  for (auto it: m_l1tau_tools) {
    // ATH_CHECK(it.retrieve());
    ATH_MSG_INFO("Initializating "<< it->name());
    ATH_CHECK(it->initialize());
    m_counters[it.name()] = 0;
  }

  // Initialize all the tools we need 
  for (auto it: m_l1jet_tools) {
    ATH_MSG_INFO("Initializating "<< it->name());
    ATH_CHECK(it->initialize());
    m_counters[it.name()] = 0;
  }


  for (auto it: m_l1xe_tools) {
    ATH_MSG_INFO("Initializating "<< it->name());
    ATH_CHECK(it->initialize());
    m_counters[it.name()] = 0;
  }

  for (auto it: m_l1muon_tools) {
    ATH_MSG_INFO("Initializating "<< it->name());
    ATH_CHECK(it->initialize());
    m_counters[it.name()] = 0;
  }

  // topological requirement 
  ATH_CHECK(m_l1topo_tool->initialize());

  // OLR requirement
  ATH_CHECK(m_l1orl_tool->initialize());
  return StatusCode::SUCCESS;
}


// Event calculate -- The meaty part of this algorithm
// I'm gonna try to keep it as small as possible
// and write the actual implementations elsewhere but ...
StatusCode Level1EmulationTool::calculate(const xAOD::EmTauRoIContainer* l1taus, 
					  const xAOD::JetRoIContainer* l1jets,
					  const xAOD::MuonRoIContainer* l1muons,
					  const xAOD::EnergySumRoI* l1xe)
{
  // Reset the counters to 0;
  reset_counters();
  

  for (const auto l1tau : *l1taus) {
    for (auto it: m_l1tau_tools) {
      l1tau->auxdecor<bool>(it.name()) = false;
      if (it->accept(*l1tau)) {
	l1tau->auxdecor<bool>(it.name()) = true;
	m_counters[it.name()] += 1;
	ATH_MSG_DEBUG("L1 TAU: pt = " << l1tau->tauClus() 
		      << ", eta = " << l1tau->eta() 
		      << ", phi = " << l1tau->phi()
		      << ", name = "<< it.name());
      }
    }
  }

  for (const auto l1jet : *l1jets) {
    for (auto it: m_l1jet_tools) {
      l1jet->auxdecor<bool>(it.name()) = false;
      if (it->accept(*l1jet)) {
	l1jet->auxdecor<bool>(it.name()) = true;
	m_counters[it.name()] += 1;
	ATH_MSG_DEBUG("L1 JET: pt = " << l1jet->et8x8() 
		     << ", eta = " << l1jet->eta() 
		     << ", phi = " << l1jet->phi()
		     << ", name = "<< it.name());
      }
    }
  }


  for (const auto l1muon : *l1muons) {
    for (auto it: m_l1muon_tools) {
      l1muon->auxdecor<bool>(it.name()) = false;
      if (it->accept(*l1muon)) {
	l1muon->auxdecor<bool>(it.name()) = true;
	m_counters[it.name()] += 1;
	ATH_MSG_DEBUG("L1 MUON: pt = " << l1muon-> thrValue()
		     << ", eta = " << l1muon->eta() 
		     << ", phi = " << l1muon->phi()
		     << ", name = "<< it.name());
      }
    }
  }


  for (auto it: m_l1xe_tools) {
    l1xe->auxdecor<bool>(it.name()) = false;
    if (it->accept(*l1xe)) {
      l1xe->auxdecor<bool>(it.name()) = true;
      m_counters[it.name()] += 1;
      // MP remove
      ATH_MSG_DEBUG("L1 XE: MET = " << l1xe->energyT() << ", name = " << it.name());
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
      
      ATH_MSG_DEBUG(chain<<": "<<tool0 << "/" << tool1 << "/" << tool2 << "/" << decor);
      if (tool0.find("TAU") != std::string::npos or tool0.find("EM") != std::string::npos) {
	if (tool1.find("TAU") != std::string::npos or tool1.find("EM") != std::string::npos) {
	  if (tool2 == "NULL") 
	    ATH_CHECK(m_l1topo_tool->execute(l1taus, l1taus, decor, tool0, tool1));
	  else
	    ATH_CHECK(m_l1topo_tool->execute(l1taus, l1taus, l1jets, decor, tool0, tool1, tool2));
	} else 
	  ATH_MSG_WARNING("tool 1 is not a TAU tool !"); 

      } else if (tool0.find("MU") != std::string::npos) {
	if (tool1.find("TAU") != std::string::npos) {
	  if (tool2 == "NULL") 
	    ATH_CHECK(m_l1topo_tool->execute(l1muons, l1taus, decor, tool0, tool1 ));
	  else
	    ATH_CHECK(m_l1topo_tool->execute(l1muons, l1taus, l1jets, decor, tool0, tool1, tool2));
	} else 
	  ATH_MSG_WARNING("tool 1 is not a TAU tool !"); 
      }
    } else if (m_name_parser->check_TOPO_type("OLR")) {
      std::string tool0 = m_name_parser->get_vec_items("TOPO")[0];
      std::string tool1 = m_name_parser->get_vec_items("TOPO")[1];
      std::string tool2 = m_name_parser->get_vec_items("TOPO")[2];
      std::string decor = m_name_parser->return_TOPO_object_string();
    
      ATH_MSG_DEBUG(chain<<": "<<tool0 << "/" << tool1 << "/" << tool2 << "/" << decor);
      if (tool0 == "NULL") {
	if (tool1.find("TAU") != std::string::npos or tool1.find("EM") != std::string::npos)
	  if (tool2.find("TAU") != std::string::npos or tool2.find("EM") != std::string::npos)
	    ATH_CHECK(m_l1orl_tool->execute(l1taus, l1taus, tool1, tool2));
	  else if (tool2.find("J") != std::string::npos)
	    ATH_CHECK(m_l1orl_tool->execute(l1taus, l1jets, tool1, tool2));
	  else
	    ATH_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
	else
	  ATH_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
      } else {
	if (tool0.find("TAU") != std::string::npos or tool0.find("EM") != std::string::npos)
	  if (tool1.find("TAU") != std::string::npos or tool1.find("EM") != std::string::npos)
	    if (tool2.find("J") != std::string::npos)
	      ATH_CHECK(m_l1orl_tool -> execute(l1taus, l1taus, l1jets, tool0, tool1, tool2));
	    else
	      ATH_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
	  else
	    ATH_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
	else
	  ATH_MSG_WARNING(chain <<" can not be parsed as an ORL chain");
      }
      
    }
  }


  return StatusCode::SUCCESS;

}


bool Level1EmulationTool::decision(const std::string & chain_name)
{
  bool found = std::find(m_l1_chains_vec.begin(), m_l1_chains_vec.end(), chain_name) != m_l1_chains_vec.end();
  if (not found) {
    ATH_MSG_ERROR(chain_name << "is not registered in the vector of chains");
    return false;
  }
  else {
   ATH_CHECK(m_name_parser->execute(chain_name));
   for (auto it: m_name_parser->get_items()) {
     if (m_counters[it.first] < it.second)
       return false;
   }   

   ATH_MSG_DEBUG(chain_name 
		<< " / " 
		<< m_name_parser->check_TOPO_type("DR") 
		<< " / "
		<< m_name_parser->return_TOPO_object_string());

   if (m_name_parser->check_TOPO_type("DR"))
     return m_l1topo_tool->decision(m_name_parser->return_TOPO_object_string());

   if (m_name_parser->check_TOPO_type("OLR"))
     return m_l1orl_tool->decision(m_name_parser->return_TOPO_object_string());
   }

   return true;
   
}

void Level1EmulationTool::PrintCounters()
{
  // Print out of the counters -- Need to put this under a debug mode
  ATH_MSG_INFO("-- Counts for the considered tools --");
  for (auto &it: m_counters)
    ATH_MSG_INFO(it.first << ": counts = " << it.second);
}

// Finalize
StatusCode Level1EmulationTool::finalize()

{

  return StatusCode::SUCCESS;

}

void Level1EmulationTool::reset_counters()
{

  for (auto &it: m_counters)
    it.second = 0;

}

}// end of the namespace
