/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOPLOTS_H
#define PFOPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"

namespace PFO {

  class PFOPlots : public PlotBase {

  public:

    PFOPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName);

    void fill(const xAOD::PFO& PFO);
    void fill(const xAOD::FlowElement& FE);
  private:
    TH1* m_PFO_pt;
    TH1* m_PFO_eta;
    TH1* m_PFO_phi;
    TH1* m_PFO_m;
    TH1* m_PFO_charge;

    
    /** Pt Histogram binned in eta */
    TH1* m_PFO_pt_etaBinA;
    TH1* m_PFO_pt_etaBinB;
    TH1* m_PFO_pt_etaBinC;

    // Flow Element histograms
    TH1* m_FE_pt;
    TH1* m_FE_eta;
    TH1* m_FE_phi;
    TH1* m_FE_m;
    TH1* m_FE_charge;

    TH1* m_FE_eta_posE; // extra bin for positive energy FE
    
    /** Pt Histogram binned in eta */
    TH1* m_FE_pt_etaBinA;
    TH1* m_FE_pt_etaBinB;
    TH1* m_FE_pt_etaBinC;
    
    void initializePlots();
    std::string m_sPFOContainerName;
    std::string m_sFEContainerName;
  };

}
#endif
