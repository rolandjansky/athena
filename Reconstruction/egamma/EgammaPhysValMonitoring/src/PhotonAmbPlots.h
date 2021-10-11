/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_PHOTONAMBPLOTS_H
#define EGAMMAPHYSVALMONITORING_PHOTONAMBPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "KinematicsPlots.h"
#include "ShowerShapesPlots.h"

#include "xAODEgamma/Photon.h"

namespace Egamma{
  
class PhotonAmbPlots:public PlotBase {
    public:
      PhotonAmbPlots(PlotBase* pParent, const std::string& sDir, const std::string& sParticleType);
      void fill(const xAOD::Photon& photon, const xAOD::EventInfo& eventInfo, bool isPrompt) const;
      
      // Reco only information
      Egamma::KinematicsPlots         m_oKinAllPlots;
      Egamma::ShowerShapesPlots       m_oShowerShapesAllPlots;
  
      std::string m_sParticleType;

      TH1* m_nParticles;
      TH1* m_nParticles_weighted;

    private:
      virtual void initializePlots();
      
};

}

#endif
