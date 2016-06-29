/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOAttributePlots.h"
#include <iostream>
namespace PFO {

  PFOAttributePlots::PFOAttributePlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName) : PlotBase(pParent, sDir), m_sPFOContainerName(sPFOContainerName){
    m_PFO_LAYER_ENERGY_EM3 = nullptr;
    m_PFO_LAYER_ENERGY_HEC0 = nullptr;
    m_PFO_LAYER_ENERGY_HEC = nullptr;
    m_PFO_LAYER_ENERGY_Tile0 = nullptr;
    m_PFO_TIMING = nullptr;
}

  void PFOAttributePlots::initializePlots(){

    m_PFO_LAYER_ENERGY_EM3 = Book1D("PFO_LAYER_ENEGRY_EM3",m_sPFOContainerName+"_LAYER_ENERGY_EM3",201,-1,100);
    m_PFO_LAYER_ENERGY_HEC0 = Book1D("PFO_LAYER_ENEGRY_HEC0",m_sPFOContainerName+"_LAYER_ENERGY_HEC0",60,-1,5);
    m_PFO_LAYER_ENERGY_HEC = Book1D("PFO_LAYER_ENEGRY_HEC",m_sPFOContainerName+"_LAYER_ENERGY_HEC",60,-1,5);
    m_PFO_LAYER_ENERGY_Tile0 = Book1D("PFO_LAYER_ENEGRY_Tile0",m_sPFOContainerName+"_LAYER_ENERGY_Tile0",60,-1,5);
    m_PFO_TIMING = Book1D("PFO_TIMING",m_sPFOContainerName+"_TIMING",10,-50,50);
    
  }

  void PFOAttributePlots::fill(const xAOD::PFO& PFO){

    float layerEnergyEM3 = 0.0;
    bool gotAttribute = PFO.attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_EM3,layerEnergyEM3);
    if (true == gotAttribute) m_PFO_LAYER_ENERGY_EM3->Fill(layerEnergyEM3);
    else m_PFO_LAYER_ENERGY_EM3->Fill(-1.0);

    float layerEnergyHEC0 = 0.0;
    gotAttribute = PFO.attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC0,layerEnergyHEC0);
    if (true == gotAttribute) m_PFO_LAYER_ENERGY_HEC0->Fill(layerEnergyHEC0);
    else m_PFO_LAYER_ENERGY_HEC0->Fill(-1.0);
  
    float layerEnergyHEC = 0.0;
    gotAttribute = PFO.attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_HEC,layerEnergyHEC);
    if (true == gotAttribute) m_PFO_LAYER_ENERGY_HEC->Fill(layerEnergyHEC);
    else m_PFO_LAYER_ENERGY_HEC->Fill(-1.0);

    float layerEnergyTile0 = 0.0;
    gotAttribute = PFO.attribute<float>(xAOD::PFODetails::eflowRec_LAYERENERGY_Tile0,layerEnergyTile0);
    if (true == gotAttribute) m_PFO_LAYER_ENERGY_Tile0->Fill(layerEnergyTile0);
    else m_PFO_LAYER_ENERGY_Tile0->Fill(-1.0);

    float timing = 0.0;
    gotAttribute = PFO.attribute<float>(xAOD::PFODetails::eflowRec_TIMING,timing);
    if (true == gotAttribute) m_PFO_TIMING->Fill(timing);
    else m_PFO_TIMING->Fill(-50);

  }

}
