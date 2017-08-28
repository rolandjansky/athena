/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_EXTENDEDEVNETCONTEXT_H
#define ATHENAKERNEL_EXTENDEDEVNETCONTEXT_H 1

#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/EventIDBase.h"

namespace Atlas {

  class ExtendedEventContext {
  public:
    ExtendedEventContext(){};

    ExtendedEventContext(IProxyDict* p,
                         EventIDBase::number_type conditionsRun = EventIDBase::UNDEFNUM)
      : m_proxy (p), m_conditionsRun (conditionsRun)
    {}

    void setProxy(IProxyDict* proxy) { m_proxy = proxy; }
    IProxyDict* proxy() const { return m_proxy; }

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

