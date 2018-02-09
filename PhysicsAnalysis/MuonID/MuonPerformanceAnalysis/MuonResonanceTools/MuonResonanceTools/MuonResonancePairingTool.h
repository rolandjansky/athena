/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonResonanceSelectionTool
//  ASG Tool performing tag and probe selection
//
//  (c) ATLAS Muon Combined Performance Software
//////////////////////////////////////////////////////////////////////////////
#ifndef MuonResonancePairingTool_H
#define MuonResonancePairingTool_H

#include "MuonResonanceTools/IMuonResonancePairingTool.h"
#include "AsgTools/AsgTool.h"

class MuonResonancePairingTool
: virtual public asg::AsgTool,
  virtual public IMuonResonancePairingTool {
  ASG_TOOL_CLASS(MuonResonancePairingTool, IMuonResonancePairingTool)
    
public:

  MuonResonancePairingTool(std::string myname);

  virtual StatusCode initialize();

 
  //std::vector<std::pair<xAOD::Muon*, xAOD::Muon*> >  buildPairs(std::pair<xAOD::MuonContainer*,xAOD::MuonContainer*>) const;
  std::vector<std::pair<const xAOD::Muon*, const xAOD::Muon*> > buildPairs(std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> >) const;

 private:
  
  double m_lowMassWindow;
  double m_highMassWindow;
  
};

#endif
