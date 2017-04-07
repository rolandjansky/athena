/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOClusterMomentPlots.h"

namespace PFO {

  PFOClusterMomentPlots::PFOClusterMomentPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_SECOND_R = nullptr;
    m_PFO_CENTER_LAMBDA = nullptr;
    m_PFO_ISOLATION = nullptr;
    m_PFO_ENG_BAD_CELLS = nullptr;
    m_PFO_N_BAD_CELLS = nullptr;
    m_PFO_BADLARQ_FRAC = nullptr;
    m_PFO_ENG_POS = nullptr;
    m_PFO_AVG_LAR_Q = nullptr;
    m_PFO_AVG_TILE_Q = nullptr;
    m_PFO_EM_PROBABILTY = nullptr;
}

  void PFOClusterMomentPlots::initializePlots(){

    m_PFO_SECOND_R = Book1D("PFO_SECOND_R",m_sPFOContainerName + "_SECOND_R",60,-1.0,50.0); 
    m_PFO_CENTER_LAMBDA = Book1D("PFO_CENTER_LAMBDA",m_sPFOContainerName + "_CENTER_LAMBDA",60,-1.0,3000.0);
    m_PFO_ISOLATION = Book1D("PFO_ISOLATION",m_sPFOContainerName + "_ISOLATION",60,-1.0,2.0);
    m_PFO_ENG_BAD_CELLS = Book1D("PFO_ENG_BAD_CELLS",m_sPFOContainerName + "_ENG_BAD_CELLS",20,-1.0,1);
    m_PFO_N_BAD_CELLS = Book1D("PFO_N_BAD_CELLS",m_sPFOContainerName + "_N_BAD_CELLS",30,-1.0,2.0);
    m_PFO_BADLARQ_FRAC = Book1D("PFO_BADLARQ_FRAC",m_sPFOContainerName + "_BADLARQ_FRAC",25,-1.0,1.5);
    m_PFO_ENG_POS = Book1D("PFO_ENG_POS",m_sPFOContainerName + "_ENG_POS",60,-100.0,10000.0);
    m_PFO_AVG_LAR_Q = Book1D("PFO_AVG_LAR_Q",m_sPFOContainerName + "_AVG_LAR_Q",240,-1.0,200.0);
    m_PFO_AVG_TILE_Q = Book1D("PFO_AVG_TILE_Q",m_sPFOContainerName + "_AVG_TILE_Q",21,-1.0,20.0);
    m_PFO_EM_PROBABILTY = Book1D("PFO_EM_PROBABILTY",m_sPFOContainerName + "_EM_PROBABILTY",20,0,1);

  }

  void PFOClusterMomentPlots::fill(const xAOD::PFO& PFO){

    float moment_SECOND_R = 0.0;
    bool gotMoment = PFO.getClusterMoment(moment_SECOND_R,xAOD::CaloCluster::SECOND_R);
    if (true == gotMoment) m_PFO_SECOND_R->Fill(moment_SECOND_R);
    else m_PFO_SECOND_R->Fill(-1.0);

    float moment_CENTER_LAMBDA = 0.0;
    gotMoment = PFO.getClusterMoment(moment_CENTER_LAMBDA,xAOD::CaloCluster::CENTER_LAMBDA);
    if (true == gotMoment) m_PFO_CENTER_LAMBDA->Fill(moment_CENTER_LAMBDA);
    else m_PFO_CENTER_LAMBDA->Fill(-1.0);
    
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

    float moment_AVG_LAR_Q = 0.0;
    gotMoment = PFO.getClusterMoment(moment_AVG_LAR_Q,xAOD::CaloCluster::AVG_LAR_Q);
    if (true == gotMoment) m_PFO_AVG_LAR_Q->Fill(moment_AVG_LAR_Q);
    else m_PFO_AVG_LAR_Q->Fill(-1.0);

    float moment_AVG_TILE_Q = 0.0;
    gotMoment = PFO.getClusterMoment(moment_AVG_TILE_Q,xAOD::CaloCluster::AVG_TILE_Q);
    if (true == gotMoment) m_PFO_AVG_TILE_Q->Fill(moment_AVG_TILE_Q);
    else m_PFO_AVG_TILE_Q->Fill(-1.0);

    float moment_EM_PROBABILTY = 0.0;
    gotMoment = PFO.getClusterMoment(moment_EM_PROBABILTY,xAOD::CaloCluster::EM_PROBABILITY);
    if (true == gotMoment) m_PFO_EM_PROBABILTY->Fill(moment_EM_PROBABILTY);
    else m_PFO_EM_PROBABILTY->Fill(-1.0);

  }

}
