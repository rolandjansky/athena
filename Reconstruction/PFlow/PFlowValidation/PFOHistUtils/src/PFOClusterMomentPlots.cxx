/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOClusterMomentPlots.h"

namespace PFO {

  PFOClusterMomentPlots::PFOClusterMomentPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_LATERAL = nullptr;
    m_PFO_LONGITUDINAL = nullptr;
    m_PFO_SECOND_R = nullptr;
    m_PFO_CENTER_LAMBDA = nullptr;
    m_PFO_FIRST_ENG_DENS = nullptr;
    m_PFO_ENG_FRAC_MAX = nullptr;
    m_PFO_ISOLATION = nullptr;
    m_PFO_ENG_BAD_CELLS = nullptr;
    m_PFO_N_BAD_CELLS = nullptr;
    m_PFO_BADLARQ_FRAC = nullptr;
    m_PFO_ENG_POS = nullptr;
    m_PFO_SIGNIFICANCE = nullptr;
    m_PFO_AVG_LAR_Q = nullptr;
    m_PFO_AVG_TILE_Q = nullptr;
}

  void PFOClusterMomentPlots::initializePlots(){

    m_PFO_LATERAL = Book1D("PFO_LATERAL",m_sPFOContainerName + "_LATERAL",60,-1.0,2.0);
    m_PFO_LONGITUDINAL = Book1D("PFO_LONGITUDINAL",m_sPFOContainerName + "_LONGITUDINAL",60,-1.0,2.0);
    m_PFO_SECOND_R = Book1D("PFO_SECOND_R",m_sPFOContainerName + "_SECOND_R",60,-1.0,50.0); 
    m_PFO_CENTER_LAMBDA = Book1D("PFO_CENTER_LAMBDA",m_sPFOContainerName + "_CENTER_LAMBDA",60,-1.0,3000.0);
    m_PFO_FIRST_ENG_DENS = Book1D("PFO_FIRST_ENG_DENS",m_sPFOContainerName + "_FIRST_ENG_DENS",60,-1.0,2.0);
    m_PFO_ENG_FRAC_MAX = Book1D("PFO_ENG_FRAC_MAX",m_sPFOContainerName + "_ENG_FRAC_MAX",60,-1.0,2.0);
    m_PFO_ISOLATION = Book1D("PFO_ISOLATION",m_sPFOContainerName + "_ISOLATION",60,-1.0,2.0);
    m_PFO_ENG_BAD_CELLS = Book1D("PFO_ENG_BAD_CELLS",m_sPFOContainerName + "_ENG_BAD_CELLS",120,-1.0,100.0);
    m_PFO_N_BAD_CELLS = Book1D("PFO_N_BAD_CELLS",m_sPFOContainerName + "_N_BAD_CELLS",120,-1.0,20.0);
    m_PFO_BADLARQ_FRAC = Book1D("PFO_BADLARQ_FRAC",m_sPFOContainerName + "_BADLARQ_FRAC",140,-3.0,5.0);
    m_PFO_ENG_POS = Book1D("PFO_ENG_POS",m_sPFOContainerName + "_ENG_POS",60,-100.0,10000.0);
    m_PFO_SIGNIFICANCE = Book1D("PFO_SIGNIFICANCE",m_sPFOContainerName + "_SIGNIFICANCE",300,-20.0,20.0);
    m_PFO_AVG_LAR_Q = Book1D("PFO_AVG_LAR_Q",m_sPFOContainerName + "_AVG_LAR_Q",240,-1.0,200.0);
    m_PFO_AVG_TILE_Q = Book1D("PFO_AVG_TILE_Q",m_sPFOContainerName + "_AVG_TILE_Q",240,-1.0,200.0);

  }

  void PFOClusterMomentPlots::fill(const xAOD::PFO& PFO){

    float moment_LATERAL = 0.0;
    bool gotMoment = PFO.getClusterMoment(moment_LATERAL,xAOD::CaloCluster::LATERAL);
    if (true == gotMoment) m_PFO_LATERAL->Fill(moment_LATERAL);
    else m_PFO_LATERAL->Fill(-1.0);

    float moment_LONGITUDINAL = 0.0;
    gotMoment = PFO.getClusterMoment(moment_LONGITUDINAL,xAOD::CaloCluster::LONGITUDINAL);
    if (true == gotMoment) m_PFO_LONGITUDINAL->Fill(moment_LONGITUDINAL);
    else m_PFO_LONGITUDINAL->Fill(-1.0);
    
    float moment_SECOND_R = 0.0;
    gotMoment = PFO.getClusterMoment(moment_SECOND_R,xAOD::CaloCluster::SECOND_R);
    if (true == gotMoment) m_PFO_SECOND_R->Fill(moment_SECOND_R);
    else m_PFO_SECOND_R->Fill(-1.0);

    float moment_CENTER_LAMBDA = 0.0;
    gotMoment = PFO.getClusterMoment(moment_CENTER_LAMBDA,xAOD::CaloCluster::CENTER_LAMBDA);
    if (true == gotMoment) m_PFO_CENTER_LAMBDA->Fill(moment_CENTER_LAMBDA);
    else m_PFO_CENTER_LAMBDA->Fill(-1.0);
    
    float moment_FIRST_ENG_DENS = 0.0;
    gotMoment = PFO.getClusterMoment(moment_FIRST_ENG_DENS,xAOD::CaloCluster::FIRST_ENG_DENS);
    if (true == gotMoment) m_PFO_FIRST_ENG_DENS->Fill(moment_FIRST_ENG_DENS);
    else m_PFO_FIRST_ENG_DENS->Fill(-1.0);

    float moment_ENG_FRAC_MAX = 0.0;
    gotMoment = PFO.getClusterMoment(moment_ENG_FRAC_MAX,xAOD::CaloCluster::ENG_FRAC_MAX);
    if (true == gotMoment) m_PFO_ENG_FRAC_MAX->Fill(moment_ENG_FRAC_MAX);
    else m_PFO_ENG_FRAC_MAX->Fill(-1.0);

    float moment_ISOLATION = 0.0;
    gotMoment = PFO.getClusterMoment(moment_ISOLATION,xAOD::CaloCluster::ISOLATION);
    if (true == gotMoment) m_PFO_ISOLATION->Fill(moment_ISOLATION);
    else m_PFO_ISOLATION->Fill(-1.0);

    float moment_ENG_BAD_CELLS = 0.0;
    gotMoment = PFO.getClusterMoment(moment_ENG_BAD_CELLS,xAOD::CaloCluster::ENG_BAD_CELLS);
    if (true == gotMoment) m_PFO_ENG_BAD_CELLS->Fill(moment_ENG_BAD_CELLS);
    else m_PFO_ENG_BAD_CELLS->Fill(-1.0);

    float moment_N_BAD_CELLS = 0.0;
    gotMoment = PFO.getClusterMoment(moment_N_BAD_CELLS,xAOD::CaloCluster::N_BAD_CELLS);
    if (true == gotMoment) m_PFO_N_BAD_CELLS->Fill(moment_N_BAD_CELLS);
    else m_PFO_N_BAD_CELLS->Fill(-1.0);

    float moment_BADLARQ_FRAC = 0.0;
    gotMoment = PFO.getClusterMoment(moment_BADLARQ_FRAC,xAOD::CaloCluster::BADLARQ_FRAC);
    if (true == gotMoment) m_PFO_BADLARQ_FRAC->Fill(moment_BADLARQ_FRAC);
    else m_PFO_BADLARQ_FRAC->Fill(-1.0);

    float moment_ENG_POS = 0.0;
    gotMoment = PFO.getClusterMoment(moment_ENG_POS,xAOD::CaloCluster::ENG_POS);
    if (true == gotMoment) m_PFO_ENG_POS->Fill(moment_ENG_POS);
    else m_PFO_ENG_POS->Fill(-1.0);

    float moment_SIGNIFICANCE = 0.0;
    gotMoment = PFO.getClusterMoment(moment_SIGNIFICANCE,xAOD::CaloCluster::SIGNIFICANCE);
    if (true == gotMoment) m_PFO_SIGNIFICANCE->Fill(moment_SIGNIFICANCE);
    else m_PFO_SIGNIFICANCE->Fill(-1.0);

    float moment_AVG_LAR_Q = 0.0;
    gotMoment = PFO.getClusterMoment(moment_AVG_LAR_Q,xAOD::CaloCluster::AVG_LAR_Q);
    if (true == gotMoment) m_PFO_AVG_LAR_Q->Fill(moment_AVG_LAR_Q);
    else m_PFO_AVG_LAR_Q->Fill(-1.0);

    float moment_AVG_TILE_Q = 0.0;
    gotMoment = PFO.getClusterMoment(moment_AVG_TILE_Q,xAOD::CaloCluster::AVG_TILE_Q);
    if (true == gotMoment) m_PFO_AVG_TILE_Q->Fill(moment_AVG_TILE_Q);
    else m_PFO_AVG_TILE_Q->Fill(-1.0);

  }

}
