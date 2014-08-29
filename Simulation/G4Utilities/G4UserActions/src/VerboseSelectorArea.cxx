/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/VerboseSelectorArea.h"
#include <cstdlib>
#include <iostream>

static VerboseSelectorArea vs("VerboseSelectorArea");

void VerboseSelectorArea::BeginOfEventAction(const G4Event*)
{
  m_nev++;
}

void VerboseSelectorArea::EndOfEventAction(const G4Event*)
{;}

void VerboseSelectorArea::BeginOfRunAction(const G4Run*)
{
  ParseProperties();  
}

void VerboseSelectorArea::ParseProperties(){

  // here we read the property map and set the needed variables

  // first fill missing properties and issue warnings

  if(theProperties.find("Xmin")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no Xmin specified, setting to default (=0)"<<endreq;
    theProperties["Xmin"]="0.0";
  };
  if(theProperties.find("Xmax")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no Xmax specified, setting to default (=0)"<<endreq;
    theProperties["Xmax"]="0.0";
  };
  if(theProperties.find("Ymin")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no Ymin specified, setting to default (=0)"<<endreq;
    theProperties["Ymin"]="0.0";
  };
  if(theProperties.find("Ymax")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no Ymax specified, setting to default (=0)"<<endreq;
    theProperties["Ymax"]="0.0";
  };
  if(theProperties.find("Zmin")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no Zmin specified, setting to default (=0)"<<endreq;
    theProperties["Zmin"]="0.0";
  };
  if(theProperties.find("Zmax")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no Zmax specified, setting to default (=0)"<<endreq;
    theProperties["Zmax"]="0.0";
  };

  if(theProperties.find("targetEvent")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no targetTrack specified, setting to default (=1)"<<endreq;
    theProperties["targetEvent"]="1";
  };

  if(theProperties.find("verboseLevel")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no verboseLevel specified, setting to default (=0)"<<endreq;
    theProperties["verboseLevel"]="0";
  };
  if(theProperties.find("Continue")==theProperties.end()){
    log() << MSG::DEBUG <<"VerboseSelector: no continue specified, setting to default (=0)"<<endreq;
    theProperties["Continue"]="0";
  };

  // now set members

  verboseLevel=strtol(theProperties["verboseLevel"].c_str(),0,0);
  m_Xmin = strtod(theProperties["Xmin"].c_str(),0);
  m_Xmax = strtod(theProperties["Xmax"].c_str(),0);
  m_Ymin = strtod(theProperties["Ymin"].c_str(),0);
  m_Ymax = strtod(theProperties["Ymax"].c_str(),0);
  m_Zmin = strtod(theProperties["Zmin"].c_str(),0);
  m_Zmax = strtod(theProperties["Zmax"].c_str(),0);
  m_Event= strtol(theProperties["targetEvent"].c_str(),0,0);
  m_verb = strtol(theProperties["targetEvent"].c_str(),0,0);
  if (m_verb>1) m_verb=1;

  log() << MSG::INFO <<"VerboseSelector: properties are targetEvent="<<m_Event
	   <<" verboseLevel="<<verboseLevel<<" continue="<<m_verb
           <<" "<<m_Xmin<<"<x<"<<m_Xmax
           <<" "<<m_Ymin<<"<y<"<<m_Ymax
           <<" "<<m_Zmin<<"<z<"<<m_Zmax
	   <<endreq;
}

void VerboseSelectorArea::EndOfRunAction(const G4Run*)
{;}

#include "G4TransportationManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4VProcess.hh"
#include "G4LogicalVolume.hh"
#include "G4PropagatorInField.hh"

void VerboseSelectorArea::SteppingAction(const G4Step* aStep)
{
  if(m_nev==m_Event||m_Event<0){
    G4ThreeVector myPos = aStep->GetPostStepPoint()->GetPosition();
    if ( ( myPos.x() < m_Xmax && myPos.x() > m_Xmin &&
           myPos.y() < m_Ymax && myPos.y() > m_Ymin &&
           myPos.z() < m_Zmax && myPos.z() > m_Zmin ) || 
         m_verb==2){

      G4TransportationManager *tm = G4TransportationManager::GetTransportationManager();
      tm->GetNavigatorForTracking()->SetVerboseLevel(verboseLevel);
      tm->GetPropagatorInField()->SetVerboseLevel(verboseLevel);

      G4RunManagerKernel *rmk = G4RunManagerKernel::GetRunManagerKernel();
      rmk->GetTrackingManager()->SetVerboseLevel(verboseLevel);
      rmk->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(verboseLevel);
      rmk->GetStackManager()->SetVerboseLevel(verboseLevel);

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

