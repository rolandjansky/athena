/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_ELECTRONVALIDATIONPLOTS_H
#define EGAMMAPHYSVALMONITORING_ELECTRONVALIDATIONPLOTS_H


#include "GaudiKernel/ToolHandle.h"
#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "ElectronPlots.h"
#include "ElectronFrwdPlots.h"
#include "KinematicsPlots.h"
#include "xAODEgamma/Electron.h"
#include "xAODTruth/TruthParticle.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

class ElectronValidationPlots:public PlotBase {
    public:
      ElectronValidationPlots(PlotBase* pParent, const std::string& sDir);
      void fill(const xAOD::Electron& electron, const xAOD::EventInfo& eventInfo, bool isPrompt);

      std::string m_sParticleType;

      Egamma::ElectronPlots           m_oCentralElecPlots;
      Egamma::ElectronFrwdPlots       m_oFrwdElecPlots;
      Egamma::KinematicsPlots         m_oTruthAllPlots;
      Egamma::KinematicsPlots         m_oTruthAllIsoPlots;
      Egamma::KinematicsPlots         m_oTruthIsoPlots;
       
      TH1* author;
      TH1* mu_average;
      TH1* mu_actual;
      TProfile* res_et;
      TProfile* res_eta;
      TProfile* res_et_cut;
      TProfile* res_eta_cut;
      TH2* matrix;

 private:
      virtual void initializePlots();

};

#endif
