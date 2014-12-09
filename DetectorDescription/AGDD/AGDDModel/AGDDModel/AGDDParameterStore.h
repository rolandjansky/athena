/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDParameterStore_H
#define AGDDParameterStore_H

#include <map>
#include <iostream>
#include "AGDDModel/AGDDParameterBag.h"

typedef std::map<std::string,AGDDParameterBag* > AGDDParameterMap;

class AGDDParameterStore : public AGDDParameterMap{
public:
	AGDDParameterStore() {}

	virtual ~AGDDParameterStore() {}

        void RegisterParameterBag(std::string volName, AGDDParameterBag* paraBag) {
		(*this)[volName]=paraBag;
	}

        AGDDParameterBag* GetParameterBag(std::string volName) {
		if ((*this).find(volName) != (*this).end())
			return (*this)[volName];
		else {
			std::cout << " Parameters for Volume "<<volName<<" not found: returning 0"<<std::endl;
			return 0;
		}
	}

        bool Exist(std::string volName) {
		return ((*this).find(volName) != (*this).end());
	}

	static AGDDParameterStore* GetParameterStore() {
		static AGDDParameterStore* thePointer=new AGDDParameterStore();
		return thePointer;
	}
};

#endif
