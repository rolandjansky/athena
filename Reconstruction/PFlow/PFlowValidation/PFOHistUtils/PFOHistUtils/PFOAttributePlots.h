/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOATTRIBUTEPLOTS_H
#define PFOATTRIBUTEPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEventInfo/EventInfo.h"

namespace PFO {

  class PFOAttributePlots : public PlotBase {

  public:

    PFOAttributePlots(PlotBase *pParent, std::string sDir, std::string sFEContainerName);
     
     void fill(const xAOD::FlowElement& FE, const xAOD::EventInfo& eventInfo);

  private:     

     TH1* m_FE_TIMING;
     TH1* m_FE_LAYER_ENERGY_Tile0;

     void initializePlots();     
     std::string m_sFEContainerName;

  };

}
#endif
