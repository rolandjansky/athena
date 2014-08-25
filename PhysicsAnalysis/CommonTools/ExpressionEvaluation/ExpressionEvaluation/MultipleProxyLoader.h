/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MultipleProxyLoader.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef MULTIPLE_PROXY_LOADER_H
#define MULTIPLE_PROXY_LOADER_H

#include "ExpressionEvaluation/IProxyLoader.h"

#include <vector>
#include <map>

namespace ExpressionParsing {
  class MultipleProxyLoader : public IProxyLoader {
    public:
      MultipleProxyLoader() { }
      virtual ~MultipleProxyLoader();

      void push_back(IProxyLoader *proxyLoader);

      virtual void reset();

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);

      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

    private:
      std::vector<IProxyLoader *> m_proxyLoaders;
      std::map<std::string, IProxyLoader *> m_varnameToProxyLoader;
  };
}

#endif // MULTIPLE_PROXY_LOADER_H
