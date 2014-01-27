/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION:
// Subject: This class assoziates MSTrackObjects to other
//	    Tracks/Particles/... in AOD-Containers
///////////////////////////////////////////////////////////////////////////


#ifndef MUONDQATRACKASSOCIATOR_H
#define MUONDQATRACKASSOCIATOR_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "McParticleEvent/TruthParticle.h"
 
//#include "MuonRecValidator/MSMuonAnalysis.h"

#include "MuonDQAUtils/MSEfficiency.h"
#include "MuonDQAUtils/MSRootInterface.h"

#include "MuonDQAUtils/MuonDQAEvent.h"
#include "MuonDQAUtils/MuonDQATrackObject.h"

#include "TH1.h"
#include <string>
 
namespace Analysis { class MuonContainer; }

class MuonDQATrackAssociator
{
 public:
  MuonDQATrackAssociator();
  ~MuonDQATrackAssociator();
  
  // Setting up the class
  void	setup(	StoreGateSvc *storeGate,	// Which Storegate should be used?
		double MaxConeDistance,		// What is the maximal Distance to assoziate one Track to another
		double MinimalMuonPt );		// What is the minimal PT of Tracks that should be considered?
  
  // The following functions do thet Track-/Trigger-/Particle-Assoziation
  bool assoziateMuonCollectionTrack( ToolHandle<Muon::MuonDQATrackObject> &object, string MuonCollectionName);
  bool assoziateTrackContainerTrack( ToolHandle<Muon::MuonDQATrackObject> &object, string TrackContainerName);
  bool assoziateTriggerInformation( ToolHandle<Muon::MuonDQATrackObject> &object);
  bool assoziateTruthTrack( ToolHandle<Muon::MuonDQATrackObject> &object);
  
  // These functions are for determining certain Isolation Criteria for Track
  bool assoziateJetEnergyInCone( ToolHandle<Muon::MuonDQATrackObject> &object, double coneDistance_max);
  bool assoziateInnerTrackPropertiesInCone( ToolHandle<Muon::MuonDQATrackObject> &object, double coneDistance_min, double coneDistance_max);
  bool assoziateElectronInCone( ToolHandle<Muon::MuonDQATrackObject> &object, double coneDistance_max);
  double distance( ToolHandle<Muon::MuonDQATrackObject> &object1, ToolHandle<Muon::MuonDQATrackObject> &object2 );
  
 private:
  StoreGateSvc	*m_storeGate;
  double   m_MaxConeDistance;
  double   m_MinimalMuonPt;
  bool     m_doTruth;
};

#endif
