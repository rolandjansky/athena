/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOATTRIBUTEPLOTS_H
#define PFOATTRIBUTEPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"

namespace PFO {

  class PFOAttributePlots : public PlotBase {

  public:

     PFOAttributePlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName);

     void fill(const xAOD::PFO& PFO);

  private:

     TH1* m_PFO_LAYER_ENERGY_EM3;
     TH1* m_PFO_LAYER_ENERGY_HEC0;
     TH1* m_PFO_LAYER_ENERGY_HEC;
     TH1* m_PFO_LAYER_ENERGY_Tile0;
     TH1* m_PFO_TIMING;

     void initializePlots();
     std::string m_sPFOContainerName;

  };

}
#endif
