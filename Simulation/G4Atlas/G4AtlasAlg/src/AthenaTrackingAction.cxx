/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/AthenaTrackingAction.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/PrimaryParticleInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/TrackInformation.h"
#include "MCTruthBase/AtlasTrajectory.h"
#include "MCTruthBase/TruthStrategyManager.h"
#include <iostream>

#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4EventManager.hh"

#include <iostream>
// static AthenaTrackingAction ta;

AthenaTrackingAction::AthenaTrackingAction(const std::string& type, const std::string& name, const IInterface* parent):
  UserActionBase(type, name, parent),m_UASvc("UserActionSvc",name){

  declareProperty("UserActionService",m_UASvc);


}


StatusCode AthenaTrackingAction::initialize(){


  CHECK(m_UASvc.retrieve());

  m_UASvcQuick = &(*m_UASvc);

  return StatusCode::SUCCESS;

}



void AthenaTrackingAction::PreTracking(const G4Track* inTrack)
{
        static int ilevel=TruthStrategyManager::GetStrategyManager()->
                                        GetSecondarySavingLevel();
//	std::cout<<" this is AthenaTrackingAction::PreUserTrackingAction"<<std::endl;
        G4Track* inT = const_cast<G4Track*> (inTrack);
        TrackHelper trackHelper(inT);
        if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
        {
                HepMC::GenParticle* part=const_cast<HepMC::GenParticle*> (
                                        trackHelper.GetTrackInformation()->
                                                GetHepMCParticle());
                EventInformation *eventInfo=TruthStrategyManager::GetStrategyManager()->
                                        GetEventInformation();
                if (trackHelper.IsPrimary()) eventInfo->SetCurrentPrimary(part);
                eventInfo->SetCurrentlyTraced(part);
        }
        if (trackHelper.IsPrimary() ||
            (trackHelper.IsRegisteredSecondary()&&ilevel>1) ||
            (trackHelper.IsSecondary()&&ilevel>2))
        {
                AtlasTrajectory *temp=new AtlasTrajectory(inTrack);
                temp->setTrackingManager(m_UASvcQuick->TrackingManager());
                m_UASvcQuick->SetTrajectory(temp);
        }
}


void AthenaTrackingAction::PostTracking(const G4Track* /*outTrack*/)
{
//	std::cout <<"AthenaTrackingAction::PostUserTrackingAction"<<std::endl;
//	FADS::FadsTrackingAction::GetTrackingAction()->ResetTraj();
  m_UASvcQuick->ResetTrajectory();

}

StatusCode AthenaTrackingAction::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}
