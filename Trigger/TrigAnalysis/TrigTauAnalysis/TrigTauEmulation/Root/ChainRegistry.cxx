/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2

#include "TrigTauEmulation/HltChain.h"
#include "TrigTauEmulation/ChainRegistry.h"

  ChainRegistry::ChainRegistry(const std::string & name) 
: asg::AsgTool(name)
{}

ChainRegistry::~ChainRegistry() {}

ChainRegistry::ChainRegistry(const ChainRegistry& other): asg::AsgTool(other.name() + "_copy") {
}

HltChain& ChainRegistry::getChain(const std::string &name) {
  auto it = m_chains.find(name);
  if(it == m_chains.end()) {
    loadDefaultChain(name); 
  }

  it = m_chains.find(name);
  if(it == m_chains.end()) {
    std::stringstream e;
    e << "Chain " << name << " does not exist";
    throw std::invalid_argument(e.str());
  }

  return it->second;
}

bool ChainRegistry::checkChain(const std::string &name){
  if(m_chains.find(name) == m_chains.end()) { 
    loadDefaultChain(name);
  }

  if(m_chains.find(name) != m_chains.end()) {
    return true;
  }

  return false;
}

StatusCode ChainRegistry::addChain(const std::string &name, const HltChain &chain) {
  // we presume that this chain relies on items that are already defined at this stage

  if(m_chains.count(name) != 0) {
    std::stringstream e;
    e << "Chain " << name << " already exists";
    throw std::invalid_argument(e.str());
  }

  m_chains[name] = chain;
  return StatusCode::SUCCESS;
}

StatusCode ChainRegistry::initialize() {

  // pick up the default chains
  //m_chains = HltParsing::chains();

  return StatusCode::SUCCESS;
}

void ChainRegistry::loadDefaultChain(const std::string& name) {
  // Single tau

  // tracktwo chains
  if(name == "HLT_tau25_perf_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau25_perf_tracktwo", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_idperf_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau25_idperf_tracktwo", "TAU12")}, "TAU12");
  }

  if(name == "HLT_tau25_perf_tracktwo_L1TAU12") {
    m_chains[name] = HltChain({
        HltItem("tau25_perf_tracktwo", "TAU12")}, "TAU12");
  }

  if(name == "HLT_tau25_loose1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau25_loose1_tracktwo", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_medium1_tracktwo_L1TAU12") {
    m_chains[name] = HltChain({
        HltItem("tau25_medium1_tracktwo", "TAU12")}, "TAU12");
  }

  if(name == "HLT_tau25_medium1_tracktwo_L1TAU12IL") {
    m_chains[name] = HltChain({
        HltItem("tau25_medium1_tracktwo", "TAU12IL")}, "TAU12IL");
  }

  if(name == "HLT_tau25_medium1_tracktwo_L1TAU12IT") {
    m_chains[name] = HltChain({
        HltItem("tau25_medium1_tracktwo", "TAU12IT")}, "TAU12IT");
  }

  if(name == "HLT_tau25_tight1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau25_tight1_tracktwo", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau35_perf_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau35_perf_tracktwo", "TAU20IM")}, "TAU20IM");
  }

  if(name == "HLT_tau35_loose1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau35_loose1_tracktwo", "TAU20IM")}, "TAU20IM");
  }

  if(name == "HLT_tau35_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau35_medium1_tracktwo", "TAU20IM")}, "TAU20IM");
  }

  if(name == "HLT_tau35_medium1_tracktwo_L1TAU20") {
    m_chains[name] = HltChain({
        HltItem("tau35_medium1_tracktwo", "TAU20")}, "TAU20");
  }

  if(name == "HLT_tau35_medium1_tracktwo_L1TAU20IL") {
    m_chains[name] = HltChain({
        HltItem("tau35_medium1_tracktwo", "TAU20IL")}, "TAU20IL");
  }

  if(name == "HLT_tau35_medium1_tracktwo_L1TAU20IT") {
    m_chains[name] = HltChain({
        HltItem("tau35_medium1_tracktwo", "TAU20IT")}, "TAU20IT");
  }

  if(name == "HLT_tau35_tight1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau35_tight1_tracktwo", "TAU20IM")}, "TAU20IM");
  }

  if(name == "HLT_tau80_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau80_medium1_tracktwo", "TAU40")}, "TAU40");
  }

  if(name == "HLT_tau125_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau125_medium1_tracktwo", "TAU60")}, "TAU60");
  }

  if(name == "HLT_tau160_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau160_medium1_tracktwo", "TAU60")}, "TAU60");
  }



  // # perf_ptonly 
  if(name == "HLT_tau0_perf_ptonly_L1TAU12") {
    m_chains[name] = HltChain({
        HltItem("tau0_perf_ptonly", "TAU12")}, "TAU12");
  }

  if(name == "HLT_tau0_perf_ptonly_L1TAU60") {
    m_chains[name] = HltChain({
        HltItem("tau0_perf_ptonly", "TAU60")}, "TAU60");
  }

  if(name == "HLT_tau5_perf_ptonly_L1TAU8") {
    m_chains[name] = HltChain({
        HltItem("tau5_perf_ptonly", "TAU8")}, "TAU8");
  }

  if(name == "HLT_tau25_perf_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau25_perf_ptonly", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_perf_ptonly_L1TAU12") {
    m_chains[name] = HltChain({
        HltItem("tau25_perf_ptonly", "TAU12")}, "TAU12");
  }

  if(name == "HLT_tau25_loose1_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau25_loose1_ptonly", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_medium1_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau25_medium1_ptonly", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_tight1_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau25_tight1_ptonly", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau25_medium1_mvonly") {
    m_chains[name] = HltChain({
        HltItem("tau25_medium1_mvonly", "TAU12IM")}, "TAU12IM");
  }

  if(name == "HLT_tau35_perf_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau35_perf_ptonly", "TAU20IM")}, "TAU20IM");
  }

  if(name == "HLT_tau35_loose1_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau35_loose1_ptonly", "TAU20IM")}, "TAU20IM");
  }

  if(name == "HLT_tau35_tight1_ptonly") {
    m_chains[name] = HltChain({
        HltItem("tau35_tight1_ptonly", "TAU20IM")}, "TAU20IM");
  }

  // Tau + MET
  if(name == "HLT_tau35_medium1_tracktwo_xe70_L1XE45") {
    m_chains[name] = HltChain({
        HltItem("tau35_medium1_tracktwo", "TAU20IM"),
        HltItem("xe70", "XE45")}, "TAU20IM_2J20_XE45");
  }


  // e + tau
  if(name == "HLT_e17_medium_tau25_medium1_tracktwo_L1DR-EM15TAU12I-J25") {
    m_chains[name] = HltChain({
        HltItem("e17_medium", "EM15HI"),
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "DR-EM15TAU12I-J25");
  }

  if(name == "HLT_e17_lhmedium_tau25_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("e17_lhmedium", "EM15HI"), 
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "EM15HI_2TAU12IM_J25_3J12");
  }

  if(name == "HLT_e17_medium_tau25_medium1_tracktwo_xe50") {
    m_chains[name] = HltChain({
        HltItem("e17_medium", "EM15HI"),
        HltItem("tau25_medium1_tracktwo", "TAU12IM"),
        HltItem("xe50", "XE35")}, "EM15HI_2TAU12IM_XE35");
  }

  // e + tau + met
  if(name == "HLT_e17_lhmedium_tau25_medium1_tracktwo_xe50") {
    m_chains[name] = HltChain({
        HltItem("e17_lhmedium", "EM15HI"),
        HltItem("tau25_medium1_tracktwo", "TAU12IM"),
        HltItem("xe50", "XE35")}, "EM15HI_2TAU12IM_XE35");
  }



  // mu + tau triggers
  if(name == "HLT_mu14_tau25_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"),
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "MU10_TAU12IM_J25_2J12");
  }

  if(name == "HLT_mu14_tau35_medium1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"),
        HltItem("tau35_medium1_tracktwo", "TAU20IM")}, "MU10_TAU20IM_J25_2J12");
  }

  if(name == "HLT_mu14_tau35_medium1_tracktwo_L1TAU20") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"),
        HltItem("tau35_medium1_tracktwo", "TAU20")}, "MU10_TAU20");
  }

  if(name == "HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I_TAU12-J25") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"), 
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "DR-MU10TAU12I_TAU12-J25");
  }

  if(name == "HLT_mu14_tau25_medium1_tracktwo_L1DR-MU10TAU12I") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"), 
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "DR-MU10TAU12I");
  }

  if(name == "HLT_mu14_tau25_medium1_tracktwo_L1MU10_TAU12I-J25") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"),
        HltItem("tau25_medium1_tracktwo", "TAU12IM")}, "MU10_TAU12I-J25");
  }

  // HLT_mu14_iloose is not in the MC15 menu ...
  // HLT_mu14_iloose_tau25_medium1_tracktwo
  // HLT_mu14_iloose_tau35_medium1_tracktwo

  // mu + tau + met trigger
  if(name == "HLT_mu14_tau25_medium1_tracktwo_xe50") {
    m_chains[name] = HltChain({
        HltItem("mu14", "MU10"),
        HltItem("tau25_medium1_tracktwo", "TAU12IM"),
        HltItem("xe50", "XE35")}, "MU10_TAU12IM_XE35");
  } 
  // HLT_mu14_iloose_tau25_medium1_tracktwo_xe50


  // di-tau triggers
  if(name == "HLT_tau35_loose1_tracktwo_tau25_loose1_tracktwo") {
    m_chains[name] = HltChain({
        HltItem("tau35_loose1_tracktwo", "TAU20IM"),
        HltItem("tau25_loose1_tracktwo", "TAU12IM")}, "TAU20IM_2TAU12IM_J25_2J20_3J12");
  }

  return;
}
