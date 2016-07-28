/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOALGPROPERTYPLOTS_H
#define PFOALGPROPERTYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"

namespace PFO {

  class PFOAlgPropertyPlots : public PlotBase {

    public:

     PFOAlgPropertyPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName);

     void fill(const xAOD::PFO& PFO);

  private:

    TH1* m_PFO_isInDenseEnvironment;
    TH1* m_PFO_tracksExpectedEnergyDeposit;

    /* Same histograms in three eta bins of |eta| < 1, 1 <= |eta| < 2 and |eta| >= 2 */
    TH1* m_PFO_isInDenseEnvironment_etaBinA;
    TH1* m_PFO_tracksExpectedEnergyDeposit_etaBinA;

    TH1* m_PFO_isInDenseEnvironment_etaBinB;
    TH1* m_PFO_tracksExpectedEnergyDeposit_etaBinB;

    TH1* m_PFO_isInDenseEnvironment_etaBinC;
    TH1* m_PFO_tracksExpectedEnergyDeposit_etaBinC;

    
    void initializePlots();
    std::string m_sPFOContainerName;
    
  };
}
  
#endif
