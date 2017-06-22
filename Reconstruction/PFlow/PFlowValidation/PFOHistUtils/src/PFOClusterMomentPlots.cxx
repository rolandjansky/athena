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
    m_PFO_SECOND_LAMBDA = nullptr;
  }

  void PFOClusterMomentPlots::initializePlots(){

    m_PFO_SECOND_R = Book1D("PFO_SECOND_R",m_sPFOContainerName + "_SECOND_R",60,-1.0,50.0); 
    m_PFO_CENTER_LAMBDA = Book1D("PFO_CENTER_LAMBDA",m_sPFOContainerName + "_CENTER_LAMBDA",60,-50.0,3000.0);
    m_PFO_ISOLATION = Book1D("PFO_ISOLATION",m_sPFOContainerName + "_ISOLATION",60,-1.0,2.0);
    m_PFO_ENG_BAD_CELLS = Book1D("PFO_ENG_BAD_CELLS",m_sPFOContainerName + "_ENG_BAD_CELLS",60,-1.0,5);
    m_PFO_N_BAD_CELLS = Book1D("PFO_N_BAD_CELLS",m_sPFOContainerName + "_N_BAD_CELLS",30,-1.0,2.0);
    m_PFO_BADLARQ_FRAC = Book1D("PFO_BADLARQ_FRAC",m_sPFOContainerName + "_BADLARQ_FRAC",25,-1.0,1.5);
    m_PFO_ENG_POS = Book1D("PFO_ENG_POS",m_sPFOContainerName + "_ENG_POS",60,-100.0,10000.0);
    m_PFO_AVG_LAR_Q = Book1D("PFO_AVG_LAR_Q",m_sPFOContainerName + "_AVG_LAR_Q",31,-1000.0,30000.0);
    m_PFO_AVG_TILE_Q = Book1D("PFO_AVG_TILE_Q",m_sPFOContainerName + "_AVG_TILE_Q",21,-10.0,200.0);
    m_PFO_EM_PROBABILTY = Book1D("PFO_EM_PROBABILTY",m_sPFOContainerName + "_EM_PROBABILTY",21,-1.0,1.0);
    m_PFO_SECOND_LAMBDA = Book1D("PFO_SECOND_LAMBDA",m_sPFOContainerName + "_SECOND_LAMBDA",60,-1.0,3000.0);
  }

  void PFOClusterMomentPlots::fill(const xAOD::PFO& PFO){

    float moment_SECOND_R = -1.0;
    PFO.getClusterMoment(moment_SECOND_R,xAOD::CaloCluster::SECOND_R);
    m_PFO_SECOND_R->Fill(moment_SECOND_R);
        
    float moment_CENTER_LAMBDA = -1.0;
    PFO.getClusterMoment(moment_CENTER_LAMBDA,xAOD::CaloCluster::CENTER_LAMBDA);
    m_PFO_CENTER_LAMBDA->Fill(moment_CENTER_LAMBDA);
    
    float moment_ISOLATION = -1.0;
    PFO.getClusterMoment(moment_ISOLATION,xAOD::CaloCluster::ISOLATION);
    m_PFO_ISOLATION->Fill(moment_ISOLATION);

    float moment_ENG_BAD_CELLS = -1.0;
    PFO.getClusterMoment(moment_ENG_BAD_CELLS,xAOD::CaloCluster::ENG_BAD_CELLS);
    m_PFO_ENG_BAD_CELLS->Fill(moment_ENG_BAD_CELLS);

    float moment_N_BAD_CELLS = -1.0;
    PFO.getClusterMoment(moment_N_BAD_CELLS,xAOD::CaloCluster::N_BAD_CELLS);
    m_PFO_N_BAD_CELLS->Fill(moment_N_BAD_CELLS);

    float moment_BADLARQ_FRAC = -1.0;
    PFO.getClusterMoment(moment_BADLARQ_FRAC,xAOD::CaloCluster::BADLARQ_FRAC);
    m_PFO_BADLARQ_FRAC->Fill(moment_BADLARQ_FRAC);

    float moment_ENG_POS = -1.0;
    PFO.getClusterMoment(moment_ENG_POS,xAOD::CaloCluster::ENG_POS);
    m_PFO_ENG_POS->Fill(moment_ENG_POS);

    float moment_AVG_LAR_Q = -1.0;
    PFO.getClusterMoment(moment_AVG_LAR_Q,xAOD::CaloCluster::AVG_LAR_Q);
    m_PFO_AVG_LAR_Q->Fill(moment_AVG_LAR_Q);

    float moment_AVG_TILE_Q = -1.0;
    PFO.getClusterMoment(moment_AVG_TILE_Q,xAOD::CaloCluster::AVG_TILE_Q);
    m_PFO_AVG_TILE_Q->Fill(moment_AVG_TILE_Q);

    float moment_EM_PROBABILTY = -1.0;
    PFO.getClusterMoment(moment_EM_PROBABILTY,xAOD::CaloCluster::EM_PROBABILITY);
    m_PFO_EM_PROBABILTY->Fill(moment_EM_PROBABILTY);
    
    float moment_SECOND_LAMBDA = -1.0;
    PFO.getClusterMoment(moment_SECOND_LAMBDA,xAOD::CaloCluster::SECOND_LAMBDA);
    m_PFO_SECOND_LAMBDA->Fill(moment_SECOND_LAMBDA);
    
  }

}
