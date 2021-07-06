/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_PHOTONCNVPLOTS_H
#define EGAMMAPHYSVALMONITORING_PHOTONCNVPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "KinematicsPlots.h"

#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonxAODHelpers.h"
#include "xAODEventInfo/EventInfo.h"

namespace Egamma{
  
class PhotonCnvPlots:public PlotBase {
    public:
      PhotonCnvPlots(PlotBase* pParent, const std::string& sDir, const std::string& sParticleType);
      void fill(const xAOD::Photon& photon, const xAOD::EventInfo& eventInfo, bool isPrompt) const;
      
      // Reco only information
      Egamma::KinematicsPlots         m_oKinAllPlots;
      Egamma::KinematicsPlots         m_oKinIsoPlots;

      std::string m_sParticleType;

      TH1* m_nParticles;
      TH1* m_nParticles_weighted;
      TH1* m_nVtx;
      TH1* m_convR;
      TH2* m_convRvsEta;
      TH2* m_convRvsType;
      TH1* m_convType;
      TH1* m_convDeltaEta;
      TH1* m_convDeltaPhi;

    private:
      virtual void initializePlots();
      
};

}

#endif
