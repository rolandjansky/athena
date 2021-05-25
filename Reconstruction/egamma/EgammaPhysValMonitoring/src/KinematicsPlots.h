/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_KINEMATICSPLOTS_H
#define EGAMMAPHYSVALMONITORING_KINEMATICSPLOTS_H

#include "TrkValHistUtils/PlotBase.h"

#include "xAODEgamma/Egamma.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODEventInfo/EventInfo.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace Egamma{
  
class KinematicsPlots:public PlotBase {
    public:
      KinematicsPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
      void fill(const xAOD::IParticle& part, const xAOD::EventInfo& eventInfo) const;
      
      // Reco only information
      std::string m_sParticleType;

      TH1* et;
      TH1* eta;
      TH1* phi;
      TH2* etvseta;

    private:
      virtual void initializePlots();
      
};

}

#endif
