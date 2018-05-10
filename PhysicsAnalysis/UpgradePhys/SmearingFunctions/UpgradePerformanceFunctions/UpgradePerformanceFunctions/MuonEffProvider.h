/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_EFF_PROVIDER_H
#define MUON_EFF_PROVIDER_H

#include "AsgTools/AsgTool.h"       
#include "AsgTools/IAsgTool.h"
#include "AsgTools/AsgMessaging.h"
///////////////////////// -*- C++ -*- /////////////////////////////  

// Tool interfaces                              

#include "AsgTools/ToolHandle.h"

#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include <xAODEventInfo/EventInfo.h>
#include <xAODEventInfo/EventInfoContainer.h>
#include <xAODEventInfo/EventInfoAuxContainer.h>

// helper class for retrieving muon efficiencies.
// We use the standard CP tool for this to avoid reinventing the wheel. 
// Since the CP tool expects xAOD::Muon objects, we need an interface layer providing such objects to the tool

class MuonEffProvider : public asg::AsgMessaging {

public:
    // no arguments needed
    MuonEffProvider(const std::string& name);
    ~MuonEffProvider() {};

    // initialize. Argument is the efficiency file to read from, and the path in which to find it.
    bool initialize(const std::string & effi_dir, const std::string &  effi_file, bool useHighEtaTagger=true);
    // retrieve the efficiency. As always in the UPF, pt should be given in *MEV* 
    float getEfficiency (double pt, double eta, double phi=0);

protected:    
 // update the internal dummy xAOD event. Just calls setP4 - we don't use any additional information
    inline void updateMuon(double pt, double eta, double phi);  

    bool m_useMuonHighEta;

    // an instance of the CP tool 
    ToolHandle<CP::IMuonEfficiencyScaleFactors> m_reco_MESF;
    // our own dummy objects - the MESF CP tool needs xAOD objects
    xAOD::Muon* m_auxMuon;
    std::unique_ptr<xAOD::MuonContainer> m_auxMuoncont;
    std::unique_ptr<xAOD::MuonAuxContainer> m_auxMuonaux;
    xAOD::EventInfo* m_dummy_eventInfo;
    std::unique_ptr<xAOD::EventInfoAuxContainer> m_dummy_eventInfoAux;
    std::unique_ptr<xAOD::EventInfoContainer> m_dummy_eventInfoCont;

};

#endif // MUON_EFF_PROVIDER_H
