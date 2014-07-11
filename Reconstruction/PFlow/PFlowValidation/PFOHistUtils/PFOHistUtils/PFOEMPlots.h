/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOEMPLOTS_H
#define PFOEMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"

namespace PFO {

  class PFOEMPlots : public PlotBase {

  public:

     PFOEMPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName);

     void fill(const xAOD::PFO& PFO);

  private:
     TH1* m_PFO_ptEM;
     TH1* m_PFO_etaEM;
     TH1* m_PFO_phiEM;
     TH1* m_PFO_mEM;
     
     void initializePlots();
     std::string m_sPFOContainerName;

  };

}
#endif
