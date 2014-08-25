/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TestProxyLoader.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ExpressionEvaluation/TestProxyLoader.h"

#include <stdexcept>

namespace ExpressionParsing {
  TestProxyLoader::~TestProxyLoader()
  {
  }

  void TestProxyLoader::reset()
  {
    intAccessCount = 0;
  }

  IProxyLoader::VariableType TestProxyLoader::variableTypeFromString(const std::string &varname)
  {
    if (varname == "intTEST") return VT_INT;
    else if (varname == "int_TEST") return VT_INT;
    else if (varname == "doubleTEST") return VT_DOUBLE;
    else if (varname == "vectorIntTEST") return VT_VECINT;
    else if (varname == "vectorDoubleTEST") return VT_VECDOUBLE;
    else return VT_UNK;
  }

  int TestProxyLoader::loadIntVariableFromString(const std::string &varname)
  {
    if (varname == "intTEST") { ++intAccessCount; return (42+(intAccessCount-1)); }
    else if (varname == "int_TEST") { return 24; }
    else throw std::runtime_error("Unknown proxy: " + varname);
  }

  double TestProxyLoader::loadDoubleVariableFromString(const std::string &varname)
  {
    if (varname == "doubleTEST") return 42.42;
    else throw std::runtime_error("Unknown proxy: " + varname);
  }

  std::vector<int> TestProxyLoader::loadVecIntVariableFromString(const std::string &varname)
  {
    if (varname == "vectorIntTEST") return std::vector<int>(2, 42);
    else throw std::runtime_error("Unknown proxy: " + varname);
  }

  std::vector<double> TestProxyLoader::loadVecDoubleVariableFromString(const std::string &varname)
  {
    if (varname == "vectorDoubleTEST") return std::vector<double>(2, 42.42);
    else throw std::runtime_error("Unknown proxy: " + varname);
  }
}
