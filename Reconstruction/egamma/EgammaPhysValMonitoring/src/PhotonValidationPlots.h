/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "CLHEP/Units/SystemOfUnits.h"

class PhotonValidationPlots:public PlotBase {
    public:
      PhotonValidationPlots(PlotBase* pParent, const std::string& sDir);
  void fill(const xAOD::Photon& photon, const xAOD::EventInfo& eventInfo, bool isPrompt) const;
      
      std::string m_sParticleType;
      
      // Reco only information
      Egamma::PhotonPlots             m_oAllPlots;
      Egamma::PhotonPlots             m_oPhotPlots;
      Egamma::PhotonPlots             m_oTopoPhotPlots;
      Egamma::PhotonAmbPlots          m_oAmbPhotPlots;
      Egamma::PhotonCnvPlots          m_oConvPhotPlots;
      Egamma::KinematicsPlots         m_oTruthAllPlots;
      Egamma::KinematicsPlots         m_oTruthAllIsoPlots;
      Egamma::KinematicsPlots         m_oTruthAllIsoConvPlots;
      Egamma::KinematicsPlots         m_oTruthAllIsoUncPlots;
      Egamma::KinematicsPlots         m_oTruthRecoPlots;
      Egamma::KinematicsPlots         m_oTruthRecoConvPlots;
      Egamma::KinematicsPlots         m_oTruthRecoUncPlots;
      Egamma::KinematicsPlots         m_oTruthRecoLoosePlots;
      Egamma::KinematicsPlots         m_oTruthRecoLooseConvPlots;
      Egamma::KinematicsPlots         m_oTruthRecoLooseUncPlots;
      Egamma::KinematicsPlots         m_oTruthRecoTightPlots;
      Egamma::KinematicsPlots         m_oTruthRecoTightConvPlots;
      Egamma::KinematicsPlots         m_oTruthRecoTightUncPlots;
      Egamma::KinematicsPlots         m_oTruthIsoPlots;
      Egamma::KinematicsPlots         m_oTruthIsoConvPlots;
      Egamma::KinematicsPlots         m_oTruthIsoUncPlots;
      
      TH1* author;
      TH1* convTruthR;
      TH1* convTruthMatchedR;
      TH2* convTruthRvsEta;
      TH2* convTruthMatchedRvsEta;
      TProfile* res_et;
      TProfile* res_eta;
      TProfile* res_et_cut;
      TProfile* res_eta_cut;
      
    private:
      virtual void initializePlots();

};

#endif
