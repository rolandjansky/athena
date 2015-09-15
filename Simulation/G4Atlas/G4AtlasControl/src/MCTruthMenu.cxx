/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>

#include "G4AtlasControl/MCTruthMenu.h"

#include "MCTruthBase/TruthStrategyManager.h"
#include "MCTruthBase/TruthStrategy.h"

#include "CLHEP/Units/SystemOfUnits.h"

MCTruthMenu::MCTruthMenu():SimpleMenu("MCTruthMenu")
{
  //        std::cout << "Creating the MCTruthMenuMenu "<<std::endl;
}

void MCTruthMenu::listStrategies()
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  m->ListStrategies();
}
void MCTruthMenu::activateStrategy(const std::string strg_name, const std::string vol_name, int vol_level)
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  TruthStrategy* strategy=m->GetStrategy(strg_name);
  if(strategy){
    strategy->Activate(vol_name,vol_level);
    if(strategy->IsActivated()){
      log() << MSG::INFO << "MCTruth::TruthStrategyManager SetActive: "<<
		     strg_name<<" active in the volume "<<vol_name<<
		     " at  level ----> "<<vol_level << endreq;
    } else {
      log() << MSG::WARNING << " Could not activate " << strg_name << " in volume " << vol_name << endreq;
    }
  } else {
    log() << MSG::WARNING << " Could not find strategy " << strg_name << " for activation in volume " << vol_name << endreq;
  }
}
void MCTruthMenu::disactivateStrategy(const std::string strg_name)
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  TruthStrategy* strategy=m->GetStrategy(strg_name);
  if (strategy) strategy->Disactivate();
}
void MCTruthMenu::listParameters()
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  m->PrintParameterList();
}

void MCTruthMenu::secondarySaving(const std::string SecondarySavingPolicy)
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  if (SecondarySavingPolicy=="Primaries") m->SetSecondarySavingLevel(1);
  if (SecondarySavingPolicy=="StoredSecondaries") m->SetSecondarySavingLevel(2);
  if (SecondarySavingPolicy=="All") m->SetSecondarySavingLevel(3);
}

void MCTruthMenu::setTruthStrategiesParameter(const std::string paramName,double energy)
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  m->SetTruthParameter(paramName,energy);
  log() << MSG::INFO << "setTruthStrategiesParameter "<<paramName<<" = "<<energy*CLHEP::MeV<<" MeV" << endreq;
}

void MCTruthMenu::enableEnvelopeRecording()
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  m->EnableRecordingEnvelopes();
}

void MCTruthMenu::addRecordingEnvelope(const std::string name, int lev, 
				       std::string sdName)
{
  TruthStrategyManager *m=TruthStrategyManager::GetStrategyManager();
  m->AddRecordingEnvelope(name,lev,sdName);
}
