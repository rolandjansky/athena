/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_EXTENDEDEVENTCONTEXT_H
#define ATHENAKERNEL_EXTENDEDEVENTCONTEXT_H

#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/EventIDBase.h"
#include "CxxUtils/checker_macros.h"

namespace Atlas {

  class ExtendedEventContext {
  public:
    ExtendedEventContext(){};

    ExtendedEventContext(IProxyDict* p,
                         EventIDBase::number_type conditionsRun = EventIDBase::UNDEFNUM)
      : m_proxy (p), m_conditionsRun (conditionsRun)
    {}

    void setProxy(IProxyDict* proxy) { m_proxy = proxy; }
    IProxyDict* proxy ATLAS_NOT_CONST_THREAD_SAFE () const { return m_proxy; }

    void setConditionsRun (EventIDBase::number_type conditionsRun)
    { m_conditionsRun = conditionsRun; }
    EventIDBase::number_type conditionsRun() const
    { return m_conditionsRun;  }

  private:
    IProxyDict* m_proxy {nullptr};
    EventIDBase::number_type m_conditionsRun {EventIDBase::UNDEFNUM};
  };
}

#endif

