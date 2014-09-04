/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsSensitiveDetector/FadsSensitiveDetector.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

namespace FADS {

std::string FadsSensitiveDetector::GetName()
{
	return name;
}

void FadsSensitiveDetector::Register()
{
	G4SDManager* SDman = G4SDManager::GetSDMpointer();
        SDman->AddNewDetector(this);
}

  FadsSensitiveDetector::FadsSensitiveDetector(std::string n): G4VSensitiveDetector(n),name(n),currentStep(0),m_msg(n),m_allowMods(false)
{
	ATH_MSG_DEBUG( "c-tor called for " << name );
	SensitiveDetectorCatalog *sdc=SensitiveDetectorCatalog::GetSensitiveDetectorCatalog();
	ATH_MSG_DEBUG( "Registering sensitive detector list " << n );
	sdc->RegisterSensitiveDetector(this);
}

FadsSensitiveDetector::~FadsSensitiveDetector() {
}

G4bool FadsSensitiveDetector::ProcessHits(G4Step* step,G4TouchableHistory*)
{
	currentStep=step;
	RetrieveVolumeTree();
	ProcessStep();
	return true;
}


void FadsSensitiveDetector::RetrieveVolumeTree()
{
	G4TouchableHistory *th=(G4TouchableHistory *)currentStep->GetPreStepPoint()
                        ->GetTouchable();
 
	for (int i=0;i<th->GetHistoryDepth();i++)
	{
		int id=th->GetVolume(i)->GetCopyNo();
		if (id!=INT_MIN)
		{
        		volumeIdentifiers.push_front(th->GetVolume(i)->GetCopyNo());
        		volumeNames.push_front(th->GetVolume(i)->GetName());
		}
	} 
}

G4ThreeVector FadsSensitiveDetector::InitialStepPosition(coordinates c)
{
	G4StepPoint *preStepPoint=currentStep->GetPreStepPoint();
	G4ThreeVector globalCoordinates=preStepPoint->GetPosition();
	if (c==WorldCoordinates) return globalCoordinates;
	
	G4TouchableHistory *theTouchable=(G4TouchableHistory *)
                                 (preStepPoint->GetTouchable());
	G4ThreeVector localCoordinates=theTouchable->GetHistory()
                ->GetTopTransform().TransformPoint(globalCoordinates);
	return localCoordinates; 
}
G4ThreeVector FadsSensitiveDetector::FinalStepPosition(coordinates c)
{
	G4StepPoint *postStepPoint=currentStep->GetPostStepPoint();
	G4ThreeVector globalCoordinates=postStepPoint->GetPosition();
	if (c==WorldCoordinates) return globalCoordinates;
	
	G4TouchableHistory *theTouchable=(G4TouchableHistory *)
                                 (postStepPoint->GetTouchable());
	G4ThreeVector localCoordinates=theTouchable->GetHistory()
                ->GetTopTransform().TransformPoint(globalCoordinates);
	return localCoordinates; 
}

}	// end namespace
