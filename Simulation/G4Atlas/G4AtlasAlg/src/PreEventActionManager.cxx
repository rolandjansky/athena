/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4AtlasAlg/PreEventActionManager.h"
#include "G4AtlasAlg/PreEventAction.h"
#include <iostream>

PreEventActionManager* PreEventActionManager::thePointer=0;

PreEventActionManager::PreEventActionManager()
{
}

PreEventActionManager* PreEventActionManager::GetPreEventActionManager()
{
	if (!thePointer) thePointer=new PreEventActionManager();
	return thePointer;
}

void PreEventActionManager::Execute()
{
	preEvActionMap::const_iterator it;
	for (it=theMap.begin();it!=theMap.end();it++)
		(*it).second->Execute();
}

void PreEventActionManager::RegisterAction(PreEventAction *act)
{
	std::string n=act->GetName();
	if (theMap.find(n)!=theMap.end())
		std::cout<<"Attempt of re-defining an existing pre-event action"
			<<std::endl;
	else
		theMap[n]=act;
}
