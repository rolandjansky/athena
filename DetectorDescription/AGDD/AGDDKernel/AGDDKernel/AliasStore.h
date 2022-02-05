/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AliasStore_H
#define AliasStore_H

#include <map>
#include <string>

class AliasStore:public std::map<std::string,std::string,std::less<std::string> > {
public:
	AliasStore() {}
	void AddAlias(const std::string&,const std::string&);
	bool IsAliased(const std::string&);
	std::string Alias(const std::string&);
};

#endif

