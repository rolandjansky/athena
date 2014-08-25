/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ProxyLoaderSingleton.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////
// Author: Thomas Gillam (thomas.gillam@cern.ch)
// ExpressionParsing library
/////////////////////////////////////////////////////////////////

#ifndef PROXY_LOADER_SINGLETON_H
#define PROXY_LOADER_SINGLETON_H

#include "ExpressionEvaluation/IProxyLoader.h"
#include "ExpressionEvaluation/TestProxyLoader.h"
#include "EncapsulatingSingleton.h"
#include <stdexcept>

namespace ExpressionParsing {
  template <> struct EncapsulatedDefault<IProxyLoader> {
    typedef TestProxyLoader type;
  };

  class ProxyLoaderSingleton : public EncapsulatingSingleton<IProxyLoader> {
  };
}

#endif // PROXY_LOADER_SINGLETON_H
