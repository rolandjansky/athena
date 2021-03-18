/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MultipleProxyLoader.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////


#include "ExpressionEvaluation/MultipleProxyLoader.h"

#include <stdexcept>
#include <iostream>

namespace ExpressionParsing {
  MultipleProxyLoader::~MultipleProxyLoader()
  {
  }


  void MultipleProxyLoader::push_back(IProxyLoader *proxyLoader)
  {
    m_proxyLoaders.push_back(proxyLoader);
  }

  void MultipleProxyLoader::reset()
  {
    for (const auto &proxyLoader : m_proxyLoaders) {
      proxyLoader->reset();
    }
  }

  IProxyLoader::VariableType MultipleProxyLoader::variableTypeFromString(const std::string &varname)
  {
    if (m_varnameToProxyLoader.find(varname) != m_varnameToProxyLoader.end()) {
      return m_varnameToProxyLoader[varname]->variableTypeFromString(varname);
    }

    IProxyLoader::VariableType result;
    bool foundProxyLoader(false);
    for (const auto &proxyLoader : m_proxyLoaders) {
      try {
        result = proxyLoader->variableTypeFromString(varname);
        if (result == VT_UNK) continue;
      } catch (const std::runtime_error &) {
        continue;
      }
      foundProxyLoader = true;
      m_varnameToProxyLoader[varname] = proxyLoader;
      break;
    }
    if (!foundProxyLoader) {
      throw std::runtime_error("MultipleProxyLoader: unable to find valid proxy loader for "+varname);
    }
    return result;
  }

  int MultipleProxyLoader::loadIntVariableFromString(const std::string &varname)
  {
    return m_varnameToProxyLoader[varname]->loadIntVariableFromString(varname);
  }

  double MultipleProxyLoader::loadDoubleVariableFromString(const std::string &varname)
  {
    return m_varnameToProxyLoader[varname]->loadDoubleVariableFromString(varname);
  }

  std::vector<int> MultipleProxyLoader::loadVecIntVariableFromString(const std::string &varname)
  {
    return m_varnameToProxyLoader[varname]->loadVecIntVariableFromString(varname);
  }

  std::vector<double> MultipleProxyLoader::loadVecDoubleVariableFromString(const std::string &varname)
  {
    return m_varnameToProxyLoader[varname]->loadVecDoubleVariableFromString(varname);
  }
}
