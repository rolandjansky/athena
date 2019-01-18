/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOCALIBHITCLUSTERMOMENTPLOTS_H
#define PFOCALIBHITCLUSTERMOMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"

namespace PFO {

  class PFOCalibHitClusterMomentPlots : public PlotBase {

  public:

    PFOCalibHitClusterMomentPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName);
    
    void fill(const xAOD::PFO& PFO);
    
  private:

    /** Histograms inclusive in eta */
    TH1* m_PFO_ENG_CALIB_FRAC_EM = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_REST = nullptr;

    /** Same histograms binned in eta regions */
    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinA = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinA = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinA = nullptr;

    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinB = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinB = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinB = nullptr;

    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinC = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinC = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinC = nullptr;

    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinD = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinD = nullptr;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinD = nullptr;
    
    void initializePlots();
    std::string m_sPFOContainerName;

  };

}
#endif
