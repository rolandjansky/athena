/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_ISOLATIONPLOTS_H
#define EGAMMAPHYSVALMONITORING_ISOLATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODEgamma/Egamma.h"
#include "xAODPrimitives/IsolationType.h"
#include "CLHEP/Units/SystemOfUnits.h"

namespace Egamma{
  
class IsolationPlots:public PlotBase {
    public:
      IsolationPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Egamma& egamma);
     
      std::string m_sParticleType;

      TH1* etcone20;
      TH1* etcone30;
      TH1* etcone40;
      TH1* etcone20corr;
      TH1* etcone30corr;
      TH1* etcone40corr;
      TH1* ptcone20;
      TH1* ptcone30;
      TH1* ptcone40;
      TH1* nucone20;
      TH1* nucone30;
      TH1* nucone40;
      TH1* topoetcone20;
      TH1* topoetcone30;
      TH1* topoetcone40;

    private:
      virtual void initializePlots();
      void FillIsolationPlot(TH1* hist, xAOD::Iso::IsolationType isoType, const xAOD::Egamma& egamma);
//      void FillIsolationPlot(TH1* hist, xAOD::EgammaParameters::IsolationType isoType, const xAOD::Egamma& egamma);//rel19
      
};

}

#endif
