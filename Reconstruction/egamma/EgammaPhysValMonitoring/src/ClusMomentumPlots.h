/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_CLUSMOMENTUMPLOTS_H
#define EGAMMAPHYSVALMONITORING_CLUSMOMENTUMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEventInfo/EventInfo.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace Egamma{
  
class ClusMomentumPlots:public PlotBase {
    public:
      ClusMomentumPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
      void fill(const xAOD::Egamma& egamma, const xAOD::EventInfo& eventInfo) const;
      
      std::string m_sParticleType;

      TH1* secondlambda;
      TH1* secondr;
      TH1* centerlambda;
      TH1* engfracmax;
      TH1* longitudinal;
      TH1* lateral;

    private:
      virtual void initializePlots();
      
};

}

#endif
