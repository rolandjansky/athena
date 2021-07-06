/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOEMPLOTS_H
#define PFOEMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEventInfo/EventInfo.h"

namespace PFO {

  class PFOEMPlots : public PlotBase {

  public:

    PFOEMPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName);

     void fill(const xAOD::PFO& PFO, const xAOD::EventInfo& eventInfo);     
     
  private:
     TH1* m_PFO_ptEM;
     TH1* m_PFO_etaEM;
     TH1* m_PFO_phiEM;
     TH1* m_PFO_mEM;

     TH1* m_FE_ptEM;
     TH1* m_FE_etaEM;
     TH1* m_FE_phiEM;
     TH1* m_FE_mEM;
     
     void initializePlots();
     std::string m_sPFOContainerName;
     std::string m_sFEContainerName;
  };

}
#endif
