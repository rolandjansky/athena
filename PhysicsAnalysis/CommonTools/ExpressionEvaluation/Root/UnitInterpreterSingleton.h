/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// UnitInterpreterSingleton.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef UNIT_INTERPRETER_SINGLETON_H
#define UNIT_INTERPRETER_SINGLETON_H

#include "ExpressionEvaluation/IUnitInterpreter.h"
#include "NTUPUnitInterpreter.h"
#include "EncapsulatingSingleton.h"
#include <stdexcept>

namespace ExpressionParsing {
  template <> struct EncapsulatedDefault<IUnitInterpreter> {
    typedef NTUPUnitInterpreter type;
  };

  class UnitInterpreterSingleton : public EncapsulatingSingleton<IUnitInterpreter> {
  };

}

#endif // UNIT_INTERPRETER_SINGLETON_H
