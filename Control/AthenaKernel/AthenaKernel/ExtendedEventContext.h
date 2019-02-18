/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_EXTENDEDEVENTCONTEXT_H
#define ATHENAKERNEL_EXTENDEDEVENTCONTEXT_H

#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/EventIDBase.h"
#include "CxxUtils/checker_macros.h"

class TrigRoiDescriptor; //!< Forward declaration

namespace Atlas {

  class ExtendedEventContext {
  public:
    ExtendedEventContext(){};

    ExtendedEventContext(IProxyDict* p,
                         EventIDBase::number_type conditionsRun = EventIDBase::UNDEFNUM,
                         const TrigRoiDescriptor* roi = nullptr)
      : m_proxy (p), m_conditionsRun (conditionsRun), m_roi (roi)
    {}

    void setProxy(IProxyDict* proxy) { m_proxy = proxy; }
    IProxyDict* proxy ATLAS_NOT_CONST_THREAD_SAFE () const { return m_proxy; }

    void setConditionsRun (EventIDBase::number_type conditionsRun)
    { m_conditionsRun = conditionsRun; }
    EventIDBase::number_type conditionsRun() const
    { return m_conditionsRun;  }

    /**
     * @brief Get cached pointer to View's Region of Interest Descriptor or nullptr if not describing a View
     *
     * If the context is describing a View within the event and that View encompasses a Region of Interest, 
     * then this method returns a cached pointer to the RoI as owned by the global context event store.
     * This is used to perform detailed trigger monitoring via auditor.
     *
     * @see EventViewCreatorAlgorithm::execute(const EventContext&)
     * @see TrigCostMTSvc::processAlg(const EventContext&, const std::string&, const AuditType)
     **/
    const TrigRoiDescriptor* roiDescriptor() const { return m_roi; }

  private:
    IProxyDict* m_proxy {nullptr};
    EventIDBase::number_type m_conditionsRun {EventIDBase::UNDEFNUM};
    const TrigRoiDescriptor* m_roi {nullptr};
  };
}

#endif

