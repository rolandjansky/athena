/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOATTRIBUTEPLOTS_H
#define PFOATTRIBUTEPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEventInfo/EventInfo.h"

namespace PFO {

  class PFOAttributePlots : public PlotBase {

  public:

    PFOAttributePlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName);

     void fill(const xAOD::PFO& PFO, const xAOD::EventInfo& eventInfo);
     void fill(const xAOD::FlowElement& FE, const xAOD::EventInfo& eventInfo);

  private:

     TH1* m_PFO_LAYER_ENERGY_EM3;
     TH1* m_PFO_LAYER_ENERGY_HEC0;
     TH1* m_PFO_LAYER_ENERGY_HEC;
     TH1* m_PFO_LAYER_ENERGY_Tile0;
     TH1* m_PFO_TIMING;

     // only attribute available to the FE is the timing
     TH1* m_FE_TIMING;

     void initializePlots();
     std::string m_sPFOContainerName;
     std::string m_sFEContainerName;

  };

}
#endif
