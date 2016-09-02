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
#include "MuonTPTools/MuonRecoTPEfficiencyTool.h"
#include <vector>

//========================================================================================================================
MuonTPTreeTool::MuonTPTreeTool(std::string name)
: AsgTool(name)
{
    // determines tree names
    declareProperty("EfficiencyFlag",         m_efficiencyFlag = "ZmmTP");
    // record scale factor info used for closure tests of the CP tools
    declareProperty("RecordScaleFactorInfo",  m_record_SF = false);
    declareProperty("IsRunOnDAOD",    m_is_on_DAOD=false);
    declareProperty("TriggerUtils",    m_trigUtils);
}

//========================================================================================================================
StatusCode MuonTPTreeTool::initialize()
{
    ATH_CHECK(m_trigUtils.retrieve());
    return StatusCode::SUCCESS;
}

//========================================================================================================================
StatusCode MuonTPTreeTool::RegisterTrees (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools)
{
    // Create histograms
    for(auto tpSelTool : probeTools) {
        if (tpSelTool->isNominal() || tpSelTool->notIncludedInNominal()){
            std::string cfdirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag();
            m_trees[cfdirs] = std::make_pair (new TTree ((std::string("TPTree_")+tpSelTool->efficiencyFlag()).c_str(),"TP"), cfdirs);

            // add tag trigger matching information
            for (auto trig : tpSelTool->tagTriggerList()) {
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

            //
            // Book trigger matching information for rerun triggers
            //
            for (auto trig : tpSelTool->tagTriggerListRerunMode()){
                ATH_MSG_INFO("Rerun tag trigger: " + trig);
                m_rerun_triggers[trig] = TrigInfo();
            }

            //
            // Book trigger matching information for observer triggers
            //
            for (const std::string &trig : tpSelTool->observerTriggerList("L1")){
                ATH_MSG_INFO("Add observer L1 trigger: " + trig);
                m_observer_triggers_L1[trig] = TrigInfo();
            }
            for (const std::string &trig : tpSelTool->observerTriggerList("HLT")){
                ATH_MSG_INFO("Add observer HLT trigger: " + trig);
                m_observer_triggers_HLT[trig] = TrigInfo();
            }

            for(auto tpEffTool : effTools) {
                if (m_match_flags.find(tpEffTool->efficiencyFlag()) == m_match_flags.end()){
                    m_match_flags[tpEffTool->efficiencyFlag()] = false;
                }
                if (dynamic_cast<MuonRecoTPEfficiencyTool*>(tpEffTool.get())){
                    m_reco_match_flags.insert(tpEffTool->efficiencyFlag());
                }
                if (tpEffTool->isNominal()){
                    if (m_match_dR.find(tpEffTool->efficiencyFlag()) == m_match_dR.end()){
                        m_match_dR[tpEffTool->efficiencyFlag()] = -1.0;
                    }
                    if (tpEffTool->doesApplySF()) {
                        if (m_scale_factors_trig.find(tpEffTool->efficiencyFlag()) == m_scale_factors_trig.end()){
                            m_scale_factors_trig[tpEffTool->efficiencyFlag()] = 1.0;
                        }
                        if (m_scale_factors_isol.find(tpEffTool->efficiencyFlag()) == m_scale_factors_isol.end()){
                            m_scale_factors_isol[tpEffTool->efficiencyFlag()] = 1.0;
                        }
                        if (m_scale_factors_reco.find(tpEffTool->efficiencyFlag()) == m_scale_factors_reco.end()){
                            m_scale_factors_reco[tpEffTool->efficiencyFlag()] = 1.0;
                        }
                    }
                }
                if (tpEffTool->triggerItem()!="") {
                    ATH_MSG_INFO("RegisterTrees - trigger item=" << tpEffTool->triggerItem() << " rerun mode=" << tpEffTool->rerunTriggerMode());

                    if (!tpEffTool->rerunTriggerMode()) {
                        if(m_triggers.find(tpEffTool->triggerItem()) == m_triggers.end())
                            m_triggers[tpEffTool->triggerItem()] = false;
                    }
                }
            }

            InitTree (m_trees[cfdirs].first, tpSelTool->probeHasCaloDeposits());
        }
    }

    return StatusCode::SUCCESS;
}

//========================================================================================================================
// fill the histos
void MuonTPTreeTool::fill(Probe& probe, ToolHandle <IMuonTPSelectionTool> & sel_tool)
{
    //
    // Reset variables for rerun and observer triggers
    //
    ResetObserverRerunTriggers();
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
        for (auto & entry: m_match_flags){
            entry.second = false;
        }
    }
    else {
        for (auto & entry: m_match_flags){
            //  skip double systematics cases
            if (probe_map->second.find(entry.first) ==  probe_map->second.end()) {
                m_match_flags[entry.first] = false;
            }
            else {
                m_match_flags[entry.first] = probe_map->second.at(entry.first);
            }
        }
    }
    auto probe_sf_map = m_scale_factors_trig_perProbe.find(&probe);
    if (probe_sf_map == m_scale_factors_trig_perProbe.end()){
        ATH_MSG_WARNING("Found a probe that doesn't have any SF info - intentional?");
        for (auto & entry: m_scale_factors_trig){
            entry.second = 1.00;
        }
    }
    else {
        for (auto & entry: m_scale_factors_trig){
            if (probe_sf_map->second.find(entry.first) ==  probe_sf_map->second.end()) {
                m_scale_factors_trig[entry.first] = 1.;
            }
            else {
                m_scale_factors_trig[entry.first] = probe_sf_map->second.at(entry.first);
            }
        }
    }
    probe_sf_map = m_scale_factors_reco_perProbe.find(&probe);
    if (probe_sf_map == m_scale_factors_reco_perProbe.end()){
        ATH_MSG_WARNING("Found a probe that doesn't have any SF info - intentional?");
        for (auto & entry: m_scale_factors_reco){
            entry.second = 1.00;
        }
    }
    else {
        for (auto & entry: m_scale_factors_reco){
            if (probe_sf_map->second.find(entry.first) ==  probe_sf_map->second.end()) {
                m_scale_factors_reco[entry.first] = 1.;
            }
            else {
                m_scale_factors_reco[entry.first] = probe_sf_map->second.at(entry.first);
            }
        }
    }
    probe_sf_map = m_scale_factors_isol_perProbe.find(&probe);
    if (probe_sf_map == m_scale_factors_isol_perProbe.end()){
        ATH_MSG_WARNING("Found a probe that doesn't have any SF info - intentional?");
        for (auto & entry: m_scale_factors_isol){
            entry.second = 1.00;
        }
    }
    else {
        for (auto & entry: m_scale_factors_isol){
            if (probe_sf_map->second.find(entry.first) ==  probe_sf_map->second.end()) {
                m_scale_factors_isol[entry.first] = 1.;
            }
            else {
                m_scale_factors_isol[entry.first] = probe_sf_map->second.at(entry.first);
            }
        }
    }
    auto probe_dR_map = m_match_dR_perProbe.find(&probe);
    if (probe_dR_map == m_match_dR_perProbe.end()){
        ATH_MSG_WARNING("Found a probe that doesn't have any dR info - intentional?");
        for (auto & entry: m_match_dR){
            entry.second = 1.00;
        }
    }
    else {
        for (auto & entry: m_match_dR){
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
    FillProbeTriggerInfo(probe);    
    ATH_MSG_DEBUG("Filling custom stuff");
    FillCustomStuff(probe);
    ATH_MSG_DEBUG("DONE filling custom stuff");
    if(sel_tool->probeHasCaloDeposits())
      FillCaloDeposits(probe);
    OutTree->Fill();
}

//========================================================================================================================
void MuonTPTreeTool::FillCaloDeposits(Probe& probe)
{    

  if(probe.probeTrack().isAvailable<float>("EMB1_dep")) m_EMB1_dep = probe.probeTrack().auxdata<float>("EMB1_dep");
  if(probe.probeTrack().isAvailable<float>("EMB1_eloss")) m_EMB1_eloss = probe.probeTrack().auxdata<float>("EMB1_eloss");
  if(probe.probeTrack().isAvailable<float>("EMB2_dep")) m_EMB2_dep = probe.probeTrack().auxdata<float>("EMB2_dep");
  if(probe.probeTrack().isAvailable<float>("EMB2_eloss")) m_EMB2_eloss = probe.probeTrack().auxdata<float>("EMB2_eloss");
  if(probe.probeTrack().isAvailable<float>("EMB3_dep")) m_EMB3_dep = probe.probeTrack().auxdata<float>("EMB3_dep");
  if(probe.probeTrack().isAvailable<float>("EMB3_eloss")) m_EMB3_eloss = probe.probeTrack().auxdata<float>("EMB3_eloss");
  if(probe.probeTrack().isAvailable<float>("EME1_dep")) m_EME1_dep = probe.probeTrack().auxdata<float>("EME1_dep");
  if(probe.probeTrack().isAvailable<float>("EME1_eloss")) m_EME1_eloss = probe.probeTrack().auxdata<float>("EME1_eloss");
  if(probe.probeTrack().isAvailable<float>("EME2_dep")) m_EME2_dep = probe.probeTrack().auxdata<float>("EME2_dep");
  if(probe.probeTrack().isAvailable<float>("EME2_eloss")) m_EME2_eloss = probe.probeTrack().auxdata<float>("EME2_eloss");
  if(probe.probeTrack().isAvailable<float>("EME3_dep")) m_EME3_dep = probe.probeTrack().auxdata<float>("EME3_dep");
  if(probe.probeTrack().isAvailable<float>("EME3_eloss")) m_EME3_eloss = probe.probeTrack().auxdata<float>("EME3_eloss");
  if(probe.probeTrack().isAvailable<float>("TileBar0_dep")) m_TileBar0_dep = probe.probeTrack().auxdata<float>("TileBar0_dep");
  if(probe.probeTrack().isAvailable<float>("TileBar0_eloss")) m_TileBar0_eloss = probe.probeTrack().auxdata<float>("TileBar0_eloss");
  if(probe.probeTrack().isAvailable<float>("TileBar1_dep")) m_TileBar1_dep = probe.probeTrack().auxdata<float>("TileBar1_dep");
  if(probe.probeTrack().isAvailable<float>("TileBar1_eloss")) m_TileBar1_eloss = probe.probeTrack().auxdata<float>("TileBar1_eloss");
  if(probe.probeTrack().isAvailable<float>("TileBar2_dep")) m_TileBar2_dep = probe.probeTrack().auxdata<float>("TileBar2_dep");
  if(probe.probeTrack().isAvailable<float>("TileBar2_eloss")) m_TileBar2_eloss = probe.probeTrack().auxdata<float>("TileBar2_eloss");
  if(probe.probeTrack().isAvailable<float>("TileExt0_dep")) m_TileExt0_dep = probe.probeTrack().auxdata<float>("TileExt0_dep");
  if(probe.probeTrack().isAvailable<float>("TileExt0_eloss")) m_TileExt0_eloss = probe.probeTrack().auxdata<float>("TileExt0_eloss");
  if(probe.probeTrack().isAvailable<float>("TileExt1_dep")) m_TileExt1_dep = probe.probeTrack().auxdata<float>("TileExt1_dep");
  if(probe.probeTrack().isAvailable<float>("TileExt1_eloss")) m_TileExt1_eloss = probe.probeTrack().auxdata<float>("TileExt1_eloss");
  if(probe.probeTrack().isAvailable<float>("TileExt2_dep")) m_TileExt2_dep = probe.probeTrack().auxdata<float>("TileExt2_dep");
  if(probe.probeTrack().isAvailable<float>("TileExt2_eloss")) m_TileExt2_eloss = probe.probeTrack().auxdata<float>("TileExt2_eloss");
  if(probe.probeTrack().isAvailable<float>("HEC0_dep")) m_HEC0_dep = probe.probeTrack().auxdata<float>("HEC0_dep");
  if(probe.probeTrack().isAvailable<float>("HEC0_eloss")) m_HEC0_eloss = probe.probeTrack().auxdata<float>("HEC0_eloss");
  if(probe.probeTrack().isAvailable<float>("HEC1_dep")) m_HEC1_dep = probe.probeTrack().auxdata<float>("HEC1_dep");
  if(probe.probeTrack().isAvailable<float>("HEC1_eloss")) m_HEC1_eloss = probe.probeTrack().auxdata<float>("HEC1_eloss");
  if(probe.probeTrack().isAvailable<float>("HEC2_dep")) m_HEC2_dep = probe.probeTrack().auxdata<float>("HEC2_dep");
  if(probe.probeTrack().isAvailable<float>("HEC2_eloss")) m_HEC2_eloss = probe.probeTrack().auxdata<float>("HEC2_eloss");
  if(probe.probeTrack().isAvailable<float>("HEC3_dep")) m_HEC3_dep = probe.probeTrack().auxdata<float>("HEC3_dep");
  if(probe.probeTrack().isAvailable<float>("HEC3_eloss")) m_HEC3_eloss = probe.probeTrack().auxdata<float>("HEC3_eloss");  
}

//========================================================================================================================
void MuonTPTreeTool::FillTriggerInfo()
{    
    for (auto & trig : m_triggers) {
        trig.second = m_trigUtils->TrigDecision(trig.first);
    }
    for (TrigInfoMap::value_type &trig : m_rerun_triggers) {
        trig.second.pass_trig = m_trigUtils->TrigDecision_RM(trig.first);
    }
    for (TrigInfoMap::value_type &trig : m_observer_triggers_L1) {
        trig.second.pass_trig = m_trigUtils->TrigDecision(trig.first);
    }
    for (TrigInfoMap::value_type &trig : m_observer_triggers_HLT) {
        trig.second.pass_trig = m_trigUtils->TrigDecision(trig.first);
    }
}

//========================================================================================================================
void MuonTPTreeTool::FillTagTriggerInfo(Probe &probe)
{    
    const xAOD::Muon* tagmuon = dynamic_cast <const xAOD::Muon*>(&(probe.tagTrack()));
    // tag is not a muon: happens for truth probes
    if (!tagmuon) return;

    // triggers from online menu
    for (auto & trigstatus : m_tag_trigmatch){
        std::string trigkey = trigstatus.first;
        m_tag_trigmatch[trigkey] = m_trigUtils->Trig_Match(*tagmuon, trigkey);
    }
    for (auto & trigstatus : m_tag_trig_dR){
        std::string trigkey = trigstatus.first;
        m_tag_trig_dR[trigkey] = m_trigUtils->Trig_DR(*tagmuon, trigkey);
    }

    for(TrigInfoMap::value_type &trig: m_rerun_triggers) {
        trig.second.match_tag_dr = m_trigUtils->Trig_DR(*tagmuon, trig.first, 0.6);
    }
    for(TrigInfoMap::value_type &trig: m_observer_triggers_L1) {
        trig.second.match_tag_dr = m_trigUtils->Trig_DR(*tagmuon, trig.first, 0.6);
    }
    for(TrigInfoMap::value_type &trig: m_observer_triggers_HLT) {
        trig.second.match_tag_dr = m_trigUtils->Trig_DR(*tagmuon, trig.first, 0.6);
    }
}

//========================================================================================================================
void MuonTPTreeTool::FillProbeTriggerInfo(Probe & probe)
{    
    const xAOD::Muon* probe_muon = dynamic_cast <const xAOD::Muon*>(&(probe.probeTrack()));

    // tag is not a muon: happens for truth probes
    if (!probe_muon) {
        return;
    }

    for(TrigInfoMap::value_type &trig: m_rerun_triggers) {
        trig.second.match_probe_dr = m_trigUtils->Trig_DR(*probe_muon, trig.first, 0.6);
    }
    for(TrigInfoMap::value_type &trig: m_observer_triggers_L1) {
        trig.second.match_probe_dr = m_trigUtils->Trig_DR(*probe_muon, trig.first, 0.6);
    }
    for(TrigInfoMap::value_type &trig: m_observer_triggers_HLT) {
        trig.second.match_probe_dr = m_trigUtils->Trig_DR(*probe_muon, trig.first, 0.6);
    }
}

//========================================================================================================================
void MuonTPTreeTool::FillCustomStuff(Probe & ){
    // do nothing in the default version
}

//========================================================================================================================
void MuonTPTreeTool::AddCustomBranches(TTree*){
    // do nothing in the default version
    // in 'your' version, do tree->Branch("something",&m_something); in here
}

//========================================================================================================================
void MuonTPTreeTool::updateMatch(Probe& probe,ToolHandle <IMuonTPEfficiencyTool> & eff_tool)
{
    if (m_match_flags_perProbe.find(&probe) == m_match_flags_perProbe.end()){
        m_match_flags_perProbe[&probe] = std::map<std::string, bool>() ;
    }
    if (m_scale_factors_isol_perProbe.find(&probe) == m_scale_factors_isol_perProbe.end()){
        m_scale_factors_isol_perProbe[&probe] = std::map<std::string, float>() ;
    }
    if (m_scale_factors_trig_perProbe.find(&probe) == m_scale_factors_trig_perProbe.end()){
        m_scale_factors_trig_perProbe[&probe] = std::map<std::string, float>() ;
    }
    if (m_scale_factors_reco_perProbe.find(&probe) == m_scale_factors_reco_perProbe.end()){
        m_scale_factors_reco_perProbe[&probe] = std::map<std::string, float>() ;
    }
    if (m_match_dR_perProbe.find(&probe) == m_match_dR_perProbe.end()){
        m_match_dR_perProbe[&probe] = std::map<std::string, float>() ;
    }
    m_match_flags_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.isMatched();
    m_scale_factors_reco_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.sf_reco();
    m_scale_factors_isol_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.sf_isol();
    m_scale_factors_trig_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.sf_trig();
    m_match_dR_perProbe[&probe][eff_tool->efficiencyFlag()] = probe.dr_match();

}

//========================================================================================================================
void MuonTPTreeTool::ForgetKnownProbes(void)
{
    m_match_flags_perProbe.clear();
    m_match_dR_perProbe.clear();
    m_scale_factors_isol_perProbe.clear();
    m_scale_factors_trig_perProbe.clear();
    m_scale_factors_reco_perProbe.clear();
}

//========================================================================================================================
void MuonTPTreeTool::ResetObserverRerunTriggers()
{
    for(TrigInfoMap::value_type &trig: m_rerun_triggers) {
        trig.second.ResetTrigInfo();
    }
    for(TrigInfoMap::value_type &trig: m_observer_triggers_L1) {
        trig.second.ResetTrigInfo();
    }
    for(TrigInfoMap::value_type &trig: m_observer_triggers_HLT) {
        trig.second.ResetTrigInfo();
    }
}

//========================================================================================================================
void MuonTPTreeTool::InitTree(TTree* tree, bool probeHasCaloDeposits)
{
    // fill the match flags
    for (auto & flag : m_match_flags){
        tree->Branch((std::string("probe_matched_")+flag.first).c_str(), &flag.second);
    }

    if (m_record_SF) {
        for (auto & flag :  m_scale_factors_reco) {
            tree->Branch((std::string("probe_scale_factor_reco_")+flag.first).c_str(), &flag.second);
        }
        for (auto & flag :  m_scale_factors_trig) {
            tree->Branch((std::string("probe_scale_factor_trig_")+flag.first).c_str(), &flag.second);
        }
        for (auto & flag :  m_scale_factors_isol) {
            tree->Branch((std::string("probe_scale_factor_isol_")+flag.first).c_str(), &flag.second);
        }
    }
    for (auto & flag : m_match_dR){
        tree->Branch((std::string("probe_dRMatch_")+flag.first).c_str(), &flag.second);
    }

    // also fill in the tag trigger info
    for (auto & flag : m_tag_trigmatch){
        tree->Branch((std::string("tag_matched_")+flag.first).c_str(), &flag.second);
    }
    for (auto & flag : m_tag_trig_dR){
        tree->Branch((std::string("tag_dRMatch_")+flag.first).c_str(), &flag.second);
    }
    // add branches for event triggers
    for (auto & flag : m_triggers) {
        tree->Branch((std::string("trig_")+flag.first).c_str(), &flag.second);
    }

    //
    // Book branches for rerun and observer triggers
    //
    for(TrigInfoMap::value_type &trig : m_rerun_triggers) {
        tree->Branch(std::string("trig_"         +trig.first+"_RM").c_str(), &trig.second.pass_trig);
        tree->Branch(std::string("probe_dRMatch_"+trig.first+"_RM").c_str(), &trig.second.match_probe_dr);
        tree->Branch(std::string("tag_dRMatch_"  +trig.first+"_RM").c_str(), &trig.second.match_tag_dr);
    }

    for(TrigInfoMap::value_type &trig : m_observer_triggers_L1) {
        tree->Branch(std::string("trig_"         +trig.first+"_OBSR"    ).c_str(), &trig.second.pass_trig);
        tree->Branch(std::string("probe_dRMatch_"+trig.first+"_OBSR"    ).c_str(), &trig.second.match_probe_dr);
        tree->Branch(std::string("tag_dRMatch_"  +trig.first+"_OBSR"    ).c_str(), &trig.second.match_tag_dr);
    }

    for(TrigInfoMap::value_type &trig : m_observer_triggers_HLT) {
        tree->Branch(std::string("trig_"         +trig.first+"_OBSR").c_str(), &trig.second.pass_trig);
        tree->Branch(std::string("probe_dRMatch_"+trig.first+"_OBSR").c_str(), &trig.second.match_probe_dr);
        tree->Branch(std::string("tag_dRMatch_"  +trig.first+"_OBSR").c_str(), &trig.second.match_tag_dr);
    }

    if(probeHasCaloDeposits) {
      tree->Branch("probe_calo_EMB1_dep", &m_EMB1_dep);
      tree->Branch("probe_calo_EMB1_eloss", &m_EMB1_eloss);
      tree->Branch("probe_calo_EMB2_dep", &m_EMB2_dep);
      tree->Branch("probe_calo_EMB2_eloss", &m_EMB2_eloss);
      tree->Branch("probe_calo_EMB3_dep", &m_EMB3_dep);
      tree->Branch("probe_calo_EMB3_eloss", &m_EMB3_eloss);
      tree->Branch("probe_calo_EME1_dep", &m_EME1_dep);
      tree->Branch("probe_calo_EME1_eloss", &m_EME1_eloss);
      tree->Branch("probe_calo_EME2_dep", &m_EME2_dep);
      tree->Branch("probe_calo_EME2_eloss", &m_EME2_eloss);
      tree->Branch("probe_calo_EME3_dep", &m_EME3_dep);
      tree->Branch("probe_calo_EME3_eloss", &m_EME3_eloss);
      tree->Branch("probe_calo_TileBar0_dep", &m_TileBar0_dep);
      tree->Branch("probe_calo_TileBar0_eloss", &m_TileBar0_eloss);
      tree->Branch("probe_calo_TileBar1_dep", &m_TileBar1_dep);
      tree->Branch("probe_calo_TileBar1_eloss", &m_TileBar1_eloss);
      tree->Branch("probe_calo_TileBar2_dep", &m_TileBar2_dep);
      tree->Branch("probe_calo_TileBar2_eloss", &m_TileBar2_eloss);
      tree->Branch("probe_calo_TileExt0_dep", &m_TileExt0_dep);
      tree->Branch("probe_calo_TileExt0_eloss", &m_TileExt0_eloss);
      tree->Branch("probe_calo_TileExt1_dep", &m_TileExt1_dep);
      tree->Branch("probe_calo_TileExt1_eloss", &m_TileExt1_eloss);
      tree->Branch("probe_calo_TileExt2_dep", &m_TileExt2_dep);
      tree->Branch("probe_calo_TileExt2_eloss", &m_TileExt2_eloss);
      tree->Branch("probe_calo_HEC0_dep", &m_HEC0_dep);
      tree->Branch("probe_calo_HEC0_eloss", &m_HEC0_eloss);
      tree->Branch("probe_calo_HEC1_dep", &m_HEC1_dep);
      tree->Branch("probe_calo_HEC1_eloss", &m_HEC1_eloss);
      tree->Branch("probe_calo_HEC2_dep", &m_HEC2_dep);
      tree->Branch("probe_calo_HEC2_eloss", &m_HEC2_eloss);
      tree->Branch("probe_calo_HEC3_dep", &m_HEC3_dep);
      tree->Branch("probe_calo_HEC3_eloss", &m_HEC3_eloss);      
    }
    
    AddCustomBranches(tree);
}

//========================================================================================================================
/// retrieve booked trees
std::vector<std::pair<TTree*, std::string> > MuonTPTreeTool::retrieveBookedTrees()
{
    std::vector<std::pair<TTree*, std::string> > OutTrees;
    for (auto & tree: m_trees){
        OutTrees.push_back(tree.second);
    }
    return OutTrees;
}







