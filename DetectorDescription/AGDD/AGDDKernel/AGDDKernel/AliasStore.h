/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AliasStore_H
#define AliasStore_H

#include <map>
#include <string>

class AliasStore:public std::map<std::string,std::string,std::less<std::string> > {
public:
	static AliasStore* GetAliasList();
	void AddAlias(std::string,std::string);
	bool IsAliased(std::string);
	std::string Alias(std::string);
private:
	AliasStore() {}
};

#endif

#define ALIAS(CNAME) AliasStore::GetAliasList()->Alias(CNAME)
