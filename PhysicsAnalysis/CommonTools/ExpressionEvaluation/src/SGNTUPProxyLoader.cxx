/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SGNTUPProxyLoader.cxx, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#include "ExpressionEvaluation/SGNTUPProxyLoader.h"

#define SG_GET_SCALAR(RETTYPE, TRYTYPE) do { if (m_evtStore->contains<TRYTYPE>(varname)) { \
  const TRYTYPE *val = nullptr; \
  StatusCode sc = m_evtStore->retrieve<TRYTYPE>((const TRYTYPE*&)val, varname); \
  if (sc.isFailure()) { \
    throw std::runtime_error("Couldn't retrieve (" #TRYTYPE ") " + varname); \
  } \
  return (RETTYPE)(*val); \
} } while (0)

#define SG_GET_VECTOR(RETTYPE, TRYTYPE) do { if (m_evtStore->contains<std::vector<TRYTYPE> >(varname)) { \
  const std::vector<TRYTYPE> *val = nullptr; \
  StatusCode sc = m_evtStore->retrieve<std::vector<TRYTYPE> >((const std::vector<TRYTYPE>*&)val, varname); \
  if (sc.isFailure()) { \
    throw std::runtime_error("Couldn't retrieve (std::vector<" #TRYTYPE ">) " + varname); \
  } \
  std::vector<RETTYPE> temp; \
  for (std::vector<TRYTYPE>::const_iterator it = val->begin(); it != val->end(); ++it) { \
    temp.push_back((RETTYPE)(*it)); \
  } \
  return temp; \
} } while (0)

namespace ExpressionParsing {
  SGNTUPProxyLoader::~SGNTUPProxyLoader()
  {
  }

  void SGNTUPProxyLoader::reset()
  {
  }

  IProxyLoader::VariableType SGNTUPProxyLoader::variableTypeFromString(const std::string &varname)
  {
    if      (m_evtStore->contains<int>(varname))                        return VT_INT;
    else if (m_evtStore->contains<bool>(varname))                       return VT_INT;
    else if (m_evtStore->contains<unsigned int>(varname))               return VT_INT;
    else if (m_evtStore->contains<float>(varname))                      return VT_DOUBLE;
    else if (m_evtStore->contains<double>(varname))                     return VT_DOUBLE;
    else if (m_evtStore->contains<std::vector<int> >(varname))          return VT_VECINT;
    else if (m_evtStore->contains<std::vector<bool> >(varname))         return VT_VECINT;
    else if (m_evtStore->contains<std::vector<unsigned int> >(varname)) return VT_VECINT;
    else if (m_evtStore->contains<std::vector<float> >(varname))        return VT_VECDOUBLE;
    else if (m_evtStore->contains<std::vector<double> >(varname))       return VT_VECDOUBLE;
    else return VT_UNK;
  }

  int SGNTUPProxyLoader::loadIntVariableFromString(const std::string &varname)
  {
    SG_GET_SCALAR(int, int);
    SG_GET_SCALAR(int, bool);
    SG_GET_SCALAR(int, unsigned int);
    throw std::runtime_error("Unknown int-like proxy: " + varname);
  }

  double SGNTUPProxyLoader::loadDoubleVariableFromString(const std::string &varname)
  {
    SG_GET_SCALAR(double, float);
    SG_GET_SCALAR(double, double);
    throw std::runtime_error("Unknown double-like proxy: " + varname);
  }

  std::vector<int> SGNTUPProxyLoader::loadVecIntVariableFromString(const std::string &varname)
  {
    SG_GET_VECTOR(int, int);
    SG_GET_VECTOR(int, bool);
    SG_GET_VECTOR(int, unsigned int);
    throw std::runtime_error("Unknown vector<int>-like proxy: " + varname);
  }

  std::vector<double> SGNTUPProxyLoader::loadVecDoubleVariableFromString(const std::string &varname)
  {
    SG_GET_VECTOR(double, float);
    SG_GET_VECTOR(double, double);
    throw std::runtime_error("Unknown vector<double>-like proxy: " + varname);
  }
}
