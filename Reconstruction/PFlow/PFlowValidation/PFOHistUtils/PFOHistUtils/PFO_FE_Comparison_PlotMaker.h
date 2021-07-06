/*                                                                                                           
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration                                    
*/
// Author: M. T. Anthony
#ifndef PFO_FE_COMPARISON_PLOTMAKER_H
#define PFO_FE_COMPARISON_PLOTMAKER_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
// Local include(s) for msg:
#include "AsgMessaging/AsgMessaging.h"

namespace PFO {
  class PFO_FE_Comparison_PlotMaker: public PlotBase, public asg::AsgMessaging {
  public:
    PFO_FE_Comparison_PlotMaker(PlotBase* pParent,std::string sDir, std::string sPFOContainerName, std::string sFEContainerName, bool m_isNeutral=false); // constructor
    void Fill(const xAOD::PFO* pfo, const xAOD::FlowElement* fe);

  private:    
    TH1* m_PFO_FE_ptratio;
    TH1* m_PFO_FE_delta_pt;
    TH1* m_PFO_FE_delta_R;
    TH1* m_PFO_FE_delta_eta;
    TH1* m_PFO_FE_delta_phi;
    TH1* m_PFO_FE_dE;
    
    //cluster/neutral flow element observables
    TH1* m_PFO_FE_SECOND_R_RelComparison;
    TH1* m_PFO_FE_CENTER_LAMBDA_RelComparison;
    TH1* m_PFO_FE_ISOLATION_RelComparison;
    TH1* m_PFO_FE_ENG_BAD_CELLS_RelComparison;
    TH1* m_PFO_FE_N_BAD_CELLS_RelComparison;
    TH1* m_PFO_FE_BADLARQ_FRAC_RelComparison;
    TH1* m_PFO_FE_ENG_POS_RelComparison;
    TH1* m_PFO_FE_AVG_LAR_Q_RelComparison;
    TH1* m_PFO_FE_AVG_TILE_Q_RelComparison;
    TH1* m_PFO_FE_EM_PROBABILITY_RelComparison;
    TH1* m_PFO_FE_SECOND_LAMBDA_RelComparison;

    TH1* m_PFO_FE_TIMING_RelComparison;
    
    TH1* m_PFO_FE_isInDenseEnvironment_RelComparison;
    TH1* m_PFO_FE_tracksExpectedEnergyDeposit;    


    void initializePlots();

    std::string m_sPFOContainerName;
    std::string m_sFEContainerName;
    bool m_isNeutral; // toggle for Neutral Flow Element/ Charged Flow Element plots
  };// end of classdef
} //end of namespace


#endif
