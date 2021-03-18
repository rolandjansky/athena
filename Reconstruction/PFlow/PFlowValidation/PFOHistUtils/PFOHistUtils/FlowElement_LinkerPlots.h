/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FLOWELEMENT_LINKERPLOTS_H
#define FLOWELEMENT_LINKERPLOTS_H


#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/FlowElement.h"
#include <string>

namespace PFO{

  class FlowElement_LinkerPlots : public PlotBase {

  public:
    FlowElement_LinkerPlots(PlotBase *pParent, std::string sDir, std::string sFEContainerName, bool doNeutralFE);
//~FlowElement_LinkerPlots();    



    void fill(const xAOD::FlowElement& FE);

  private:


    TH1* m_CFE_tau_dR;
    TH1* m_CFE_tau_NMatchedTau;
    TH1* m_CFE_tau_tauNMatchedFE;
    
    TH1* m_NFE_tau_dR;
    TH1* m_NFE_tau_NMatchedTau;
    TH1* m_NFE_tau_tauNMatchedFE;
    
    
    TH1* m_CFE_muon_dR;
    TH1* m_CFE_muon_NMatchedMuon;
    TH1* m_CFE_muon_muonNMatchedFE;
    TH1* m_CFE_muon_largeDR_debug_author;
    TH1* m_CFE_muon_largeDR_debug_type;
    
    TH1* m_NFE_muon_dR;
    TH1* m_NFE_muon_NMatchedMuon;
    TH1* m_NFE_muon_muonNMatchedFE;
      
      
    TH1* m_CFE_photon_dR;
    TH1* m_CFE_photon_NMatchedPhoton;
    TH1* m_CFE_photon_photonNMatchedFE;

    TH1* m_NFE_photon_dR;
    TH1* m_NFE_photon_NMatchedPhoton;
    TH1* m_NFE_photon_photonNMatchedFE;


    TH1* m_CFE_electron_dR;
    TH1* m_CFE_electron_NMatchedElectron;
    TH1* m_CFE_electron_electronNMatchedFE;
    
    TH1* m_NFE_electron_dR;
    TH1* m_NFE_electron_NMatchedElectron;
    TH1* m_NFE_electron_electronNMatchedFE;

    void initializePlots();
    std::string m_sFEContainerName;
    bool m_doNeutralFE;
  };

}
#endif
