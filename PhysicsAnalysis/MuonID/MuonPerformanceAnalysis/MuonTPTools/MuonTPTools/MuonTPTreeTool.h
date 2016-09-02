/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPPlots.h
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#ifndef MUONTPTreeTool_H_
#define MUONTPTreeTool_H_
#include "MuonTPTools/IMuonTPTreeTool.h"
#include "MuonTPTools/IMuonTPTrigUtils.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "AsgTools/AsgTool.h"
#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include <map>
#include <unordered_set>

// base class for the plotting tools.
// takes care of most technical aspects
// you typically only need to overload AddPlots, in order to add the plots you like
// define the plot classes to add in the HistUtils package

class MuonTPTreeTool:
public asg::AsgTool,
  virtual public IMuonTPTreeTool {

 public:
  MuonTPTreeTool(std::string name);
  virtual ~MuonTPTreeTool() {}

  virtual StatusCode initialize();

  virtual StatusCode RegisterTrees (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools);

  virtual void updateMatch(Probe& probe, ToolHandle <IMuonTPEfficiencyTool> & eff_tool);
  virtual void fill(Probe& probe,ToolHandle <IMuonTPSelectionTool> & sel_tool);
  virtual void FillCustomStuff(Probe& probe);        // can override to fill custom branches
    
  /// retrieve booked histograms
  std::vector< std::pair<TTree*, std::string> > retrieveBookedTrees();

  virtual void FillTriggerInfo();
  virtual void FillTagTriggerInfo  (Probe &probe);
  virtual void FillProbeTriggerInfo(Probe &probe);
  virtual void FillCaloDeposits(Probe& probe);

  virtual void InitTree(TTree* tree, bool);
  virtual void AddCustomBranches(TTree* tree);        // can override to add custom branches

  virtual void ForgetKnownProbes();
      
  virtual void ResetObserverRerunTriggers();

 protected:

  struct TrigInfo 
  {
  TrigInfo(): pass_trig(false), match_tag_dr(10.0), match_probe_dr(10.0) {}
    bool  pass_trig;
    float match_tag_dr;
    float match_probe_dr;
    
    void ResetTrigInfo() 
    {
      pass_trig      = false;
      match_tag_dr   = 10.0;
      match_probe_dr = 10.0;
    }
  };

  typedef std::map<std::string, TrigInfo> TrigInfoMap;

 protected:

  ToolHandle<IMuonTPTrigUtils> m_trigUtils;

  std::string m_efficiencyFlag;
        
  std::map<std::string, std::pair< TTree*, std::string> > m_trees;
        
  std::map<std::string, bool>  m_match_flags;
  std::map<std::string, float> m_scale_factors_reco;
  std::map<std::string, float> m_scale_factors_trig;
  std::map<std::string, float> m_scale_factors_isol;
  std::map<std::string, float> m_match_dR;
        
  std::map<std::string, bool>  m_triggers;
  std::map<std::string, bool>  m_tag_trigmatch;
  std::map<std::string, float> m_tag_trig_dR;
  // this is used to identify the cases where a object different from the probe is matched to the pobe, e.g. reco measurements
  std::unordered_set<std::string>  m_reco_match_flags;

  TrigInfoMap                  m_rerun_triggers;
  TrigInfoMap                  m_observer_triggers_L1;
  TrigInfoMap                  m_observer_triggers_HLT;

  std::map<Probe*, std::map<std::string, bool> >  m_match_flags_perProbe;
  std::map<Probe*, std::map<std::string, float> > m_scale_factors_reco_perProbe;
  std::map<Probe*, std::map<std::string, float> > m_scale_factors_trig_perProbe;
  std::map<Probe*, std::map<std::string, float> > m_scale_factors_isol_perProbe;
  std::map<Probe*, std::map<std::string, float> > m_match_dR_perProbe;
        
  bool m_is_on_DAOD;	// disables stuff which we do not have in the derivations

  bool m_record_SF;
  //         std::map<Probe*, std::map<std::string, bool> > m_tag_trigmatch_perProbe;
  //         std::map<Probe*, std::map<std::string, float> > m_tag_trig_dR_perProbe;

  float m_EMB1_dep;
  float m_EMB1_eloss;
  float m_EMB2_dep;
  float m_EMB2_eloss;
  float m_EMB3_dep;
  float m_EMB3_eloss;
  float m_EME1_dep;
  float m_EME1_eloss;
  float m_EME2_dep;
  float m_EME2_eloss;
  float m_EME3_dep;
  float m_EME3_eloss;
  float m_TileBar0_dep;
  float m_TileBar0_eloss;
  float m_TileBar1_dep;
  float m_TileBar1_eloss;
  float m_TileBar2_dep;
  float m_TileBar2_eloss;
  float m_TileExt0_dep;
  float m_TileExt0_eloss;
  float m_TileExt1_dep;
  float m_TileExt1_eloss;
  float m_TileExt2_dep;
  float m_TileExt2_eloss;
  float m_HEC0_dep;
  float m_HEC0_eloss;
  float m_HEC1_dep;
  float m_HEC1_eloss;
  float m_HEC2_dep;
  float m_HEC2_eloss;
  float m_HEC3_dep;
  float m_HEC3_eloss;
  };

#endif /* MUONTPTreeTool_H_ */
