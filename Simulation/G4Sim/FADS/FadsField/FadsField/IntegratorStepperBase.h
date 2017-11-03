/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IntegratorStepperBase_H
#define IntegratorStepperBase_H

#include <string>
#include "G4MagIntegratorStepper.hh"

namespace FADS {


  /// Abstract base class for implementing a templated stepper factory
  class IntegratorStepperBase {
  public:
	IntegratorStepperBase(const std::string& n)
      : m_stepperName(n)
    {  }

	virtual G4MagIntegratorStepper* Build() = 0;

	std::string GetName() const {
      return m_stepperName;
    }

	void SetName(const std::string& n) {
      m_stepperName = n;
    }

  protected:
	std::string m_stepperName;
  };


}

#endif
