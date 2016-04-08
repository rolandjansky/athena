///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  int m_eventCounter;
  bool is_MC;

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
  EgammaPhysValMonitoringTool();

 StatusCode fillRecoElecHistograms();
 StatusCode fillRecoFrwdElecHistograms();
 StatusCode fillRecoPhotHistograms();

 const xAOD::TruthParticle* Match(const xAOD::Egamma* particle, int pdg);


  // Containers
  std::string m_photonContainerName;
  std::string m_electronContainerName;
  std::string m_electronContainerFrwdName;
  std::string m_truthParticleContainerName;
  std::string m_egammaTruthContainerName;

  // Hists
  ElectronValidationPlots m_oElectronValidationPlots;
  PhotonValidationPlots m_oPhotonValidationPlots;
  
  ToolHandle<IMCTruthClassifier>        m_truthClassifier;

  const xAOD::TruthParticleContainer* m_truthallParticles;
  const xAOD::TruthParticleContainer* m_truthParticles;
}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 
}

#endif //> !EGAMMAPHYSVALMONITORING_EGAMMAPHYVALMONITORINGTOOL_H

