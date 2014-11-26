/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_ELECTRONFRWDPLOTS_H
#define EGAMMAPHYSVALMONITORING_ELECTRONFRWDPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "KinematicsPlots.h"
#include "ClusMomentumPlots.h"
#include "IsolationPlots.h"
#include "TrackPlots.h"
//#include "TrkValHistUtils/EfficiencyPlots.h"

#include "xAODEgamma/Electron.h"

namespace Egamma{

class ElectronFrwdPlots:public PlotBase {
    public:
      ElectronFrwdPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Electron& electron, bool isPrompt);

      // Reco only information
      Egamma::KinematicsPlots     m_oKinFrwdAllRecoPlots;
      Egamma::KinematicsPlots     m_oKinFrwdIsoRecoPlots;
      Egamma::KinematicsPlots     m_oKinFrwdTightPlots;
      Egamma::ClusMomentumPlots   m_oClustMomAllRecoPlots;
      Egamma::ClusMomentumPlots   m_oClustMomIsoRecoPlots;
      
      
      TH1* nParticles;

      std::string m_sParticleType;

    private:
      virtual void initializePlots();
      


};

}

#endif
