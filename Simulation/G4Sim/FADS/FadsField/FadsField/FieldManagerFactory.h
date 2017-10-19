/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldManagerFactory_H
#define FieldManagerFactory_H

#include "FadsField/FadsFieldManager.h"
#include <string>

namespace FADS {

class FieldManagerFactory {
public:
	FieldManagerFactory(std::string n):m_name(n) { RegisterToStore(); }
	std::string GetName() {return m_name;}
	virtual FadsFieldManager *CreateFieldManager(std::string) = 0;
	virtual ~FieldManagerFactory() {}
private:
	std::string m_name;
	void RegisterToStore();
};

}
#endif
