/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// IUnitInterpreter.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef UNIT_INTERPRETER_H
#define UNIT_INTERPRETER_H

#include <string>
#include <map>

namespace ExpressionParsing {
  class IUnitInterpreter {
    public:
      virtual ~IUnitInterpreter() { }

      bool isKnownUnit(const std::string &unitname) const;
      double unitValue(const std::string &unitname) const;

    private:
      virtual const std::map<std::string, double> &unitMap() const = 0;
  };
}

#endif // UNIT_INTERPRETER_H
