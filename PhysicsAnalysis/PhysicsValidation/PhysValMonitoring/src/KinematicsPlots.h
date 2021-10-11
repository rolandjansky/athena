/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSVALMONITORING_KINEMATICSPLOTS_H
#define PHYSVALMONITORING_KINEMATICSPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODBase/IParticle.h"
#include "xAODEventInfo/EventInfo.h"

namespace PhysVal{
  
class KinematicsPlots:public PlotBase {
    public:
      KinematicsPlots(PlotBase* pParent, const std::string& sDir, std::string sParticleType);
      void fill(const xAOD::IParticle* part,const xAOD::EventInfo* evt);
      void fill(const xAOD::EventInfo* evt);

      void initializeEvent();
      
      // Reco only information
      std::string m_sParticleType;

      TH1* n = nullptr;

      TH1* et = nullptr;
      TH1* eta = nullptr;
      TH1* phi = nullptr;

      TH1* lead_et = nullptr;
      TH1* lead_eta = nullptr;
      TH1* lead_phi = nullptr;

    private:

      const xAOD::IParticle* m_lead = nullptr;
      unsigned int m_npart = 0;

      virtual void initializePlots();

      
};

}

#endif
