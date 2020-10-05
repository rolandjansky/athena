/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// IProxyLoader.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef IPROXY_LOADER_H
#define IPROXY_LOADER_H

#include <string>
#include <vector>

namespace ExpressionParsing {
  class IProxyLoader {
    public:
      enum VariableType { VT_UNK, VT_INT, VT_DOUBLE, VT_VECINT, VT_VECDOUBLE, VT_VECEMPTY };

      virtual ~IProxyLoader() { }

      virtual void reset() = 0;

      virtual VariableType variableTypeFromString(const std::string &varname) = 0;

      virtual int loadIntVariableFromString(const std::string &varname) = 0;
      virtual double loadDoubleVariableFromString(const std::string &varname) = 0;
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname) = 0;
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname) = 0;
  };
}

#endif // IPROXY_LOADER_H
