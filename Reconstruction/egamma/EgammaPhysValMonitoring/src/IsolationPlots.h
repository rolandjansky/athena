/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_ISOLATIONPLOTS_H
#define EGAMMAPHYSVALMONITORING_ISOLATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODPrimitives/IsolationType.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace Egamma{
  
class IsolationPlots:public PlotBase {
    public:
      IsolationPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
  void fill(const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo);
     
      std::string m_sParticleType;

      TH1* etcone20;
      TH1* etcone30;
      TH1* etcone40;
      TH1* ptcone20;
      TH1* ptcone30;
      TH1* ptcone40;
      TH1* ptvarcone20;
      TH1* ptvarcone30;
      TH1* ptvarcone40;
      TH1* topoetcone20;
      TH1* topoetcone30;
      TH1* topoetcone40;

    private:
      virtual void initializePlots();
      void FillIsolationPlot(TH1* hist, xAOD::Iso::IsolationType isoType, const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo);
//      void FillIsolationPlot(TH1* hist, xAOD::EgammaParameters::IsolationType isoType, const xAOD::Egamma& egamma);//rel19
      
};

}

#endif
