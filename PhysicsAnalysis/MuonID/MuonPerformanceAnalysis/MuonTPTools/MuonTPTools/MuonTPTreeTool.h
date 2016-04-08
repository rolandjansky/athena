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
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "MuonTPTools/IMuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPEfficiencyTool.h"
#include "AsgTools/AsgTool.h"
#include "MuonEfficiencyCorrections/IMuonEfficiencyScaleFactors.h"
#include <map>

// base class for the plotting tools.
// takes care of most technical aspects
// you typically only need to overload AddPlots, in order to add the plots you like
// define the plot classes to add in the HistUtils package

class MuonTPTreeTool:
public asg::AsgTool,
  virtual public IMuonTPTreeTool {

 public:
  MuonTPTreeTool(std::string name);
  virtual ~MuonTPTreeTool();

  virtual StatusCode initialize();

  virtual StatusCode RegisterTrees (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools);

  virtual void updateMatch(Probe& probe, ToolHandle <IMuonTPEfficiencyTool> & eff_tool);
  virtual void fill(Probe& probe,ToolHandle <IMuonTPSelectionTool> & sel_tool);
  virtual void FillCustomStuff(Probe& probe);        // can override to fill custom branches
    
  /// retrieve booked histograms
  std::vector< std::pair<TTree*, std::string> > retrieveBookedTrees();

  virtual void FillTriggerInfo();
  virtual void FillTagTriggerInfo(Probe & probe);
  virtual void InitTree(TTree* tree);
  virtual void AddCustomBranches(TTree* tree);        // can override to add custom branches

  virtual void ForgetKnownProbes();
      
 protected:

  ToolHandle<Trig::TrigDecisionTool> m_trigTool;

  std::string m_efficiencyFlag;
        
  std::map<std::string, std::pair< TTree*, std::string> > m_trees;
        
  std::map<std::string, bool> m_match_flags;
  std::map<std::string, float> m_scale_factors;
  std::map<std::string, float> m_match_dR;
        
  std::map<std::string, bool> m_triggers;
  std::map<std::string, bool> m_tag_trigmatch;
  std::map<std::string, float> m_tag_trig_dR;
        
  std::map<Probe*, std::map<std::string, bool> > m_match_flags_perProbe;
  std::map<Probe*, std::map<std::string, float> > m_scale_factors_perProbe;
  std::map<Probe*, std::map<std::string, float> > m_match_dR_perProbe;
        
  bool m_record_SF;
  //         std::map<Probe*, std::map<std::string, bool> > m_tag_trigmatch_perProbe;
  //         std::map<Probe*, std::map<std::string, float> > m_tag_trig_dR_perProbe;
  };

#endif /* MUONTPTreeTool_H_ */
