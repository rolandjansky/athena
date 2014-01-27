/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////////////
//
// DESCRIPTION:
// Subject: This class assoziates MSTrackObjects to other
//	    Tracks/Particles/... in AOD-Containers
///////////////////////////////////////////////////////////////////////////


#ifndef MSTRACKASSOCIATOR_H
#define MSTRACKASSOCIATOR_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ObjectVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "McParticleEvent/TruthParticle.h"
 
#include "MuonDQAUtils/MSEfficiency.h"
#include "MuonDQAUtils/MSEvent.h"
#include "MuonDQAUtils/MSPhysicsObject.h"
#include "MuonDQAUtils/MSTrackObject.h"

//#include "CBNT_Utils/CBNT_AthenaAwareBase.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include <string>
 
namespace Analysis { class MuonContainer; }

class MSTrackAssociator
{
 public:
  MSTrackAssociator();
  ~MSTrackAssociator();

  // Setting up the class
  void	setup(	StoreGateSvc *storeGate,	// Which Storegate should be used?
		double MaxConeDistance,		// What is the maximal Distance to assoziate one Track to another
		double MinimalMuonPt);		// What is the minimal PT of Tracks that should be considered?

  // The following functions do thet Track-/Trigger-/Particle-Assoziation
  bool assoziateMuonCollectionTrack(MSTrackObject &object, string MuonCollectionName);
  bool assoziateTrackContainerTrack(MSTrackObject &object, string TrackContainerName);
  bool assoziateTriggerInformation(MSTrackObject &object);
  bool assoziateTruthTrack(MSTrackObject &object);

  // These functions are for determining certain Isolation Criteria for Track
  bool assoziateJetEnergyInCone(MSTrackObject &object, double coneDistance_max);
  bool assoziateInnerTrackPropertiesInCone(MSTrackObject &object, double coneDistance_min, double coneDistance_max);
  bool assoziateElectronInCone(MSTrackObject &object, double coneDistance_max);

 private:
  double	distance(MSPhysicsObject &ob1, MSPhysicsObject &ob2);

  StoreGateSvc	*m_storeGate;
  double	m_MaxConeDistance;
  double	m_MinimalMuonPt;
  bool          m_dotruth;
};

#endif
