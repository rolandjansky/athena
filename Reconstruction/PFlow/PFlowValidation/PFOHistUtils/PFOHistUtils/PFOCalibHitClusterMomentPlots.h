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
    TH1* m_PFO_ENG_CALIB_FRAC_EM;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD;
    TH1* m_PFO_ENG_CALIB_FRAC_REST;

    /** Same histograms binned in eta regions */
    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinA;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinA;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinA;

    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinB;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinB;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinB;

    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinC;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinC;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinC;

    TH1* m_PFO_ENG_CALIB_FRAC_EM_etaBinD;
    TH1* m_PFO_ENG_CALIB_FRAC_HAD_etaBinD;
    TH1* m_PFO_ENG_CALIB_FRAC_REST_etaBinD;
    
    void initializePlots();
    std::string m_sPFOContainerName;

  };

}
#endif
