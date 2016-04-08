/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonTPTool
//  ASG Tool performing tag-and-probe analysis
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonTPTool_H
#define MuonTPTool_H

#include "MuonTPTools/IMuonTPTool.h"
#include "MuonTPTools/IMuonTPPlotTool.h"
#include "MuonTPTools/IMuonTPTreeTool.h"
#include "AsgTools/AsgTool.h"
#include "TTree.h"
#include "AsgTools/ToolHandleArray.h"
class ITHistSvc;

class IMuonTPSelectionTool;
class IMuonTPEfficiencyTool;

class MuonTPTool
: public asg::AsgTool,
  virtual public IMuonTPTool {
  ASG_TOOL_CLASS(MuonTPTool, IMuonTPTool)
    
    public:  
  MuonTPTool(std::string myname);
  virtual ~MuonTPTool();
  
  virtual StatusCode initialize();
  
  /// run the tag-and-probe 
  void runTagAndProbe(const xAOD::MuonContainer*, const xAOD::IParticleContainer*, const xAOD::IParticleContainer*) ;      

  /// retrieve booked histograms
  std::vector<HistData> retrieveBookedHistograms();
  std::vector<std::pair < TTree*, std::string> > retrieveBookedTrees();
  std::vector<std::pair <TGraph*,  std::string> > retrieveBookedGraphs();
  virtual void UpdateEfficiencies();
  
 private:

  //ServiceHandle<ITHistSvc> m_histSvc;
  ToolHandleArray<IMuonTPSelectionTool> m_muonTPSelectionTools;
  ToolHandleArray<IMuonTPEfficiencyTool> m_muonTPEfficiencyTools;
  std::string m_efficiencyFlag;

  TTree* m_inefficient_evts;
  bool m_dump_inefficient;
  int m_evt;
  int m_run;
  bool m_centeta;
  ToolHandleArray<IMuonTPPlotTool> m_TPPlots;
  ToolHandleArray<IMuonTPTreeTool> m_TPTrees;
  

  };

#endif
