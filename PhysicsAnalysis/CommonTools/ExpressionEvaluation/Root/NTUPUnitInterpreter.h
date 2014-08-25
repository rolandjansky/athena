/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// NTUPUnitInterpreter.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef NTUP_UNIT_INTERPRETER_H
#define NTUP_UNIT_INTERPRETER_H

#include "ExpressionEvaluation/IUnitInterpreter.h"

namespace ExpressionParsing {
  class NTUPUnitInterpreter : public IUnitInterpreter {
    public:
      NTUPUnitInterpreter();
      virtual ~NTUPUnitInterpreter();

    private:
      virtual const std::map<std::string, double> &unitMap() const;

    private:
      std::map<std::string, double> m_unitStringToValue;
  };
}

#endif // NTUP_UNIT_INTERPRETER_H
