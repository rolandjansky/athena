/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// TestProxyLoader.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef TEST_PROXY_LOADER_H
#define TEST_PROXY_LOADER_H

#include "ExpressionEvaluation/IProxyLoader.h"

namespace ExpressionParsing {
  class TestProxyLoader : public IProxyLoader {
    public:
      TestProxyLoader() : intAccessCount(0) { }
      virtual ~TestProxyLoader();

      virtual void reset();

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);

      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

    private:
      unsigned int intAccessCount;
  };
}

#endif // TEST_PROXY_LOADER_H
