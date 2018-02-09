/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonResonanceTool
//  ASG Tool performing muon resonance analysis
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonResonanceTool_H
#define MuonResonanceTool_H

#define MURESETABINS 7
#define TRACKBINS 3
#define SYSBINS 11

#include "MuonResonanceTools/IMuonResonanceTool.h"
#include "AsgTools/AsgTool.h"
#ifndef ROOTCORE
#include "GaudiKernel/ToolHandle.h"
#endif


class IMuonResonanceSelectionTool;
class IMuonResonancePairingTool;

class MuonResonanceTool
: virtual public asg::AsgTool,
  virtual public IMuonResonanceTool {
  ASG_TOOL_CLASS(MuonResonanceTool, IMuonResonanceTool)
    
    public:  
  MuonResonanceTool(std::string myname);
  virtual ~MuonResonanceTool();
  
  virtual StatusCode initialize();
  
  /// find the resonances 
  void findResonance(const xAOD::MuonContainer* , bool isMC) const;
  TString tp_types[TRACKBINS] = {"CB", "ID", "ME"};
  int NSys = SYSBINS; 
  
  /// retrieve booked histograms
  std::vector< HistData > retrieveBookedHistograms();
  

 private:

  bool m_doSys;
  bool m_PlotsInit = false;
  std::vector<std::string> m_sysnames;
  std::string m_histoprefix;
  std::map<std::string, std::vector<double> > m_binning;

  ToolHandle<IMuonResonanceSelectionTool> m_muonResonanceSelectionTool;
  ToolHandle<IMuonResonancePairingTool>   m_muonResonancePairingTool;

  MuonResonancePlots* m_resonancePlots[MURESETABINS][TRACKBINS][SYSBINS];
  std::vector< CP::SystematicSet > getSystematics() const;
  std::vector< std::string > getSysNames() const;

  bool hasTriggerMatch(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd) const;

  int EtaBin(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd) const;
  int AuthorBin(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd) const;

  };

#endif
