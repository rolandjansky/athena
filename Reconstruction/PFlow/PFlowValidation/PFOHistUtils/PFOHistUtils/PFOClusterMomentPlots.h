/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOCLUSTERMOMENTPLOTS_H
#define PFOCLUSTERMOMENTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEventInfo/EventInfo.h"

namespace PFO {

  class PFOClusterMomentPlots : public PlotBase {

  public:

    PFOClusterMomentPlots(PlotBase *pParent, std::string sDir, std::string sFEContainerName);
        
    void fill(const xAOD::FlowElement& FE, const xAOD::EventInfo& eventInfo);
  private:
    
    /** Histograms inclusive in eta */
    TH1* m_FE_SECOND_R;
    TH1* m_FE_CENTER_LAMBDA;
    TH1* m_FE_ISOLATION;
    TH1* m_FE_ENG_BAD_CELLS;
    TH1* m_FE_N_BAD_CELLS;
    TH1* m_FE_BADLARQ_FRAC;
    TH1* m_FE_ENG_POS;
    TH1* m_FE_AVG_LAR_Q;
    TH1* m_FE_AVG_TILE_Q;
    TH1* m_FE_EM_PROBABILITY;
    TH1* m_FE_SECOND_LAMBDA;

    /** Same histograms binned in eta regions */
    TH1* m_FE_SECOND_R_etaBinA;
    TH1* m_FE_CENTER_LAMBDA_etaBinA;
    TH1* m_FE_ISOLATION_etaBinA;
    TH1* m_FE_ENG_BAD_CELLS_etaBinA;
    TH1* m_FE_N_BAD_CELLS_etaBinA;
    TH1* m_FE_BADLARQ_FRAC_etaBinA;
    TH1* m_FE_ENG_POS_etaBinA;
    TH1* m_FE_AVG_LAR_Q_etaBinA;
    TH1* m_FE_AVG_TILE_Q_etaBinA;
    TH1* m_FE_EM_PROBABILITY_etaBinA;
    TH1* m_FE_SECOND_LAMBDA_etaBinA;

    TH1* m_FE_SECOND_R_etaBinB;
    TH1* m_FE_CENTER_LAMBDA_etaBinB;
    TH1* m_FE_ISOLATION_etaBinB;
    TH1* m_FE_ENG_BAD_CELLS_etaBinB;
    TH1* m_FE_N_BAD_CELLS_etaBinB;
    TH1* m_FE_BADLARQ_FRAC_etaBinB;
    TH1* m_FE_ENG_POS_etaBinB;
    TH1* m_FE_AVG_LAR_Q_etaBinB;
    TH1* m_FE_AVG_TILE_Q_etaBinB;
    TH1* m_FE_EM_PROBABILITY_etaBinB;
    TH1* m_FE_SECOND_LAMBDA_etaBinB;

    TH1* m_FE_SECOND_R_etaBinC;
    TH1* m_FE_CENTER_LAMBDA_etaBinC;
    TH1* m_FE_ISOLATION_etaBinC;
    TH1* m_FE_ENG_BAD_CELLS_etaBinC;
    TH1* m_FE_N_BAD_CELLS_etaBinC;
    TH1* m_FE_BADLARQ_FRAC_etaBinC;
    TH1* m_FE_ENG_POS_etaBinC;
    TH1* m_FE_AVG_LAR_Q_etaBinC;
    TH1* m_FE_AVG_TILE_Q_etaBinC;
    TH1* m_FE_EM_PROBABILITY_etaBinC;
    TH1* m_FE_SECOND_LAMBDA_etaBinC;

    TH1* m_FE_SECOND_R_etaBinD;
    TH1* m_FE_CENTER_LAMBDA_etaBinD;
    TH1* m_FE_ISOLATION_etaBinD;
    TH1* m_FE_ENG_BAD_CELLS_etaBinD;
    TH1* m_FE_N_BAD_CELLS_etaBinD;
    TH1* m_FE_BADLARQ_FRAC_etaBinD;
    TH1* m_FE_ENG_POS_etaBinD;
    TH1* m_FE_AVG_LAR_Q_etaBinD;
    TH1* m_FE_AVG_TILE_Q_etaBinD;
    TH1* m_FE_EM_PROBABILITY_etaBinD;
    TH1* m_FE_SECOND_LAMBDA_etaBinD;

    
    void initializePlots();    
    std::string m_sFEContainerName;
  };

}
#endif
