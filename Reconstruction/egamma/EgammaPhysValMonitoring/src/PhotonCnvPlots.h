/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_PHOTONCNVPLOTS_H
#define EGAMMAPHYSVALMONITORING_PHOTONCNVPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "KinematicsPlots.h"



#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonxAODHelpers.h"


namespace Egamma{
  
class PhotonCnvPlots:public PlotBase {
    public:
      PhotonCnvPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Photon& photon, bool isPrompt);
      
      // Reco only information
      Egamma::KinematicsPlots         m_oKinAllPlots;
      Egamma::KinematicsPlots         m_oKinIsoPlots;


      

      std::string m_sParticleType;

      TH1* nParticles;
      TH1* nVtx;
      TH1* convR;
      TH2* convRvsEta;
      TH2* convRvsType;
      TH1* convType;
      TH1* convDeltaEta;
      TH1* convDeltaPhi;

    private:
      virtual void initializePlots();
      
};

}

#endif
