/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// SGNTUPProxyLoader.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef SG_NTUP_PROXY_LOADER_H
#define SG_NTUP_PROXY_LOADER_H

#include "ExpressionEvaluation/IProxyLoader.h"

#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/StlVectorClids.h"
#include "SGTools/BuiltinsClids.h"

namespace ExpressionParsing {
  class SGNTUPProxyLoader : public IProxyLoader {
    public:
      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

      SGNTUPProxyLoader(StoreGateSvc_t &evtStore) : m_evtStore(evtStore) { }
      virtual ~SGNTUPProxyLoader();

      virtual void reset();

      virtual IProxyLoader::VariableType variableTypeFromString(const std::string &varname);

      virtual int loadIntVariableFromString(const std::string &varname);
      virtual double loadDoubleVariableFromString(const std::string &varname);
      virtual std::vector<int> loadVecIntVariableFromString(const std::string &varname);
      virtual std::vector<double> loadVecDoubleVariableFromString(const std::string &varname);

    private:
      StoreGateSvc_t m_evtStore;
  };
}

#endif // SG_NTUP_PROXY_LOADER_H
