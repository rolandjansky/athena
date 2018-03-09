/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOPVMATCHEDPLOTS_H
#define PFOPVMATCHEDPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODTracking/Vertex.h" 

namespace PFO {

  class PFOPVMatchedPlots : public PlotBase {

  public:

    PFOPVMatchedPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName);

    void fill(const xAOD::PFO& PFO, const xAOD::Vertex& theVertex);

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
    
    void initializePlots();
    std::string m_sPFOContainerName;

  };

}
#endif
