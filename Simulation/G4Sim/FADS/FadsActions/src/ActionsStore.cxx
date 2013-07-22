/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsActions/ActionsStore.h"
#include "FadsActions/ActionCenterMessenger.h"
#include "FadsActions/UserAction.h"

namespace FADS {

ActionsStore* ActionsStore::thePointer=ActionsStore::GetActionsStore();

ActionsStore::ActionsStore()
{
	theMessenger=new ActionStoreMessenger(this);
}

ActionsStore::ActionsStore(const ActionsStore&) 
{
        theMessenger=new ActionStoreMessenger(this);
}

ActionsStore* ActionsStore::GetActionsStore()
{
	if (!thePointer) thePointer=new ActionsStore;
	return thePointer;
}

void ActionsStore::Register(UserAction *a)
{
	if (theMap.find(a->GetName())!=theMap.end())
	{
		std::cout<<" Attempt of re-defining an existing Action "<<
			 a->GetName()<<"!!!"<<std::endl;
	}
	else
		theMap[a->GetName()]=a;
}

UserAction* ActionsStore::Retrieve(std::string nam)
{
	if (theMap.find(nam)==theMap.end())
	{
		std::cout<<" Action "<<nam<<" not found!!!"<<std::endl;
		return 0;
	}
	else
		return theMap[nam];
}

void ActionsStore::Unregister(const UserAction *a)
{
	if (theMap.find(a->GetName())==theMap.end())
	{
		std::cout<<" Attempt of clearing a not-existing Action "<<
			 a->GetName()<<"!!!"<<std::endl;
	}
	else
		theMap.erase(theMap.find(a->GetName()));
}

void ActionsStore::ListActions()
{
	userActionMap::const_iterator it;
	std::cout<<" List of all define actions: "<<std::endl;
	for (it=theMap.begin();it!=theMap.end();it++)
	{
		std::cout<<"\t--> "<<(*it).second->GetName()<<std::endl;
	}
}


void ActionsStore::SetActionProperty(std::string actionName, std::string propName, std::string propValue){

  Retrieve(actionName)->SetProperty(propName,propValue); 

}


}	// end namespace
