/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsAlg/FadsRunManager.h"
#include "FadsControl/RunManagerDef.h"


RUNMANAGERDEFINITION(FadsRunManager,DefaultRunManager)

void FadsRunManager::InitializeRun()
{
	RunInitialization();
}

void FadsRunManager::TerminateRun()
{
	RunTermination();
}

void FadsRunManager::SimulateOneEvent()
{
   static int i_event=1;
   currentEvent = GenerateEvent(i_event);
   eventManager->ProcessOneEvent(currentEvent);
   AnalyzeEvent(currentEvent);
//   if(i_event<n_select) G4UImanager::GetUIpointer()->ApplyCommand(msg);
   StackPreviousEvent(currentEvent);
   currentEvent = 0;
   i_event++;
}
