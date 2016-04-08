/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EqOfMotionFactory_H
#define EqOfMotionFactory_H

#include "FadsField/EqOfMotionFactoryBase.h"
#include "FadsField/FieldManager.h"
#include "G4Mag_EqRhs.hh"
#include "G4MagneticField.hh"

namespace FADS {

template <class T> 
class EqOfMotionFactory : public EqOfMotionFactoryBase {
private:
	T* theEqOfMotion;
public:
	 EqOfMotionFactory(std::string n): EqOfMotionFactoryBase(n) , theEqOfMotion(0)
	{
		RegisterToStore();
	}
	void Create() 
	{
		if (!theEqOfMotion) 
		{
			theEqOfMotion=
			new T(FieldManager::GetFieldManager()->GetMagneticFieldMap());
		}
	}
	void Delete()
	{
		if (theEqOfMotion) 
		{
			delete theEqOfMotion;
			theEqOfMotion=0;
		}
	}
	virtual G4Mag_EqRhs* Build() 
	{
		Create(); return theEqOfMotion;
	}
	void RegisterToStore() {FieldManager::GetFieldManager()->RegisterEqOfMotion(this);}
};

}	// end namespace

#endif
