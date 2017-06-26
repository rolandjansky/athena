/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_EXTENDEDEVNETCONTEXT_H
#define ATHENAKERNEL_EXTENDEDEVNETCONTEXT_H 1

#include "AthenaKernel/IProxyDict.h"

namespace Atlas {

  class ExtendedEventContext {
  public:
    ExtendedEventContext(){};

    ExtendedEventContext(IProxyDict* p): m_proxy(p){};

    void setProxy(IProxyDict* proxy) { m_proxy = proxy; }
    IProxyDict* proxy() const { return m_proxy; }
    
  private:
    IProxyDict* m_proxy {nullptr};

  };
}

#endif

