/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasControl/ActionMenu.h"
#include "FadsActions/ActionsStore.h"
#include "FadsActions/UserAction.h"

ActionMenu::ActionMenu():SimpleMenu("ActionMenu")
{;}

void ActionMenu::actionList() const
{
	FADS::ActionsStore *as=FADS::ActionsStore::GetActionsStore();
	as->ListActions();
}

void ActionMenu::actionDelete(const std::string aName) const 
{
	FADS::ActionsStore *as=FADS::ActionsStore::GetActionsStore();
	delete as->Retrieve(aName);
}

void ActionMenu::actionRegister(const std::string aName, const std::string status) const
{
	FADS::ActionsStore *as=FADS::ActionsStore::GetActionsStore();
	as->Retrieve(aName)->RegisterFor(status);
}

void ActionMenu::actionProperty(const std::string aName, const std::string propName,const std::string propValue) const
{
    FADS::ActionsStore *as=FADS::ActionsStore::GetActionsStore();
	as->SetActionProperty(aName,propName,propValue);
}
