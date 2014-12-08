/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AliasStore.h"
#include <iostream>

AliasStore* AliasStore::GetAliasList()
{
	static AliasStore* theStore=new AliasStore;
	return theStore;
}

void AliasStore::AddAlias(std::string tag,std::string alias)
{
	if (this->find(tag)!=this->end())
	{
		std::cout<<"trying to set existing alias: old value kept"<<std::endl;
	}
	else
		(*this)[tag]=alias;
}

std::string AliasStore::Alias(std::string tag)
{
	if (this->find(tag)!=this->end())
		return (*this)[tag];
	else
		return tag;
}

bool AliasStore::IsAliased(std::string tag)
{
	if (this->find(tag)!=this->end())
		return true;
	return false;
}
