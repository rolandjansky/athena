/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VerboseSelector.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruthBase/TruthStrategyManager.h"

#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PropagatorInField.hh"
#include "G4RunManagerKernel.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TransportationManager.hh"
#include "G4VProcess.hh"

#include <iostream>

VerboseSelector::VerboseSelector(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent), m_targetEvent(-1), m_targetTrack(-1), m_targetBarcode(-1), m_verboseLevel(0), m_evtCount(1), m_verb(1), m_Xmin(0), m_Xmax(0), m_Ymin(0), m_Ymax(0), m_Zmin(0), m_Zmax(0){

  declareProperty("TargetEvent",m_targetEvent);
  declareProperty("TargetTrack",m_targetTrack);
  declareProperty("TargetBarcode",m_targetBarcode);
  declareProperty("VerboseLevel",m_verboseLevel);
  declareProperty("Continue",m_verb);
  // properties for area check at step time
  declareProperty("Xmin",m_Xmin);
  declareProperty("Xmax",m_Xmax);

  declareProperty("Ymin",m_Ymin);
  declareProperty("Ymax",m_Ymax);

  declareProperty("Zmin",m_Zmin);
  declareProperty("Zmax",m_Zmax);

}



void VerboseSelector::Step(const G4Step* aStep)
{
  if(m_evtCount==m_targetEvent||m_targetEvent<0){
    G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
    if ( ( myPos.x() < m_Xmax && myPos.x() > m_Xmin &&
           myPos.y() < m_Ymax && myPos.y() > m_Ymin &&
           myPos.z() < m_Zmax && myPos.z() > m_Zmin ) ||
         m_verb==2){

      G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
      tm->GetNavigatorForTracking()->SetVerboseLevel(m_verboseLevel);
      tm->GetPropagatorInField()->SetVerboseLevel(m_verboseLevel);

      G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
      rmk->GetTrackingManager()->SetVerboseLevel(m_verboseLevel);
      rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(m_verboseLevel);
      rmk->GetStackManager()->SetVerboseLevel(m_verboseLevel);

      G4Track *tr = aStep->GetTrack();
      G4ThreeVector mom = tr->GetMomentumDirection();

      std::cout << "Moving " << tr->GetDefinition()->GetParticleName() << " at (" << myPos.x()
                << ", " << myPos.y() << ", " << myPos.z() << ") to (" << mom.x() << ", " << mom.y()
                << ", " << mom.z() << ") from ";
      if (aStep->GetPreStepPoint()->GetPhysicalVolume()) std::cout << aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
      else std::cout << "noVolume";
      std::cout << " to ";
      if (aStep->GetPostStepPoint()->GetPhysicalVolume()) std::cout << aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
      else std::cout << "outOfWorld";
      std::cout << " with KE=" << tr->GetKineticEnergy() << " pT=" << tr->GetMomentum().perp()
                << " eta=" << tr->GetMomentum().eta() << " length " << aStep->GetStepLength() << " energy "
                << aStep->GetTotalEnergyDeposit() << " with process ";
      if (aStep->GetPostStepPoint()->GetProcessDefinedStep()) std::cout << aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
      else std::cout << "Unknown";
      std::cout << " from a ";
      if (tr->GetCreatorProcess()) std::cout << tr->GetCreatorProcess()->GetProcessName();
      else if (tr->GetParentID()==0) std::cout << "Generator";
      else std::cout << "pid="<<tr->GetParentID();
      if (tr->GetLogicalVolumeAtVertex()) std::cout << " in " << tr->GetLogicalVolumeAtVertex()->GetName() << ", ";
      else std::cout << " nowhere, ";
      std::cout << tr->GetTrackLength() << " mm ago" << std::endl;

      if (m_verb==1) m_verb=2;

    }
  }
}





StatusCode VerboseSelector::initialize(){

  ATH_MSG_INFO("VerboseSelector: properties are TargetEvent="<<m_targetEvent
               <<" TargetTrack="<<m_targetTrack
               <<" TargetBarcode="<<m_targetBarcode
               <<" VerboseLevel="<<m_verboseLevel );
  ATH_MSG_INFO( "VerboseSelector: area properties are:  continue="<<m_verb
                <<" "<<m_Xmin<<"<x<"<<m_Xmax
                <<" "<<m_Ymin<<"<y<"<<m_Ymax
                <<" "<<m_Zmin<<"<z<"<<m_Zmax );

  return StatusCode::SUCCESS;

}

void VerboseSelector::EndOfEvent(const G4Event*)
{
  m_evtCount++;
}

void VerboseSelector::PreTracking(const G4Track* aTrack){

  // std::cout<<" this is VerboseSelector::PreUserTrackingAction "<<std::endl;
  if(m_evtCount==m_targetEvent||m_targetEvent<0)
    {
      int trackID = aTrack->GetTrackID();
      G4Track *itr=const_cast<G4Track*>(aTrack);
      TrackHelper trackHelper(itr);
      EventInformation* eventInfo=TruthStrategyManager::GetStrategyManager()->
        GetEventInformation();

      //int primaryBarcode(0);
      int currentBarcode(0);

      if (trackHelper.IsPrimary() || trackHelper.IsRegisteredSecondary())
        {
          //primaryBarcode=eventInfo->GetCurrentPrimary()->barcode();
          currentBarcode=eventInfo->GetCurrentlyTraced()->barcode();
        }
      bool p1=m_targetTrack<0 && m_targetBarcode<0;
      bool p2=trackID==m_targetTrack ;
      bool p3=currentBarcode==m_targetBarcode;

      if(p1 || p2 || p3)
        {
          ATH_MSG_INFO(std::endl<<"---------> Dumping now track #"<<trackID<<" barcode "
                       <<currentBarcode<<" in event "<<m_evtCount<<std::endl);
          G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(m_verboseLevel);
        }
    }

}

void VerboseSelector::PostTracking(const G4Track* aTrack)
{
  if(m_evtCount==m_targetEvent||m_targetEvent<0){
    if(aTrack->GetTrackID()==m_targetTrack||m_targetTrack<0)
      G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(0);
  }
}


StatusCode VerboseSelector::queryInterface(const InterfaceID& riid, void** ppvInterface)
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
