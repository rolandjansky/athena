/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PFOHistUtils/PFOAttributePlots.h"
#include <iostream>
namespace PFO {

  PFOAttributePlots::PFOAttributePlots(PlotBase* pParent, std::string sDir, std::string sFEContainerName) : PlotBase(pParent, sDir), 																	  
																	  m_sFEContainerName(sFEContainerName) {    
    m_FE_LAYER_ENERGY_Tile0 = nullptr;
    m_FE_TIMING = nullptr;
}

  void PFOAttributePlots::initializePlots(){    
    if(!m_sFEContainerName.empty()){
      m_FE_LAYER_ENERGY_Tile0 = Book1D("_LAYER_ENEGRY_HEC",m_sFEContainerName+"_LAYER_ENERGY_HEC",60,-1,5);
      m_FE_TIMING = Book1D("_TIMING",m_sFEContainerName+"_TIMING",10,-50,50);
    }
  }

 
  void PFOAttributePlots::fill(const xAOD::FlowElement& FE, const xAOD::EventInfo& eventInfo){
    float timing=-50;
    static SG::AuxElement::ConstAccessor<float>acc_FE_Timing("TIMING");
    if(acc_FE_Timing.isAvailable(FE)){
      timing=acc_FE_Timing(FE);
      m_FE_TIMING->Fill(timing,eventInfo.beamSpotWeight());
    }
    else{
      m_FE_TIMING->Fill(timing,eventInfo.beamSpotWeight());
    }

    float E_tile0 = -50;
    static SG::AuxElement::ConstAccessor<float>acc_FE_LAYER_ENERGY_Tile0("LAYERENERGY_TILE0");
    if(acc_FE_LAYER_ENERGY_Tile0.isAvailable(FE)){
      E_tile0 =acc_FE_LAYER_ENERGY_Tile0(FE);
      m_FE_LAYER_ENERGY_Tile0->Fill(E_tile0 ,eventInfo.beamSpotWeight());
    }
    else{
      m_FE_LAYER_ENERGY_Tile0->Fill(E_tile0 ,eventInfo.beamSpotWeight());
    }

  } // end of FE fill function
} // end of PFO namespace
