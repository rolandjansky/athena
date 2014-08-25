/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// NTUPUnitInterpreter.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "NTUPUnitInterpreter.h"

namespace ExpressionParsing {
  NTUPUnitInterpreter::NTUPUnitInterpreter()
  {
    m_unitStringToValue["keV"] = 0.001;
    m_unitStringToValue["MeV"] = 1.;
    m_unitStringToValue["GeV"] = 1000.;
    m_unitStringToValue["TeV"] = 1000000.;

    m_unitStringToValue["mm"] = 1.;
    m_unitStringToValue["cm"] = 10.;
  }

  NTUPUnitInterpreter::~NTUPUnitInterpreter()
  {
  }

  const std::map<std::string, double> &NTUPUnitInterpreter::unitMap() const
  {
    return m_unitStringToValue;
  }
}
