/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Interface class of TauTriggerEmulationTool
// author Quentin Buat <quentin.buat@no.spam.cern.ch>
#ifndef HLTEMULATIONTOOL_IHLTEMULATIONTOOL_H
#define HLTEMULATIONTOOL_IHLTEMULATIONTOOL_H


#include "PATInterfaces/CorrectionCode.h"
#include "AsgTools/IAsgTool.h"

// EDM Include
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "TrigTauEmulation/DecoratedHltTau.h"

// fwd declarations

namespace TrigTauEmul {
class IHltEmulationTool : virtual public asg::IAsgTool

{

  // Declare the interface that the class provides 
  ASG_TOOL_INTERFACE(IHltEmulationTool)

  public:

  /// virtual destructor
  virtual ~IHltEmulationTool() {};
 
  virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus,
               const xAOD::JetRoIContainer* l1jets,
               const xAOD::MuonRoIContainer* l1muons,
               const xAOD::EnergySumRoI* l1xe,
               const std::vector<DecoratedHltTau>& hlt_taus) = 0;


  //virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus, 
				 //const xAOD::JetRoIContainer* l1jets,
				 //const xAOD::MuonRoIContainer* l1muons,
				 //const xAOD::EnergySumRoI* l1xe,
				 //const xAOD::TauJetContainer* hlt_taus,
				 //const DataVector<xAOD::TrackParticle>* preselTracksIso,
				 //const DataVector<xAOD::TrackParticle>* preselTracksCore) = 0;

  //virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus, 
				 //const xAOD::JetRoIContainer* l1jets,
				 //const xAOD::MuonRoIContainer* l1muons,
				 //const xAOD::EnergySumRoI* l1xe,
				 //const xAOD::TauJetContainer* hlt_taus,
				 //const DataVector<xAOD::TrackParticle>* fast_tracks) = 0;

  //virtual StatusCode calculate(const xAOD::TauJetContainer* hlt_taus,
				   //const xAOD::EmTauRoIContainer* l1taus,
				   //const DataVector<xAOD::TrackParticle> * fast_tracks) = 0;
  
  //virtual StatusCode calculate(const xAOD::TauJetContainer* hlt_taus,
				   //const xAOD::EmTauRoIContainer* l1taus,
				   //const DataVector<xAOD::TrackParticle> * preselTracksIso,
				   //const DataVector<xAOD::TrackParticle> * preselTracksCore) = 0;

  //virtual StatusCode execute(const xAOD::EmTauRoIContainer* l1taus, 
				 //const xAOD::JetRoIContainer* l1jets,
				 //const xAOD::MuonRoIContainer* l1muons,
				 //const xAOD::EnergySumRoI* l1xe,
				 //const xAOD::TauJetContainer* hlt_taus,
				 //const xAOD::TauJetContainer* presel_taus) = 0;

  //virtual StatusCode calculate(const xAOD::TauJetContainer* hlt_taus,
				   //const xAOD::TauJetContainer* presel_taus,
				   //const xAOD::EmTauRoIContainer* l1taus) = 0;

  virtual bool decision(const std::string & chain_name) = 0;

};
}
#endif
