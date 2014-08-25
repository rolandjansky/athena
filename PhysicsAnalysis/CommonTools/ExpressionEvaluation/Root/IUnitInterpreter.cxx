/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// IUnitInterpreter.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ExpressionEvaluation/IUnitInterpreter.h"

#include <stdexcept>

namespace ExpressionParsing {
  bool IUnitInterpreter::isKnownUnit(const std::string &unitname) const
  {
    return (unitMap().count(unitname) > 0);
  }

  double IUnitInterpreter::unitValue(const std::string &unitname) const
  {
    typedef std::map<std::string, double>::const_iterator Iterator;
    const std::map<std::string, double> &theMap = unitMap();
    for (Iterator it = theMap.begin(); it != theMap.end(); ++it) {
      if (it->first == unitname) {
        return it->second;
      }
    }

    throw std::runtime_error("Trying to access unknown unit: "+unitname);
  }
}
