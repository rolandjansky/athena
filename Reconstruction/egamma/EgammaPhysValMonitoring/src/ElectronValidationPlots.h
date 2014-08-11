/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
      ElectronValidationPlots(PlotBase* pParent, std::string sDir);
      void fill(const xAOD::Electron& electron, bool isPrompt);

      std::string m_sParticleType;

      Egamma::ElectronPlots           m_oCentralElecPlots;
      Egamma::ElectronFrwdPlots       m_oFrwdElecPlots;
      Egamma::KinematicsPlots         m_oTruthPlots;
      
      TH1* author;
      

 private:
      virtual void initializePlots();

};

#endif
