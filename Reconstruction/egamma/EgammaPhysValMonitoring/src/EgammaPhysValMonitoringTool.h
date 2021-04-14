///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// EgammaPhysValMonitoringTool.h 
// Header file for class EgammaPhysValMonitoringTool
// Author: 
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAPHYSVALMONITORING_EGAMMAPHYVALMONITORINGTOOL_H
#define EGAMMAPHYSVALMONITORING_EGAMMAPHYVALMONITORINGTOOL_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"

// Local includes
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

// Root includes
#include "ElectronValidationPlots.h"
#include "PhotonValidationPlots.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaAuxContainer.h"

#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthParticleAuxContainer.h"

#include "xAODEventInfo/EventInfo.h"

#include "StoreGate/ReadHandleKey.h"

#include "CLHEP/Units/SystemOfUnits.h"

class IMCTruthClassifier;
namespace EgammaPhysValMonitoring {


class EgammaPhysValMonitoringTool
  : public ManagedMonitorToolBase
{ 
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  EgammaPhysValMonitoringTool( const std::string& type,
		  const std::string& name, 
		  const IInterface* parent );

  /// Destructor: 
  virtual ~EgammaPhysValMonitoringTool(); 

  // Athena algtool's Hooks
  virtual StatusCode initialize();
  virtual StatusCode bookHistograms();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();


  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 


  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  EgammaPhysValMonitoringTool() = delete;

  StatusCode fillRecoElecHistograms(const xAOD::TruthParticleContainer* truthParticles, const xAOD::EventInfo* eventInfo);
  StatusCode fillRecoFrwdElecHistograms(const xAOD::TruthParticleContainer* truthParticles, const xAOD::EventInfo* eventInfo);
  StatusCode fillRecoPhotHistograms(const xAOD::TruthParticleContainer* truthParticles, const xAOD::EventInfo* eventInfo);

  const xAOD::TruthParticle* Match(const xAOD::Egamma* particle, int pdg,
				   const xAOD::TruthParticleContainer* truthParticles) const;


  // Containers
  SG::ReadHandleKey<xAOD::EventInfo> m_EventInfoContainerKey {this,
      "EventInfoContainerName", "EventInfo", "Input event information container"};
  SG::ReadHandleKey<xAOD::PhotonContainer> m_photonContainerKey {this,
      "PhotonContainerName", "Photons", "Input photon container"};
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronContainerKey {this,
      "ElectronContainerName", "Electrons", "Input electron container"};
  SG::ReadHandleKey<xAOD::ElectronContainer> m_electronContainerFrwdKey {this,
      "ElectronContainerFrwdName", "ForwardElectrons", "Input forward electron container"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainerKey {this,
      "TruthParticleContainerName", "TruthParticles", "Input global truth particles"};
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_egammaTruthContainerKey {this,
      "EgammaTruthContainerName", "egammaTruthParticles", "Input egamma truth particles"};

  Gaudi::Property<bool> m_isMC {this, "isMC", false, "Should be set from file peeking"};

  // Hists
  ElectronValidationPlots m_oElectronValidationPlots;
  PhotonValidationPlots m_oPhotonValidationPlots;
  
  ToolHandle<IMCTruthClassifier>  m_truthClassifier {this,
      "MCTruthClassifier", "EMMCTruthClassifier", "Handle of MCTruthClassifier"};

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !EGAMMAPHYSVALMONITORING_EGAMMAPHYVALMONITORINGTOOL_H

