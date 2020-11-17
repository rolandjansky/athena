/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LEPTONFELINKERPLOTS_H
#define LEPTONFELINKERPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"
#include "xAODMuon/Muon.h"
#include "xAODTau/TauJet.h"

namespace PFO {

  class LeptonFELinkerPlots : public PlotBase {

  public:

    LeptonFELinkerPlots(PlotBase *pParent, std::string sDir, std::string LeptonContainerName, const bool& m_doNeutralFE, const int& m_LeptonID);
    
    void fill(const xAOD::Electron& el);
    void fill(const xAOD::Muon& muon);
    void fill(const xAOD::TauJet& tau);
    void fill(const xAOD::Photon& phot);


  private:
     
    TH1* m_electron_NMatchedCFE;
    TH1* m_electron_NMatchedNFE;
    
    TH1* m_muon_NMatchedCFE;
    TH1* m_muon_NMatchedNFE;
    
    TH1* m_tau_NMatchedCFE;
    TH1* m_tau_NMatchedNFE;
    
    TH1* m_photon_NMatchedCFE;
    TH1* m_photon_NMatchedNFE;
    
    void initializePlots();
    std::string m_LeptonContainerName;
    bool m_doNeutralFE;
    int  m_LeptonID;
  };

}
#endif
