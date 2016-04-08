/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAPHYSVALMONITORING_ELECTRONPLOTS_H
#define EGAMMAPHYSVALMONITORING_ELECTRONPLOTS_H

#include "GaudiKernel/ToolHandle.h"

#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "KinematicsPlots.h"
#include "ShowerShapesPlots.h"
#include "IsolationPlots.h"
#include "TrackPlots.h"
#include "TrkValHistUtils/EfficiencyPlots.h"
#include "MCTruthClassifier/IMCTruthClassifier.h"

#include "xAODEgamma/Electron.h"
#include "xAODTruth/TruthParticle.h"

namespace Egamma{

class ElectronPlots:public PlotBase {
    public:
      ElectronPlots(PlotBase* pParent, std::string sDir, std::string sParticleType);
      void fill(const xAOD::Electron& electron, bool isPrompt);
     // Reco only information

      Egamma::KinematicsPlots     m_oKinAllRecoPlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesAllRecoPlots;
      Egamma::IsolationPlots      m_oIsolationAllRecoPlots;
      Egamma::TrackPlots          m_oTrackAllRecoPlots;
      Egamma::KinematicsPlots     m_oKinIsoRecoPlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesIsoRecoPlots;
      Egamma::IsolationPlots      m_oIsolationIsoRecoPlots;
      Egamma::TrackPlots          m_oTrackIsoRecoPlots;

      Egamma::KinematicsPlots     m_oKinIsoLoosePPPlots;
      Egamma::KinematicsPlots     m_oKinIsoMediumPPPlots;
      Egamma::KinematicsPlots     m_oKinIsoTightPPPlots;
      Egamma::KinematicsPlots     m_oKinIsoLHLoosePlots;
      Egamma::KinematicsPlots     m_oKinIsoLHMediumPlots;
      Egamma::KinematicsPlots     m_oKinIsoLHTightPlots;       
   
      TH1* nParticles;
      TH1* nTypeParticles;
      std::string m_sParticleType;

    private:
      virtual void initializePlots();
      
      bool Match(const xAOD::Egamma& particle);

};

}

#endif
