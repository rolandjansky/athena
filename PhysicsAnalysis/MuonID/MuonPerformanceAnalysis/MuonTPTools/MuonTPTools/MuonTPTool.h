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
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandleArray.h"

class IMuonTPSelectionTool;
class IMuonTPEfficiencyTool;

class MuonTPTool
: virtual public asg::AsgTool,
  virtual public IMuonTPTool {
  ASG_TOOL_CLASS(MuonTPTool, IMuonTPTool)
    
    public:  
  MuonTPTool(std::string myname);
  virtual ~MuonTPTool();
  
  virtual StatusCode initialize();
  
  /// run the tag-and-probe 
  void runTagAndProbe(const xAOD::MuonContainer*, const xAOD::IParticleContainer*, const xAOD::IParticleContainer*) const;      

  /// retrieve booked histograms
  std::vector<HistData> retrieveBookedHistograms();
  virtual void UpdateEfficiencies();
  
 private:

  ToolHandleArray<IMuonTPSelectionTool> m_muonTPSelectionTools;
  ToolHandleArray<IMuonTPEfficiencyTool> m_muonTPEfficiencyTools;
  std::string m_efficiencyFlag;

  ToolHandleArray<IMuonTPPlotTool> m_TPPlots;

  };

#endif
