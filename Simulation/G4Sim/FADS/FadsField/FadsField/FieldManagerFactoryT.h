/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FieldManagerFactoryT_H
#define FieldManagerFactoryT_H

#include "FadsField/FieldManagerFactory.h"

namespace FADS {

template <class T> class FieldManagerFactoryT:public FieldManagerFactory {
public:
	FieldManagerFactoryT(std::string n):FieldManagerFactory(n),m_theFactory(0) {}
	FadsFieldManager *CreateFieldManager(std::string n)
	{	
		if (!m_theFactory) m_theFactory=new T(n);
		return m_theFactory;
	}
private:
	T* m_theFactory;
};

}
#endif
