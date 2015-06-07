/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPlots.h"
#include "MCTruthClassifier/MCTruthClassifierDefs.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include <iostream>
using namespace std;
using namespace MCTruthPartClassifier;

namespace Egamma{

ElectronPlots::ElectronPlots(PlotBase* pParent, std::string sDir, 
			     std::string sParticleType):PlotBase(pParent, sDir),
							m_oKinAllRecoPlots(this, "All/KinPlots/", "All Reco "+ sParticleType +" Electron"),
							m_oShowerShapesAllRecoPlots(this, "All/ShowerShapesPlots/","All Reco "+ sParticleType +" Electron"  ),
							m_oIsolationAllRecoPlots(this, "All/IsolationPlots/", "All Reco "+ sParticleType +"  Electron" ),
							m_oTrackAllRecoPlots(this, "All/TrackPlots/", "All Reco "+ sParticleType +"  Electron"  ),
							m_oKinIsoRecoPlots(this, "Iso/KinPlots/", "Iso Reco "+ sParticleType +"  Electron"),
							m_oShowerShapesIsoRecoPlots(this, "Iso/ShowerShapesPlots/","Iso Reco "+ sParticleType +"  Electron" ),
							m_oIsolationIsoRecoPlots(this, "Iso/IsolationPlots/", "Iso Reco "+ sParticleType +"  Electron" ),
							m_oTrackIsoRecoPlots(this, "Iso/TrackPlots/", "Iso Reco "+ sParticleType +"  Electron" ),
							m_oKinIsoLoosePPPlots(this, "IsoLoosePP/KinPlots/", "LoosePP "+ sParticleType +"   Electron"),
							m_oKinIsoMediumPPPlots(this, "IsoMediumPP/KinPlots/", "MediumPP "+ sParticleType +"   Electron"),
							m_oKinIsoTightPPPlots(this, "IsoTightPP/KinPlots/", "TightPP "+ sParticleType +"   Electron"),
						        nParticles(0),
							nTypeParticles(0),
							m_sParticleType(sParticleType)
{}	

void ElectronPlots::initializePlots(){
  nParticles = Book1D("n", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + " electrons;Events", 15, 0, 15.);
//  nTypeParticles= Book1D("nType", "Number of"+ m_sParticleType + "s;#" + m_sParticleType + "s per type;Events", 15, 0, 15.);
 }

  void ElectronPlots::fill(const xAOD::Electron& electron, bool isPrompt) {

  m_oKinAllRecoPlots.fill(electron);
  m_oShowerShapesAllRecoPlots.fill(electron);
  m_oIsolationAllRecoPlots.fill(electron);
  m_oTrackAllRecoPlots.fill(electron);
 
  if(!isPrompt) return; 
   
  m_oKinIsoRecoPlots.fill(electron);
  m_oShowerShapesIsoRecoPlots.fill(electron);
  m_oIsolationIsoRecoPlots.fill(electron);
  m_oTrackIsoRecoPlots.fill(electron);
 
  bool val_loose=false;    
  electron.passSelection(val_loose, "Loose");
  if(val_loose) {
    m_oKinIsoLoosePPPlots.fill(electron);
  }

  bool val_med=false;  
  electron.passSelection(val_med, "Medium");
  if(val_med) {
    m_oKinIsoMediumPPPlots.fill(electron);
  }

  bool val_tight=false;    
  electron.passSelection(val_tight, "Tight");
  if(val_tight) {
    m_oKinIsoTightPPPlots.fill(electron);

  }

}


}
