/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOAlgPropertyPlots.h"

namespace PFO {

  PFOAlgPropertyPlots::PFOAlgPropertyPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_isInDenseEnvironment = nullptr;
    m_PFO_tracksExpectedEnergyDeposit = nullptr;
  }

  void PFOAlgPropertyPlots::initializePlots(){

    m_PFO_isInDenseEnvironment = Book1D("PFO_isInDenseEnvironment",m_sPFOContainerName+"PFO_isInDenseEnvironment",3,-1,2);
    m_PFO_tracksExpectedEnergyDeposit = Book1D("PFO_tracksExpectedEnergyDeposit",m_sPFOContainerName+"PFO_tracksExpectedEnergyDeposit",11,-1,10);

    m_PFO_isInDenseEnvironment_etaBinA = Book1D("PFO_isInDenseEnvironment_binA",m_sPFOContainerName+"PFO_isInDenseEnvironment (|eta| < 1)",3,-1,2);
    m_PFO_tracksExpectedEnergyDeposit_etaBinA = Book1D("PFO_tracksExpectedEnergyDeposit_binA)",m_sPFOContainerName+"PFO_tracksExpectedEnergyDeposit (|eta| < 1)",11,-1,10);

    m_PFO_isInDenseEnvironment_etaBinB = Book1D("PFO_isInDenseEnvironment_binB",m_sPFOContainerName+"PFO_isInDenseEnvironment (1 <= |eta| < 2)",3,-1,2);
    m_PFO_tracksExpectedEnergyDeposit_etaBinB = Book1D("PFO_tracksExpectedEnergyDeposit_binB",m_sPFOContainerName+"PFO_tracksExpectedEnergyDeposit (1 <= |eta| < 2)",11,-1,10);

    m_PFO_isInDenseEnvironment_etaBinC = Book1D("PFO_isInDenseEnvironment_binC",m_sPFOContainerName+"PFO_isInDenseEnvironment (|eta| >= 2)",3,-1,2);
    m_PFO_tracksExpectedEnergyDeposit_etaBinC = Book1D("PFO_tracksExpectedEnergyDeposit_binC",m_sPFOContainerName+"PFO_tracksExpectedEnergyDeposit (|eta| >= 2)",11,-1,10);
    
    
  }

  void PFOAlgPropertyPlots::fill(const xAOD::PFO& PFO){

    xAOD::PFODetails::PFOAttributes myAttribute_isInDenseEnvironment = xAOD::PFODetails::PFOAttributes::eflowRec_isInDenseEnvironment;
    int isInDenseEnvironment = false;
    bool gotAttribute = PFO.attribute(myAttribute_isInDenseEnvironment,isInDenseEnvironment);
    if (true == gotAttribute) {
      m_PFO_isInDenseEnvironment->Fill(isInDenseEnvironment);
      if (fabs(PFO.eta()) < 1) m_PFO_isInDenseEnvironment_etaBinA->Fill(isInDenseEnvironment);
      else if (fabs(PFO.eta()) < 2) m_PFO_isInDenseEnvironment_etaBinB->Fill(isInDenseEnvironment);
      else m_PFO_isInDenseEnvironment_etaBinC->Fill(isInDenseEnvironment);
    }

    else {
      m_PFO_isInDenseEnvironment->Fill(-1.0);
      if (fabs(PFO.eta()) < 1) m_PFO_isInDenseEnvironment_etaBinA->Fill(-1.0);
      else if (fabs(PFO.eta()) < 2) m_PFO_isInDenseEnvironment_etaBinB->Fill(-1.0);
      else m_PFO_isInDenseEnvironment_etaBinC->Fill(-1.0);
    }
      
    float expectedEnergy = 0.0;
    xAOD::PFODetails::PFOAttributes myAttribute_tracksExpectedEnergyDeposit = xAOD::PFODetails::PFOAttributes::eflowRec_tracksExpectedEnergyDeposit;
    gotAttribute = PFO.attribute(myAttribute_tracksExpectedEnergyDeposit,expectedEnergy);
    if (true == gotAttribute) {
      m_PFO_tracksExpectedEnergyDeposit->Fill(expectedEnergy/1000.0);
      if (fabs(PFO.eta()) < 1)  m_PFO_tracksExpectedEnergyDeposit_etaBinA->Fill(expectedEnergy/1000.0);
      else if (fabs(PFO.eta()) < 2) m_PFO_tracksExpectedEnergyDeposit_etaBinB->Fill(expectedEnergy/1000.0);
      else m_PFO_tracksExpectedEnergyDeposit_etaBinC->Fill(expectedEnergy/1000.0);
    }
    else{
      m_PFO_tracksExpectedEnergyDeposit->Fill(-1.0);
      if (fabs(PFO.eta()) < 1)  m_PFO_tracksExpectedEnergyDeposit_etaBinA->Fill(-1.0);
      else if (fabs(PFO.eta()) < 2) m_PFO_tracksExpectedEnergyDeposit_etaBinB->Fill(-1.0);
      else m_PFO_tracksExpectedEnergyDeposit_etaBinC->Fill(-1.0);
    }
  }
}
