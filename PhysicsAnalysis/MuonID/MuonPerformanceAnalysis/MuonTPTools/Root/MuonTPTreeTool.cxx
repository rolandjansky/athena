/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPPlots.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/MuonTPTreeTool.h"
#include "MuonTPTools/MuonTrigTPEfficiencyTool.h"
#include <vector>

MuonTPTreeTool::MuonTPTreeTool(std::string name)
  : AsgTool(name),
    m_trigTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  // determines tree names
  declareProperty("EfficiencyFlag",         m_efficiencyFlag = "ZmmTP");
  // record scale factor info used for closure tests of the CP tools
  declareProperty("RecordScaleFactorInfo",  m_record_SF = false);
  declareProperty("TriggerDecisionTool",    m_trigTool);
}

MuonTPTreeTool::~MuonTPTreeTool() {}

StatusCode MuonTPTreeTool::initialize()
{
  ATH_CHECK(m_trigTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode MuonTPTreeTool::RegisterTrees (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools)
{
  // Create histograms
  for(auto tpSelTool : probeTools) {
    if (tpSelTool->isNominal() || tpSelTool->notIncludedInNominal()){
      std::string cfdirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag();
      m_trees[cfdirs] = std::make_pair (new TTree ((std::string("TPTree_")+tpSelTool->efficiencyFlag()).c_str(),"TP"), cfdirs);
            
      // add tag trigger matching information
      std::vector<std::string> tag_triggers = tpSelTool->tagTriggerList();
      for (auto trig : tag_triggers){
	if (m_tag_trigmatch.find(trig) == m_tag_trigmatch.end()){
	  m_tag_trigmatch[trig] = false;
	}
	if (m_tag_trig_dR.find(trig) == m_tag_trig_dR.end()){
	  m_tag_trig_dR[trig] = -1;
	}
	if (m_triggers.find(trig) == m_triggers.end()){
	  m_triggers[trig] = false;
	}	
      }
            
      for(auto tpEffTool : effTools) {
	if (m_match_flags.find(tpEffTool->efficiencyFlag()) == m_match_flags.end()){
	  m_match_flags[tpEffTool->efficiencyFlag()] = false;
	}
	if (m_scale_factors.find(tpEffTool->efficiencyFlag()) == m_scale_factors.end()){
	  m_scale_factors[tpEffTool->efficiencyFlag()] = 1.0;
	}
	if (tpEffTool->isNominal()){
	  if (m_match_dR.find(tpEffTool->efficiencyFlag()) == m_match_dR.end()){
	    m_match_dR[tpEffTool->efficiencyFlag()] = -1.0;
	  }
	}
	if (tpEffTool->triggerItem()!="" && 
	    m_triggers.find(tpEffTool->triggerItem()) == m_triggers.end()){
	  m_triggers[tpEffTool->triggerItem()] = false;
	}	  
      }

      InitTree (m_trees[cfdirs].first);
    }
  }

  return StatusCode::SUCCESS;

}

// fill the histos
void MuonTPTreeTool::fill(Probe& probe,ToolHandle <IMuonTPSelectionTool> & sel_tool)
{
  std::string cfdirs = m_efficiencyFlag+"/"+sel_tool->efficiencyFlag();
  auto tree = m_trees.find(cfdirs);
  if (tree == m_trees.end()){
    if (sel_tool->isNominal() || sel_tool->notIncludedInNominal()){
      ATH_MSG_ERROR("Trying to fill a nonexisting tree!");
    }
    return;
  }
  TTree* OutTree = tree->second.first;
  auto probe_map = m_match_flags_perProbe.find(&probe);
  if (probe_map == m_match_flags_perProbe.end()){
    ATH_MSG_WARNING("Found a probe that doesn't have any match info - intentional?");
    for (auto entry: m_match_flags){
      entry.second = false;
    }
  }
  else {
    for (auto entry: m_match_flags){
      //  skip double systematics cases
      if (probe_map->second.find(entry.first) ==  probe_map->second.end()) {
	m_match_flags[entry.first] = false;
      }
      else {
	m_match_flags[entry.first] = probe_map->second.at(entry.first);
      }
    }
  }
  auto probe_sf_map = m_scale_factors_perProbe.find(&probe);
  if (probe_sf_map == m_scale_factors_perProbe.end()){
    ATH_MSG_WARNING("Found a probe that doesn't have any SF info - intentional?");
    for (auto entry: m_scale_factors){
      entry.second = 1.00;
    }
  }
  else {
    for (auto entry: m_scale_factors){
      if (probe_sf_map->second.find(entry.first) ==  probe_sf_map->second.end()) {
	m_scale_factors[entry.first] = 1.;
      }
      else {
	m_scale_factors[entry.first] = probe_sf_map->second.at(entry.first);
      }
    }
  }
  auto probe_dR_map = m_match_dR_perProbe.find(&probe);
  if (probe_dR_map == m_match_dR_perProbe.end()){
    ATH_MSG_WARNING("Found a probe that doesn't have any dR info - intentional?");
    for (auto entry: m_match_dR){
      entry.second = 1.00;
    }
  }
  else {
    for (auto entry: m_match_dR){
      if (probe_dR_map->second.find(entry.first) ==  probe_dR_map->second.end()) {
	m_match_dR[entry.first] = -1.;
      }
      else {
	m_match_dR[entry.first] = probe_dR_map->second.at(entry.first);
      }
    }
  }
  FillTriggerInfo();
  FillTagTriggerInfo(probe);
  FillCustomStuff(probe);
  OutTree->Fill();

}

void MuonTPTreeTool::FillTriggerInfo()
{    
  for (auto trig : m_triggers) 
    m_triggers[trig.first] = m_trigTool->isPassed(trig.first);
}

void MuonTPTreeTool::FillTagTriggerInfo(Probe & probe)
{    
  const xAOD::Muon* tagmuon = dynamic_cast <const xAOD::Muon*>(&(probe.tagTrack()));
  // tag is not a muon: happens for truth probes
  if (!tagmuon) return;
  for (auto trigstatus : m_tag_trigmatch){
    std::string trigkey = trigstatus.first;
    if (tagmuon->isAvailable<bool>(std::string("trigmatch_")+trigkey)){
      m_tag_trigmatch[trigkey] = tagmuon->auxdata<bool>(std::string("trigmatch_")+trigkey);
    }
    else{
      ATH_MSG_WARNING("Missing tag trigger matching info for chain "<<trigkey<<"!");
      m_tag_trigmatch[trigkey] = false;
    }
  }
  for (auto trigstatus : m_tag_trig_dR){
    std::string trigkey = trigstatus.first;
    if (tagmuon->isAvailable<float>(std::string("trigmatch_dR_")+trigkey)){
      m_tag_trig_dR[trigkey] = tagmuon->auxdata<float>(std::string("trigmatch_dR_")+trigkey);
    }
    else{
      ATH_MSG_WARNING("Missing tag trigger dR info for chain "<<trigkey<<"!");
      m_tag_trig_dR[trigkey] = -1.;
    }
  }
}

void MuonTPTreeTool::FillCustomStuff(Probe & ){
  // do nothing in the default version
}

void MuonTPTreeTool::AddCustomBranches(TTree*){
  // do nothing in the default version
  // in 'your' version, do tree->Branch("something",&m_something); in here
}

void MuonTPTreeTool::updateMatch(Probe& probe,ToolHandle <IMuonTPEfficiencyTool> & eff_tool)
{
  if (m_match_flags_perProbe.find(&probe) == m_match_flags_perProbe.end()){
    m_match_flags_perProbe[&probe] = std::map<std::string, bool>() ;
  }
  if (m_scale_factors_perProbe.find(&probe) == m_scale_factors_perProbe.end()){
    m_scale_factors_perProbe[&probe] = std::map<std::string, float>() ;
  }
  if (m_match_dR_perProbe.find(&probe) == m_match_dR_perProbe.end()){
    m_match_dR_perProbe[&probe] = std::map<std::string, float>() ;
  }
  m_match_flags_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.isMatched();
  m_scale_factors_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.sfweight();
  m_match_dR_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.dr_match();

}

void MuonTPTreeTool::ForgetKnownProbes(void)
{
  m_match_flags_perProbe.clear();
  m_match_dR_perProbe.clear();
  m_scale_factors_perProbe.clear();
}

void MuonTPTreeTool::InitTree(TTree* tree)
{
  // fill the match flags 
  for (auto flag : m_match_flags){
    tree->Branch((std::string("probe_matched_")+flag.first).c_str(), &(m_match_flags[flag.first]));
    if (m_record_SF) tree->Branch((std::string("probe_scale_factor_")+flag.first).c_str(), &(m_scale_factors[flag.first]));
  }
  for (auto flag : m_match_dR){
    tree->Branch((std::string("probe_dRMatch_")+flag.first).c_str(), &(m_match_dR[flag.first]));
  }
  // also fill in the tag trigger info
  for (auto bla : m_tag_trig_dR){
    tree->Branch((std::string("tag_matched_")+bla.first).c_str(),&(m_tag_trigmatch[bla.first]));
    tree->Branch((std::string("tag_dRMatch_")+bla.first).c_str(),&(m_tag_trig_dR[bla.first]));
  }
  // add branches for event triggers
  for (auto bla : m_triggers) {
    tree->Branch(bla.first.c_str(),&(m_triggers[bla.first]));    
  }
  
  AddCustomBranches(tree);
}

/// retrieve booked trees
std::vector<std::pair<TTree*, std::string> > MuonTPTreeTool::retrieveBookedTrees()
{
  std::vector<std::pair<TTree*, std::string> > OutTrees;
  for (auto tree: m_trees){
    OutTrees.push_back(tree.second);
  }
  return OutTrees;
}







