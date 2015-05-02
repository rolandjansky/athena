/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IMuonTPTool.h

#ifndef IMuonTPTool_H
#define IMuonTPTool_H

#include "AsgTools/IAsgTool.h"

#include "TTree.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "MuonPerformanceHistUtils/MuonTPEfficiencyPlots.h"

class IMuonTPTool : virtual public asg::IAsgTool {
  ASG_TOOL_INTERFACE(IMuonTPTool)

public:

    /// run the tag-and-probe 
    virtual void runTagAndProbe(const xAOD::MuonContainer*, const xAOD::IParticleContainer*, const xAOD::IParticleContainer*) =0;

    /// retrieve booked histograms
    virtual std::vector<HistData> retrieveBookedHistograms() = 0;
    virtual std::vector<std::pair < TTree*, std::string> > retrieveBookedTrees() = 0;
    ///Retrieve all booked graphs
    virtual std::vector<std::pair <TGraph*,  std::string> > retrieveBookedGraphs() = 0;
    virtual void UpdateEfficiencies() = 0;

};

#endif
