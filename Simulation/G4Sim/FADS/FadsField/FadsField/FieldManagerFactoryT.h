/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldManagerFactoryT_H
#define FieldManagerFactoryT_H

#include "FadsField/FieldManagerFactory.h"

namespace FADS {

template <class T> class FieldManagerFactoryT:public FieldManagerFactory {
public:
	FieldManagerFactoryT(std::string n):FieldManagerFactory(n),theFactory(0) {}
	FadsFieldManager *CreateFieldManager(std::string n)
	{	
		if (!theFactory) theFactory=new T(n);
		return theFactory;
	}
private:
	T* theFactory;
};

}
#endif
