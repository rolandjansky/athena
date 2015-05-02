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
#include <vector>
#include <vector>

MuonTPTreeTool::MuonTPTreeTool(std::string name)
	  : AsgTool(name){
	  declareProperty("EfficiencyFlag",        m_efficiencyFlag = "ZmmTP");

}

StatusCode MuonTPTreeTool::RegisterTrees (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools){


      // Create histograms
      for(auto tpSelTool : probeTools) {
        std::string cfdirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag();
        m_trees[cfdirs] = std::make_pair (new TTree ((std::string("TPTree_")+tpSelTool->efficiencyFlag()).c_str(),"TP"), cfdirs);
        for(auto tpEffTool : effTools) {
            if (m_match_flags.find(tpEffTool->efficiencyFlag()) == m_match_flags.end()){
                m_match_flags[tpEffTool->efficiencyFlag()] = false;
            }
        }
        InitTree (m_trees[cfdirs].first);
      }

  return StatusCode::SUCCESS;

}
 // fill the histos
 void MuonTPTreeTool::fill(Probe& probe,ToolHandle <IMuonTPSelectionTool> sel_tool){
    std::string cfdirs = m_efficiencyFlag+"/"+sel_tool->efficiencyFlag();
    auto tree = m_trees.find(cfdirs);
    if (tree == m_trees.end()){
        ATH_MSG_ERROR("Trying to fill a nonexisting tree!");
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
            m_match_flags[entry.first] = probe_map->second.at(entry.first);
        }
    }
    FillCustomStuff(probe);
    OutTree->Fill();

 }
void MuonTPTreeTool::FillCustomStuff(Probe & ){
    // do nothing in the default version
}
void MuonTPTreeTool::AddCustomBranches(TTree*){
    // do nothing in the default version
    // in 'your' version, do tree->Branch("something",&m_something); in here
}


void MuonTPTreeTool::updateMatch(Probe& probe,ToolHandle <IMuonTPEfficiencyTool> eff_tool){

    if (m_match_flags_perProbe.find(&probe) == m_match_flags_perProbe.end()){
        m_match_flags_perProbe[&probe] = std::map<std::string, bool>() ;
    }
    m_match_flags_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.isMatched();

}

void MuonTPTreeTool::ForgetKnownProbes(void){
    m_match_flags_perProbe.clear();
}

 
void MuonTPTreeTool::InitTree(TTree* tree){
    for (auto flag : m_match_flags){
        tree->Branch((std::string("matched_")+flag.first).c_str(), &(m_match_flags[flag.first]));
    }
    AddCustomBranches(tree);
}

  /// retrieve booked trees
  std::vector<std::pair<TTree*, std::string> > MuonTPTreeTool::retrieveBookedTrees(){
    std::vector<std::pair<TTree*, std::string> > OutTrees;
    for (auto tree: m_trees){
        OutTrees.push_back(tree.second);
    }
    return OutTrees;
  }

  MuonTPTreeTool::~MuonTPTreeTool(){

  }






