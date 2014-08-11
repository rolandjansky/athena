/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_PHOTONVALIDATIONPLOTS_H
#define EGAMMAPHYSVALMONITORING_PHOTONVALIDATIONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "PhotonPlots.h"
#include "PhotonCnvPlots.h"
#include "PhotonAmbPlots.h"

#include "xAODEgamma/Photon.h"
#include "xAODTruth/TruthParticle.h"

class PhotonValidationPlots:public PlotBase {
    public:
      PhotonValidationPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::Photon& photon, bool isPrompt);
      
      std::string m_sParticleType;
      
      // Reco only information
      Egamma::PhotonPlots             m_oAllPlots;
      Egamma::PhotonPlots             m_oPhotPlots;
      Egamma::PhotonPlots             m_oTopoPhotPlots;
      Egamma::PhotonAmbPlots          m_oAmbPhotPlots;
      Egamma::PhotonCnvPlots          m_oConvPhotPlots;
      Egamma::KinematicsPlots         m_oTruthPlots;
      Egamma::KinematicsPlots         m_oTruthIsoPlots;
      Egamma::KinematicsPlots         m_oTruthIsoConvPlots;
      Egamma::KinematicsPlots         m_oTruthIsoUncPlots;
      
      TH1* author;
      
    private:
      virtual void initializePlots();

};

#endif
