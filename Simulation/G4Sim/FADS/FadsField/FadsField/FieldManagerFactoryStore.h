/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldManagerFactoryStore_H
#define FieldManagerFactoryStore_H

#include "FadsField/FieldManagerFactory.h"
#include <map>
namespace FADS {
// class FieldManagerFactory;

typedef std::map<std::string,FieldManagerFactory*,std::less<std::string> > factoryMap;

class FieldManagerFactoryStore {
public:
	static FieldManagerFactoryStore* GetStore();
	void AddFieldManagerFactory(FieldManagerFactory *);
	FieldManagerFactory *GetFieldManagerFactory(std::string n);
private:
	FieldManagerFactoryStore();
	factoryMap m_theFactories;
};

}
#endif
