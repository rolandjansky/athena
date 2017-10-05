/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/ClusterMomentPlots.h"

namespace PFO {

  ClusterMomentPlots::ClusterMomentPlots(PlotBase* pParent, std::string sDir, std::string sClusterContainerName) : PlotBase(pParent, sDir), m_sClusterContainerName(sClusterContainerName){
    m_LATERAL = nullptr;
    m_LONGITUDINAL = nullptr;
    m_SECOND_R = nullptr;
    m_CENTER_LAMBDA = nullptr;
    m_FIRST_ENG_DENS = nullptr;
    m_ENG_FRAC_MAX = nullptr;
    m_ISOLATION = nullptr;
    m_ENG_BAD_CELLS = nullptr;
    m_N_BAD_CELLS = nullptr;
    m_BADLARQ_FRAC = nullptr;
    m_ENG_POS = nullptr;
    m_SIGNIFICANCE = nullptr;
    m_AVG_LAR_Q = nullptr;
    m_AVG_TILE_Q = nullptr;
    m_CELL_SIG_SAMPLING = nullptr;
    m_ENG_BAD_HV_CELLS = nullptr;
    m_N_BAD_HV_CELLS = nullptr;
    m_EM_PROBABILITY = nullptr;

}

  void ClusterMomentPlots::initializePlots(){

    m_LATERAL = Book1D("PFO_LATERAL",m_sClusterContainerName + "_LATERAL",60,-1.0,2.0);
    m_LONGITUDINAL = Book1D("PFO_LONGITUDINAL",m_sClusterContainerName + "_LONGITUDINAL",60,-1.0,2.0);
    m_SECOND_R = Book1D("PFO_SECOND_R",m_sClusterContainerName + "_SECOND_R",60,-1.0,50.0); 
    m_CENTER_LAMBDA = Book1D("PFO_CENTER_LAMBDA",m_sClusterContainerName + "_CENTER_LAMBDA",60,-1.0,3000.0);
    m_FIRST_ENG_DENS = Book1D("PFO_FIRST_ENG_DENS",m_sClusterContainerName + "_FIRST_ENG_DENS",60,-1.0,2.0);
    m_ENG_FRAC_MAX = Book1D("PFO_ENG_FRAC_MAX",m_sClusterContainerName + "_ENG_FRAC_MAX",60,-1.0,2.0);
    m_ISOLATION = Book1D("PFO_ISOLATION",m_sClusterContainerName + "_ISOLATION",60,-1.0,2.0);
    m_ENG_BAD_CELLS = Book1D("PFO_ENG_BAD_CELLS",m_sClusterContainerName + "_ENG_BAD_CELLS",120,-1.0,100.0);
    m_N_BAD_CELLS = Book1D("PFO_N_BAD_CELLS",m_sClusterContainerName + "_N_BAD_CELLS",120,-1.0,20.0);
    m_BADLARQ_FRAC = Book1D("PFO_BADLARQ_FRAC",m_sClusterContainerName + "_BADLARQ_FRAC",140,-3.0,5.0);
    m_ENG_POS = Book1D("PFO_ENG_POS",m_sClusterContainerName + "_ENG_POS",60,-100.0,10000.0);
    m_SIGNIFICANCE = Book1D("PFO_SIGNIFICANCE",m_sClusterContainerName + "_SIGNIFICANCE",300,-20.0,20.0);
    m_AVG_LAR_Q = Book1D("PFO_AVG_LAR_Q",m_sClusterContainerName + "_AVG_LAR_Q",240,-1.0,200.0);
    m_AVG_TILE_Q = Book1D("PFO_AVG_TILE_Q",m_sClusterContainerName + "_AVG_TILE_Q",240,-1.0,200.0);
    m_CELL_SIG_SAMPLING = Book1D("PFO_CELL_SIG_SAMPLING",m_sClusterContainerName + "_CELL_SIG_SAMPLING",31,-1.0,30);
    m_ENG_BAD_HV_CELLS = Book1D("PFO_ENG_BAD_HV_CELLS",m_sClusterContainerName + "_ENG_BAD_HV_CELLS",500,-5000.0,20000.0); 
    m_N_BAD_HV_CELLS = Book1D("PFO_N_BAD_HV_CELLS",m_sClusterContainerName + "_N_BAD_HV_CELLS",50,-1.0,100.0);
    m_EM_PROBABILITY = Book1D("PFO_EM_PROBABILITY",m_sClusterContainerName + "_EM_PROBABILITY",50,-0.05,1.05); 

  }

  void ClusterMomentPlots::fill(const xAOD::CaloCluster& cluster){

    double moment_LATERAL = 0.0;
    bool gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::LATERAL,moment_LATERAL);
    if (true == gotMoment) m_LATERAL->Fill(moment_LATERAL);
    else m_LATERAL->Fill(-1.0);

    double moment_LONGITUDINAL = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::LONGITUDINAL,moment_LONGITUDINAL);
    if (true == gotMoment) m_LONGITUDINAL->Fill(moment_LONGITUDINAL);
    else m_LONGITUDINAL->Fill(-1.0);
    
    double moment_SECOND_R = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::SECOND_R,moment_SECOND_R);
    if (true == gotMoment) m_SECOND_R->Fill(moment_SECOND_R);
    else m_SECOND_R->Fill(-1.0);

    double moment_CENTER_LAMBDA = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,moment_CENTER_LAMBDA);
    if (true == gotMoment) m_CENTER_LAMBDA->Fill(moment_CENTER_LAMBDA);
    else m_CENTER_LAMBDA->Fill(-1.0);
    
    double moment_FIRST_ENG_DENS = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS,moment_FIRST_ENG_DENS);
    if (true == gotMoment) m_FIRST_ENG_DENS->Fill(moment_FIRST_ENG_DENS);
    else m_FIRST_ENG_DENS->Fill(-1.0);

    double moment_ENG_FRAC_MAX = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::ENG_FRAC_MAX,moment_ENG_FRAC_MAX);
    if (true == gotMoment) m_ENG_FRAC_MAX->Fill(moment_ENG_FRAC_MAX);
    else m_ENG_FRAC_MAX->Fill(-1.0);

    double moment_ISOLATION = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::ISOLATION,moment_ISOLATION);
    if (true == gotMoment) m_ISOLATION->Fill(moment_ISOLATION);
    else m_ISOLATION->Fill(-1.0);

    double moment_ENG_BAD_CELLS = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::ENG_BAD_CELLS,moment_ENG_BAD_CELLS);
    if (true == gotMoment) m_ENG_BAD_CELLS->Fill(moment_ENG_BAD_CELLS);
    else m_ENG_BAD_CELLS->Fill(-1.0);

    double moment_N_BAD_CELLS = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::N_BAD_CELLS,moment_N_BAD_CELLS);
    if (true == gotMoment) m_N_BAD_CELLS->Fill(moment_N_BAD_CELLS);
    else m_N_BAD_CELLS->Fill(-1.0);

    double moment_BADLARQ_FRAC = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::BADLARQ_FRAC,moment_BADLARQ_FRAC);
    if (true == gotMoment) m_BADLARQ_FRAC->Fill(moment_BADLARQ_FRAC);
    else m_BADLARQ_FRAC->Fill(-1.0);

    double moment_ENG_POS = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::ENG_POS,moment_ENG_POS);
    if (true == gotMoment) m_ENG_POS->Fill(moment_ENG_POS);
    else m_ENG_POS->Fill(-1.0);

    double moment_SIGNIFICANCE = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::SIGNIFICANCE,moment_SIGNIFICANCE);
    if (true == gotMoment) m_SIGNIFICANCE->Fill(moment_SIGNIFICANCE);
    else m_SIGNIFICANCE->Fill(-1.0);

    double moment_AVG_LAR_Q = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::AVG_LAR_Q,moment_AVG_LAR_Q);
    if (true == gotMoment) m_AVG_LAR_Q->Fill(moment_AVG_LAR_Q);
    else m_AVG_LAR_Q->Fill(-1.0);

    double moment_AVG_TILE_Q = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::AVG_TILE_Q,moment_AVG_TILE_Q);
    if (true == gotMoment) m_AVG_TILE_Q->Fill(moment_AVG_TILE_Q);
    else m_AVG_TILE_Q->Fill(-1.0);

    double moment_CELL_SIG_SAMPLING = 0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::CELL_SIG_SAMPLING,moment_CELL_SIG_SAMPLING);
    if (true == gotMoment) m_CELL_SIG_SAMPLING->Fill(moment_CELL_SIG_SAMPLING);
    else m_CELL_SIG_SAMPLING->Fill(-1.0);

    double moment_ENG_BAD_HV_CELLS = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::ENG_BAD_HV_CELLS,moment_ENG_BAD_HV_CELLS);
    if (true == gotMoment) m_ENG_BAD_HV_CELLS->Fill(moment_ENG_BAD_HV_CELLS);
    else m_ENG_BAD_HV_CELLS->Fill(-1.0);

    double moment_N_BAD_HV_CELLS = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::N_BAD_HV_CELLS,moment_N_BAD_HV_CELLS);
    if (true == gotMoment) m_N_BAD_HV_CELLS->Fill(moment_N_BAD_HV_CELLS);
    else m_N_BAD_HV_CELLS->Fill(-1.0);

    double moment_EM_PROBABILITY = 0.0;
    gotMoment = cluster.retrieveMoment(xAOD::CaloCluster::EM_PROBABILITY,moment_EM_PROBABILITY);
    if (true == gotMoment) m_EM_PROBABILITY->Fill(moment_EM_PROBABILITY);
    else m_EM_PROBABILITY->Fill(-1.0);

  }

}
