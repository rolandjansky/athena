/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      PhotonAmbPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Photon& photon, bool isPrompt);
      
      // Reco only information
      Egamma::KinematicsPlots         m_oKinAllPlots;
      Egamma::ShowerShapesPlots       m_oShowerShapesAllPlots;

      

      std::string m_sParticleType;

      TH1* m_nParticles;

    private:
      virtual void initializePlots();
      
};

}

#endif
