/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/PhysicsMenu.h"
#include "G4AtlasControl/PhysicsRegionProxy.h"

#include "FadsPhysics/PhysicsListCatalog.h"
#include "FadsPhysics/PhysicsListSteering.h"
#include "FadsRun/FadsRunManager.h"
#include "FadsKinematics/GeneratorCenter.h"
#include "FadsField/RegionInformation.h"
#include "FadsField/FieldIntParameters.h"

#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4ProductionCuts.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iostream>

PhysicsMenu::PhysicsMenu():SimpleMenu("PhysicsMenu")
{
  //	std::cout <<" creating the PhysicsMenu "<<std::endl;
}

void PhysicsMenu::PrintLists()
{
  FADS::PhysicsListCatalog* plm=FADS::PhysicsListCatalog::GetInstance();
  plm->PrintAll();
}

void PhysicsMenu::UsePhysicsList(const std::string pList)
{
  FADS::PhysicsListCatalog* plm = FADS::PhysicsListCatalog::GetInstance();
  FADS::FadsPhysicsList* pl = plm->GetPhysicsListEntry(pList);
  G4RunManager* rm = G4RunManager::GetRunManager();
  /// @todo I don't think the types are right for this dynamic cast...
  FADS::FadsRunManager* arm = dynamic_cast<FADS::FadsRunManager*>(rm);
  //log() << MSG::VERBOSE << "FOO G4AtlasControl::PhysicsMenu: " << rm << ", " << arm << endreq;
  if (arm)
    arm->SetPhysicsList(pl);
  else
    plm->SetCurrentPhysicsList(pl);
}

void PhysicsMenu::ConstructPhysicsList()
{
  FADS::PhysicsListCatalog* plm = FADS::PhysicsListCatalog::GetInstance();
  FADS::FadsPhysicsList* pl = plm->GetCurrentPhysicsList();
  if (pl==0) {
    log() << MSG::WARNING << "The physics list is not set: nothing done" << endreq;
  } else {
    plm->SetCurrentPhysicsList(pl);
    pl->Construct();
  }
}

void PhysicsMenu::SetGeneralCut(double cut)
{
  FADS::PhysicsListCatalog* plm=FADS::PhysicsListCatalog::GetInstance();
  plm->SetDefaultCutValue(cut);
}

PhysicsRegionProxy PhysicsMenu::CreateNewRegion(const std::string regionName)
{
  G4RegionStore& regionStore(*(G4RegionStore::GetInstance()));
  G4Region *temp(NULL);
  for (unsigned int i=0;i<regionStore.size();i++)
    {
      if (static_cast<const std::string>(regionStore[i]->GetName())==regionName) { temp=regionStore[i]; }
    }
  if (temp) 
    {
      log() << MSG::WARNING << "Detected attempt to re-create the pre-existing "<<temp->GetName()<< " region! Nothing done." << endreq;
    }
  else
    {
      temp=new G4Region(regionName);
      // we assign a G4ProductionCuts by defaults
      G4ProductionCuts *pc=new G4ProductionCuts;
      pc->SetProductionCut(1*CLHEP::mm);
      log() << MSG::INFO << "New region: "<<regionName<<". A default cut"
	    <<" of 1mm is assigned for gamma,e+,e-" << endreq;
      temp->SetProductionCuts(pc);
      // we add field integration parameters for stepper dispatcher
      FADS::RegionInformation* regInfo = new FADS::RegionInformation();
      FADS::FieldIntParameters* fieldIntParam = new FADS::FieldIntParameters();
      regInfo->SetFieldIntParameters(fieldIntParam);
      temp->SetUserInformation(regInfo);
      log() << MSG::INFO << "New region: "<<regionName<<" has the default field integration parameters." << endreq;
    }
  PhysicsRegionProxy tempRegion(regionName, temp);
  return tempRegion;
}
void PhysicsMenu::ListRegions()
{
  G4RegionStore& rs=*(G4RegionStore::GetInstance());
  for (unsigned int i=0;i<rs.size();i++) {
    log() << MSG::INFO << "---- Region: "<<rs[i]->GetName() << endreq;
  }
}

void PhysicsMenu::SetNeutronTimeCut(double t)
{
  G4RunManager *rm=G4RunManager::GetRunManager();
  const FADS::PhysicsListSteering *pl=dynamic_cast<const FADS::PhysicsListSteering *>(rm->GetUserPhysicsList());
  if (pl) {
    FADS::PhysicsListSteering *ppl=const_cast<FADS::PhysicsListSteering *>(pl);
    ppl->SetNeutronTimeCut(t);
  }
}
void PhysicsMenu::SetNeutronEnergyCut(double e)
{
  G4RunManager *rm = G4RunManager::GetRunManager();
  const FADS::PhysicsListSteering *pl = dynamic_cast<const FADS::PhysicsListSteering*>(rm->GetUserPhysicsList());
  if (pl) {
    FADS::PhysicsListSteering *ppl = const_cast<FADS::PhysicsListSteering*>(pl);
    ppl->SetNeutronEnergyCut(e);
  }
}

void PhysicsMenu::SetApplyCuts(bool e)
{
  G4RunManager *rm = G4RunManager::GetRunManager();
  const FADS::PhysicsListSteering *pl = dynamic_cast<const FADS::PhysicsListSteering*>(rm->GetUserPhysicsList());
  if (pl) {
    FADS::PhysicsListSteering *ppl = const_cast<FADS::PhysicsListSteering*>(pl);
    ppl->SetApplyCuts(e);
  }
}

void PhysicsMenu::SetRandomNumberService(const std::string name)
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->RandomNumberService(name);
}
void PhysicsMenu::SetVertexOverrideFile(const std::string path)
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->VertexOverrideFile(path);
}
void PhysicsMenu::SetVertexOverrideEventFile(const std::string path)
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();
  gc->VertexOverrideEventFile(path);
}

