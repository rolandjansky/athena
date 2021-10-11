/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDKernel/AliasStore.h"
#include <iostream>

void AliasStore::AddAlias(const std::string& tag,const std::string& alias)
{
	if (this->find(tag)!=this->end())
	{
		std::cout<<"trying to set existing alias: old value kept"<<std::endl;
	}
	else
		(*this)[tag]=alias;
}

std::string AliasStore::Alias(const std::string& tag)
{
	if (this->find(tag)!=this->end())
		return (*this)[tag];
	else
		return tag;
}

bool AliasStore::IsAliased(const std::string& tag)
{
	if (this->find(tag)!=this->end())
		return true;
	return false;
}
