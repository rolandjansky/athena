/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "G4AtlasControl/FieldMenu.h"

#include "FadsField/FieldManager.h"
#include "FadsField/FieldManagerStore.h"
#include "FadsField/MagneticFieldMap.h"
#include "FadsField/FadsFieldManager.h"

#include "G4Field/FieldMapMgr.h"

#include "G4LogicalVolumeStore.hh"
#include "G4FieldManager.hh"
#include "G4MagneticField.hh"
#include "G4ChordFinder.hh"
#include "G4MagIntegratorDriver.hh"

FieldMenu::FieldMenu()
  :SimpleMenu("FieldMenu")
  ,m_defaultStepper("AtlasRK4")
{
}

void FieldMenu::list()
{
	FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
	fm->PrintFieldList();
}

void FieldMenu::initialize()
{
	FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
	fm->InitializeField();
}
void FieldMenu::select(const std::string fieldName)
{
	FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
	fm->SelectMagneticField(fieldName);
}
void FieldMenu::fieldmap(const std::string fieldMapfilename)
{
        FieldMapMgr *fmapm=FieldMapMgr::GetFieldMapMgr();
        fmapm->SetFieldMapFile(fieldMapfilename);
	//log() << MSG::VERBOSE << "fieldmap  "<<fieldMapfilename << endreq;
}
void FieldMenu::assign(const std::string fieldMap, const std::string name, const std::string volName)
{
	std::string nname=name;
	FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
	if (nname=="*") nname=fieldMap;
 	// step one - get the field map, create it, initialize it
	FADS::MagneticFieldMap* magField=fm->GetMagneticField(fieldMap);
	if (!magField) return;
	G4MagneticField* theMap;
	theMap=magField->Create(nname);
	//log() << MSG::VERBOSE << "Field map pointer: "<<theMap << endreq;
	if (!magField->IsInitialized()) magField->SetInitialValues();
	FADS::FadsFieldManager* fieldMgr=new FADS::FadsFieldManager(volName);

	fieldMgr->SetDetectorField(theMap);
	fieldMgr->CreateChordFinder(theMap);
	//fieldMgr->SetField(nname);

    // Apply the current stepper to this field manager
    if (fm->GetCurrentStepper()){
      fieldMgr->GetChordFinder()->GetIntegrationDriver()->RenewStepperAndAdjust(fm->GetCurrentStepper());
    } else { // Default value
      fieldMgr->GetChordFinder()->GetIntegrationDriver()->RenewStepperAndAdjust(fm->GetStepper(m_defaultStepper));
    }

	// step two - fish for the volume
	G4LogicalVolumeStore& lvs=*(G4LogicalVolumeStore::GetInstance());
	int ndone=0;
	for (unsigned int i=0;i<lvs.size();i++)
	{
		G4LogicalVolume *lv=lvs[i];
		if ((std::string)lv->GetName() == volName) 
		{
			lv->SetFieldManager(fieldMgr,true);
			ndone++;
		}
	}
	if (ndone) 
	{
            	log() << MSG::INFO << "Field map "<<fieldMap<<" assigned to "<<
		  ndone<<" copies of volume "<<volName << endreq;
	}
	else 
	{
		log() << MSG::WARNING << " Warning: volume "<<volName<<" not found "
		      <<"in the store. Nothing done " << endreq;
		delete fieldMgr;
		magField->Terminate();
	}
}
void FieldMenu::setDeltaOneStep(double value)
{
        FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
        fm->SetDeltaOneStep(value);
}
void FieldMenu::setDeltaIntersection(double value)
{
        FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();        
        fm->SetDeltaIntersection(value);
}
void FieldMenu::setMinimumEpsilonStep(double value)
{
        FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();        
        fm->SetMinimumEpsilonStep(value);
}
void FieldMenu::setMaximumEpsilonStep(double value)
{
        FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();        
        fm->SetMaximumEpsilonStep(value);
}
void FieldMenu::setDeltaOneStep(const std::string volName,double value)
{
        FADS::FieldManagerStore *fms=FADS::FieldManagerStore::GetFieldManagerStore();
  		FADS::FadsFieldManager *fm=fms->GetFieldManager(volName);
        fm->SetDeltaOneStep(value);
}
void FieldMenu::setDeltaIntersection(const std::string volName,double value)
{
        FADS::FieldManagerStore *fms=FADS::FieldManagerStore::GetFieldManagerStore();
  		FADS::FadsFieldManager *fm=fms->GetFieldManager(volName);        
        fm->SetDeltaIntersection(value);
}
void FieldMenu::setMinimumEpsilonStep(const std::string volName,double value)
{
        FADS::FieldManagerStore *fms=FADS::FieldManagerStore::GetFieldManagerStore();
  		FADS::FadsFieldManager *fm=fms->GetFieldManager(volName);        
        fm->SetMinimumEpsilonStep(value);
}
void FieldMenu::setMaximumEpsilonStep(const std::string volName,double value)
{
        FADS::FieldManagerStore *fms=FADS::FieldManagerStore::GetFieldManagerStore();
  		FADS::FadsFieldManager *fm=fms->GetFieldManager(volName);       
        fm->SetMaximumEpsilonStep(value);
}

void FieldMenu::ListAvailableSteppers()
{
		FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
		fm->ListSteppers();
}

void FieldMenu::SetCurrentStepper(const std::string stepperName)
{
		FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
		fm->SetCurrentStepper( stepperName);
}

void FieldMenu::UseCurrentStepper()
{
		FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
		fm->UseCurrentStepper();
}

void FieldMenu::UseStepper(const std::string stepperName)
{
		FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
		fm->UseStepper( stepperName);
}

void FieldMenu::UseStepper(const std::string volName, const std::string stepperName)
{
 		FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();

 		FADS::FieldManagerStore *fms=FADS::FieldManagerStore::GetFieldManagerStore();
 		FADS::FadsFieldManager *ffm=fms->GetFieldManager(volName);
 		ffm->SetStepper(fm->GetStepper(stepperName));
}

void FieldMenu::SetEquationOfMotion(const std::string equationName)
{
		FADS::FieldManager *fm=FADS::FieldManager::GetFieldManager();
		fm->EquationOfMotion( equationName);
}

