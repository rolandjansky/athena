/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPlots.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"
#include <iostream>
using namespace std;
using namespace MCTruthPartClassifier;

namespace Egamma{

ElectronPlots::ElectronPlots(PlotBase* pParent, const std::string& sDir,
			     const std::string& sParticleType):PlotBase(pParent, sDir),
							m_oKinAllRecoPlots(this, "All/KinPlots/", "All Reco "+ sParticleType +" Electron"),
							m_oShowerShapesAllRecoPlots(this, "All/ShowerShapesPlots/","All Reco "+ sParticleType +" Electron"  ),
							m_oIsolationAllRecoPlots(this, "All/IsolationPlots/", "All Reco "+ sParticleType +"  Electron" ),
							m_oTrackAllRecoPlots(this, "All/TrackPlots/", "All Reco "+ sParticleType +"  Electron"  ),
							m_oKinIsoRecoPlots(this, "Iso/KinPlots/", "Iso Reco "+ sParticleType +"  Electron"),
							m_oShowerShapesIsoRecoPlots(this, "Iso/ShowerShapesPlots/","Iso Reco "+ sParticleType +"  Electron" ),
							m_oIsolationIsoRecoPlots(this, "Iso/IsolationPlots/", "Iso Reco "+ sParticleType +"  Electron" ),
							m_oTrackIsoRecoPlots(this, "Iso/TrackPlots/", "Iso Reco "+ sParticleType +"  Electron" ),
							
							m_oKinIsoLHLoosePlots(this, "IsoLHLoose/KinPlots/", "LHLoose "+ sParticleType +"   Electron"),
							m_oShowerShapesIsoLHLoosePlots(this, "IsoLHLoose/ShowerShapesPlots/","LHLoose "+ sParticleType +"  Electron" ),
							m_oIsolationIsoLHLoosePlots(this, "IsoLHLoose/IsolationPlots/", "LHLoose "+ sParticleType +"  Electron" ),
							m_oTrackIsoLHLoosePlots(this, "IsoLHLoose/TrackPlots/", "LHLoose "+ sParticleType +"  Electron" ),
                                                        m_oKinIsoLHMediumPlots(this, "IsoLHMedium/KinPlots/", "LHMedium "+ sParticleType +"   Electron"),
							m_oShowerShapesIsoLHMediumPlots(this, "IsoLHMedium/ShowerShapesPlots/","LHMedium "+ sParticleType +"  Electron" ),
							m_oIsolationIsoLHMediumPlots(this, "IsoLHMedium/IsolationPlots/", "LHMedium "+ sParticleType +"  Electron" ),
							m_oTrackIsoLHMediumPlots(this, "IsoLHMedium/TrackPlots/", "LHMedium "+ sParticleType +"  Electron" ),
                                                        m_oKinIsoLHTightPlots(this, "IsoLHTight/KinPlots/", "LHTight "+ sParticleType +"   Electron"),
							m_oShowerShapesIsoLHTightPlots(this, "IsoLHTight/ShowerShapesPlots/","LHTight "+ sParticleType +"  Electron" ),
							m_oIsolationIsoLHTightPlots(this, "IsoLHTight/IsolationPlots/", "LHTight "+ sParticleType +"  Electron" ),
							m_oTrackIsoLHTightPlots(this, "IsoLHTight/TrackPlots/", "LHTight "+ sParticleType +"  Electron" ),
						        nParticles(nullptr),
							nParticles_weighted(nullptr),
							nTypeParticles(nullptr),
							m_sParticleType(sParticleType)
{}

void ElectronPlots::initializePlots(){
  nParticles = Book1D("n", "Number of "+ m_sParticleType + "s;#" + m_sParticleType + " electrons;Events", 15, 0, 15.);
  nParticles_weighted = Book1D("n_weighted", "Number of "+ m_sParticleType + "s;#" + m_sParticleType + " electrons;Events", 15, 0, 15.);
 }

  void ElectronPlots::fill(const xAOD::Electron& electron, const xAOD::EventInfo& eventInfo, bool isPrompt) const {

  m_oKinAllRecoPlots.fill(electron,eventInfo);
  m_oShowerShapesAllRecoPlots.fill(electron,eventInfo);
  m_oIsolationAllRecoPlots.fill(electron,eventInfo);
  m_oTrackAllRecoPlots.fill(electron,eventInfo);

  if(!isPrompt) return;

  m_oKinIsoRecoPlots.fill(electron,eventInfo);
  m_oShowerShapesIsoRecoPlots.fill(electron,eventInfo);
  m_oIsolationIsoRecoPlots.fill(electron,eventInfo);
  m_oTrackIsoRecoPlots.fill(electron,eventInfo);

  bool val_LHloose=false;
  electron.passSelection(val_LHloose, "LHLoose");
  if(val_LHloose) {
    m_oKinIsoLHLoosePlots.fill(electron,eventInfo);
    m_oShowerShapesIsoLHLoosePlots.fill(electron,eventInfo);
    m_oIsolationIsoLHLoosePlots.fill(electron,eventInfo);
    m_oTrackIsoLHLoosePlots.fill(electron,eventInfo);
  }
  
  bool val_LHmed=false;
  electron.passSelection(val_LHmed, "LHMedium");
  if(val_LHmed) {
    m_oKinIsoLHMediumPlots.fill(electron,eventInfo);
    m_oShowerShapesIsoLHMediumPlots.fill(electron,eventInfo);
    m_oIsolationIsoLHMediumPlots.fill(electron,eventInfo);
    m_oTrackIsoLHMediumPlots.fill(electron,eventInfo);
  }
  
  bool val_LHtight=false;
  electron.passSelection(val_LHtight, "LHTight");
  if(val_LHtight) {
    m_oKinIsoLHTightPlots.fill(electron,eventInfo);
    m_oShowerShapesIsoLHTightPlots.fill(electron,eventInfo);
    m_oIsolationIsoLHTightPlots.fill(electron,eventInfo);
    m_oTrackIsoLHTightPlots.fill(electron,eventInfo);
  }
  }
  
  
}
