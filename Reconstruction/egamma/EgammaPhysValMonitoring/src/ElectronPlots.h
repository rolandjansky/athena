/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
      ElectronPlots(PlotBase* pParent, const std::string& sDir, const std::string& sParticleType);
      void fill(const xAOD::Electron& electron, const xAOD::EventInfo& eventInfo, bool isPrompt) const;
     // Reco only information

      Egamma::KinematicsPlots     m_oKinAllRecoPlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesAllRecoPlots;
      Egamma::IsolationPlots      m_oIsolationAllRecoPlots;
      Egamma::TrackPlots          m_oTrackAllRecoPlots;
      Egamma::KinematicsPlots     m_oKinIsoRecoPlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesIsoRecoPlots;
      Egamma::IsolationPlots      m_oIsolationIsoRecoPlots;
      Egamma::TrackPlots          m_oTrackIsoRecoPlots;
  
      Egamma::KinematicsPlots     m_oKinIsoLHLoosePlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesIsoLHLoosePlots;
      Egamma::IsolationPlots      m_oIsolationIsoLHLoosePlots;
      Egamma::TrackPlots          m_oTrackIsoLHLoosePlots;
      Egamma::KinematicsPlots     m_oKinIsoLHMediumPlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesIsoLHMediumPlots;
      Egamma::IsolationPlots      m_oIsolationIsoLHMediumPlots;
      Egamma::TrackPlots          m_oTrackIsoLHMediumPlots;
      Egamma::KinematicsPlots     m_oKinIsoLHTightPlots;
      Egamma::ShowerShapesPlots   m_oShowerShapesIsoLHTightPlots;
      Egamma::IsolationPlots      m_oIsolationIsoLHTightPlots;
      Egamma::TrackPlots          m_oTrackIsoLHTightPlots;
   
      TH1* nParticles;
      TH1* nParticles_weighted;
      TH1* nTypeParticles;
      std::string m_sParticleType;

    private:
      virtual void initializePlots();
      
      bool Match(const xAOD::Egamma& particle);

};

}

#endif
