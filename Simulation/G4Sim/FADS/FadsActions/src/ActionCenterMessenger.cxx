/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/ActionCenterMessenger.h"
#include "FadsActions/ActionsStore.h"
#include "FadsActions/UserAction.h"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"

namespace FADS {

ActionStoreMessenger::ActionStoreMessenger(ActionsStore *v)
{
	rm=v;
	
	directory=new G4UIdirectory("/Actions/");
	directory->SetGuidance("Control commands for user actions");

	select=new G4UIcommand("/Actions/Select",this);
	select->SetGuidance("Selection of an action");
	G4UIparameter* parameter;
	G4bool omitable;
	parameter = new G4UIparameter ("Action name", 's', omitable = false);
	select->SetParameter(parameter);
	
	list=new G4UIcommand("/Actions/List",this);
	list->SetGuidance("Lists all registered user actions");
	
	del=new G4UIcommand("/Actions/Delete",this);
	del->SetGuidance("Deletes an User action in the list");
	parameter = new G4UIparameter ("Action name", 's', omitable = false);
	del->SetParameter(parameter);
	
	reg=new G4UIcommand("/Actions/RegisterForAction",this);
	reg->SetGuidance("Registers an User action for an action");
	parameter = new G4UIparameter ("Action name", 's', omitable = false);
	reg->SetParameter(parameter);
	G4UIparameter* parameter2= new G4UIparameter ("Action", 's', omitable = false);
	parameter2->SetParameterCandidates("Step BeginOfEvent EndOfEvent BeginOfRun EndOfRun");
	reg->SetParameter(parameter2);
	
}

ActionStoreMessenger::~ActionStoreMessenger()
{
	delete select;
	delete list;
	delete directory;
	delete del;
	delete reg;
}

void ActionStoreMessenger::SetNewValue(G4UIcommand * command,G4String newValues)
{
	if (command==list)
	{
		rm->ListActions();
	}
	else if (command==select)
	{
//		rm->SelectGenerator(newValues);
	}
	else if (command==del)
	{
		delete rm->Retrieve(newValues);
	}
	else if (command==reg)
	{
		G4String aName,stateName;
                const char* s = newValues;
                std::istringstream is ((char*)s);
                is>>aName>>stateName;
		rm->Retrieve(aName)->RegisterFor(stateName);
	}
}

G4String ActionStoreMessenger::GetCurrentValue(G4UIcommand * /*command*/)
{
	G4String s="Undefined";
	return s;
}

}	// end namespace
