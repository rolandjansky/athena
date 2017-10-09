/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EqOfMotionFactoryBase_H
#define EqOfMotionFactoryBase_H

#include <string>

class G4EquationOfMotion;
class G4Mag_EqRhs;

namespace FADS {

// abstract base class for implementing a templated stepper factory
class EqOfMotionFactoryBase {
public:
	EqOfMotionFactoryBase(std::string n):m_EqOfMotionName(n) {}
	virtual G4Mag_EqRhs* Build()=0;
	std::string GetName() const {return m_EqOfMotionName;}
	void SetName(std::string n) {m_EqOfMotionName=n;}
protected:
	std::string m_EqOfMotionName;
};

}	// end namespace

#endif
