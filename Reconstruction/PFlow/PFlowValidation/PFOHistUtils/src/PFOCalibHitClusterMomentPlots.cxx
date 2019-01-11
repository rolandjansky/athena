/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOCalibHitClusterMomentPlots.h"

namespace PFO {

  PFOCalibHitClusterMomentPlots::PFOCalibHitClusterMomentPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_ENG_CALIB_FRAC_EM = nullptr;
    m_PFO_ENG_CALIB_FRAC_HAD = nullptr;
    m_PFO_ENG_CALIB_FRAC_REST = nullptr;

    m_PFO_ENG_CALIB_FRAC_EM_etaBinA = nullptr;
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinA = nullptr;
    m_PFO_ENG_CALIB_FRAC_REST_etaBinA = nullptr;

    m_PFO_ENG_CALIB_FRAC_EM_etaBinB = nullptr;
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinB = nullptr;
    m_PFO_ENG_CALIB_FRAC_REST_etaBinB = nullptr;

    m_PFO_ENG_CALIB_FRAC_EM_etaBinC = nullptr;
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinC = nullptr;
    m_PFO_ENG_CALIB_FRAC_REST_etaBinC = nullptr;

    m_PFO_ENG_CALIB_FRAC_EM_etaBinD = nullptr;
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinD = nullptr;
    m_PFO_ENG_CALIB_FRAC_REST_etaBinD = nullptr;
  }

  void PFOCalibHitClusterMomentPlots::initializePlots(){

    m_PFO_ENG_CALIB_FRAC_EM = Book1D("_ENG_CALIB_FRAC_EM",m_sPFOContainerName + "_ENG_CALIB_FRAC_EM",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_HAD = Book1D("_ENG_CALIB_FRAC_HAD",m_sPFOContainerName + "_ENG_CALIB_FRAC_HAD",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_REST = Book1D("_ENG_CALIB_FRAC_REST",m_sPFOContainerName + "_ENG_CALIB_FRAC_REST",22,0,1.1);
    
    m_PFO_ENG_CALIB_FRAC_EM_etaBinA = Book1D("_ENG_CALIB_FRAC_EM_A",m_sPFOContainerName + "_ENG_CALIB_FRAC_EM (|eta| < 1.5)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinA = Book1D("_ENG_CALIB_FRAC_HAD_A",m_sPFOContainerName + "_ENG_CALIB_FRAC_HAD (|eta| < 1.5)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_REST_etaBinA = Book1D("_ENG_CALIB_FRAC_REST_A",m_sPFOContainerName + "_ENG_CALIB_FRAC_REST (|eta| < 1.5)",22,0,1.1);
    
    m_PFO_ENG_CALIB_FRAC_EM_etaBinB = Book1D("_ENG_CALIB_FRAC_EM_B",m_sPFOContainerName + "_ENG_CALIB_FRAC_EM (1.5 <= |eta| < 2.5)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinB = Book1D("_ENG_CALIB_FRAC_HAD_B",m_sPFOContainerName + "_ENG_CALIB_FRAC_HAD (1.5 <= |eta| < 2.5)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_REST_etaBinB = Book1D("_ENG_CALIB_FRAC_REST_B",m_sPFOContainerName + "_ENG_CALIB_FRAC_REST (1.5 <= |eta| < 2.5)",22,0,1.1);
    
    m_PFO_ENG_CALIB_FRAC_EM_etaBinC = Book1D("_ENG_CALIB_FRAC_EM_C",m_sPFOContainerName + "_ENG_CALIB_FRAC_EM (2.5 <= |eta| < 3.2)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinC = Book1D("_ENG_CALIB_FRAC_HAD_C",m_sPFOContainerName + "_ENG_CALIB_FRAC_HAD (2.5 <= |eta| < 3.2)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_REST_etaBinC = Book1D("_ENG_CALIB_FRAC_REST_C",m_sPFOContainerName + "_ENG_CALIB_FRAC_REST (2.5 <= |eta| < 3.2)",22,0,1.1);

    m_PFO_ENG_CALIB_FRAC_EM_etaBinD = Book1D("_ENG_CALIB_FRAC_EM_D",m_sPFOContainerName + "_ENG_CALIB_FRAC_EM (|eta| >= 3.2)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_HAD_etaBinD = Book1D("_ENG_CALIB_FRAC_HAD_D",m_sPFOContainerName + "_ENG_CALIB_FRAC_HAD (|eta| >= 3.2)",22,0,1.1);
    m_PFO_ENG_CALIB_FRAC_REST_etaBinD = Book1D("_ENG_CALIB_FRAC_REST_D",m_sPFOContainerName + "_ENG_CALIB_FRAC_REST (|eta| >= 3.2)",22,0,1.1);
    
  }

  void PFOCalibHitClusterMomentPlots::fill(const xAOD::PFO& PFO){
    
    float moment_ENG_CALIB_FRAC_EM = -1.0;
    PFO.getClusterMoment(moment_ENG_CALIB_FRAC_EM,xAOD::CaloCluster::ENG_CALIB_FRAC_EM);
    m_PFO_ENG_CALIB_FRAC_EM->Fill(moment_ENG_CALIB_FRAC_EM);
    
    float moment_ENG_CALIB_FRAC_HAD = -1.0;
    PFO.getClusterMoment(moment_ENG_CALIB_FRAC_HAD,xAOD::CaloCluster::ENG_CALIB_FRAC_HAD);
    m_PFO_ENG_CALIB_FRAC_HAD->Fill(moment_ENG_CALIB_FRAC_HAD);

    float moment_ENG_CALIB_FRAC_REST = -1.0;
    PFO.getClusterMoment(moment_ENG_CALIB_FRAC_REST,xAOD::CaloCluster::ENG_CALIB_FRAC_REST);
    m_PFO_ENG_CALIB_FRAC_REST->Fill(moment_ENG_CALIB_FRAC_REST);
    
    float PFOEta = PFO.eta();
    
    if (fabs(PFOEta) < 1.5){
      m_PFO_ENG_CALIB_FRAC_EM_etaBinA->Fill(moment_ENG_CALIB_FRAC_EM);
      m_PFO_ENG_CALIB_FRAC_HAD_etaBinA->Fill(moment_ENG_CALIB_FRAC_HAD);
      m_PFO_ENG_CALIB_FRAC_REST_etaBinA->Fill(moment_ENG_CALIB_FRAC_REST);
    }//|eta| < 1.5
    else if (fabs(PFOEta) < 2.5){
      m_PFO_ENG_CALIB_FRAC_EM_etaBinB->Fill(moment_ENG_CALIB_FRAC_EM);
      m_PFO_ENG_CALIB_FRAC_HAD_etaBinB->Fill(moment_ENG_CALIB_FRAC_HAD);
      m_PFO_ENG_CALIB_FRAC_REST_etaBinB->Fill(moment_ENG_CALIB_FRAC_REST);
    }
    else if (fabs(PFOEta) < 3.2){
      m_PFO_ENG_CALIB_FRAC_EM_etaBinC->Fill(moment_ENG_CALIB_FRAC_EM);
      m_PFO_ENG_CALIB_FRAC_HAD_etaBinC->Fill(moment_ENG_CALIB_FRAC_HAD);
      m_PFO_ENG_CALIB_FRAC_REST_etaBinC->Fill(moment_ENG_CALIB_FRAC_REST);
    }
    else{
      m_PFO_ENG_CALIB_FRAC_EM_etaBinD->Fill(moment_ENG_CALIB_FRAC_EM);
      m_PFO_ENG_CALIB_FRAC_HAD_etaBinD->Fill(moment_ENG_CALIB_FRAC_HAD);
      m_PFO_ENG_CALIB_FRAC_REST_etaBinD->Fill(moment_ENG_CALIB_FRAC_REST);
    }
    
  }

}
