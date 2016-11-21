/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_PHOTONPLOTS_H
#define EGAMMAPHYSVALMONITORING_PHOTONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "KinematicsPlots.h"
#include "ShowerShapesPlots.h"
#include "IsolationPlots.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODEgamma/Photon.h"

namespace Egamma{
  
class PhotonPlots:public PlotBase {
    public:
      PhotonPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Photon& photon, bool isPrompt);      
      // Reco only information
      Egamma::KinematicsPlots         m_oKinAllPlots;
      Egamma::ShowerShapesPlots       m_oShowerShapesAllPlots;
      Egamma::KinematicsPlots         m_oKinIsoPlots;
      Egamma::ShowerShapesPlots       m_oShowerShapesIsoPlots;
      Egamma::KinematicsPlots         m_oKinIsoUncPlots;
      Egamma::KinematicsPlots         m_oKinIsoConvPlots;
      Egamma::KinematicsPlots         m_oKinIsoLoosePlots;
      Egamma::KinematicsPlots         m_oKinIsoLooseUncPlots;
      Egamma::KinematicsPlots         m_oKinIsoLooseConvPlots;
      Egamma::KinematicsPlots         m_oKinIsoTightPlots;
      Egamma::KinematicsPlots         m_oKinIsoTightUncPlots;
      Egamma::KinematicsPlots         m_oKinIsoTightConvPlots;
      
      std::string m_sParticleType;

      TH1* m_nParticles;

    private:
      virtual void initializePlots();
      
      
};

}

#endif
