/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PFOALGPROPERTYPLOTS_H
#define PFOALGPROPERTYPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#include "xAODEventInfo/EventInfo.h"

namespace PFO {

  class PFOAlgPropertyPlots : public PlotBase {

    public:

    PFOAlgPropertyPlots(PlotBase *pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName);

     void fill(const xAOD::PFO& PFO, const xAOD::EventInfo& eventInfo);
     void fill(const xAOD::FlowElement& theFE, const xAOD::EventInfo& eventInfo);
  private:
     /** PFO histograms */
     TH1* m_PFO_isInDenseEnvironment;
     TH1* m_PFO_tracksExpectedEnergyDeposit;
     
     /* Same histograms in three eta bins of |eta| < 1, 1 <= |eta| < 2 and |eta| >= 2 */
     TH1* m_PFO_isInDenseEnvironment_etaBinA;
     TH1* m_PFO_tracksExpectedEnergyDeposit_etaBinA;
     
     TH1* m_PFO_isInDenseEnvironment_etaBinB;
     TH1* m_PFO_tracksExpectedEnergyDeposit_etaBinB;
     
     TH1* m_PFO_isInDenseEnvironment_etaBinC;
     TH1* m_PFO_tracksExpectedEnergyDeposit_etaBinC;
     
     /** Flow Element Histograms */
     TH1* m_FE_isInDenseEnvironment;
     TH1* m_FE_tracksExpectedEnergyDeposit;
     
     /* Same histograms in three eta bins of |eta| < 1, 1 <= |eta| < 2 and |eta| >= 2 */
     TH1* m_FE_isInDenseEnvironment_etaBinA;
     TH1* m_FE_tracksExpectedEnergyDeposit_etaBinA;
     
     TH1* m_FE_isInDenseEnvironment_etaBinB;
     TH1* m_FE_tracksExpectedEnergyDeposit_etaBinB;
     
     TH1* m_FE_isInDenseEnvironment_etaBinC;
     TH1* m_FE_tracksExpectedEnergyDeposit_etaBinC;
     
     
     void initializePlots();
     std::string m_sPFOContainerName;
     std::string m_sFEContainerName;
     
  };
}
  
#endif
