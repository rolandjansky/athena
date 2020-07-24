/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_EXTENDEDEVENTCONTEXT_H
#define ATHENAKERNEL_EXTENDEDEVENTCONTEXT_H

#include "AthenaKernel/IProxyDict.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventContext.h"
#include "CxxUtils/checker_macros.h"

class TrigRoiDescriptor; //!< Forward declaration
namespace SG {
class ThinningCache;
}

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


    /**
     * @brief Thread-local thinning cache.
     *        Set when we are doing output with thinning; the cache
     *        provides information about what was thinned.
     *        This is to allow converters to get thinning information.
     *        Unfortunately, we don't have a better way of doing this
     *        without changing Gaudi interfaces.
     */
    void setThinningCache (const SG::ThinningCache* cache) { m_thinningCache = cache; }
    const SG::ThinningCache* thinningCache() const { return m_thinningCache; }

  private:
    IProxyDict* m_proxy {nullptr};
    EventIDBase::number_type m_conditionsRun {EventIDBase::UNDEFNUM};
    const TrigRoiDescriptor* m_roi {nullptr};
    const SG::ThinningCache* m_thinningCache {nullptr};
  };


  //**********************************************************************
  // Out-of-line routines for setting and getting the event context.
  // Under some circumstances, the dynamic loader can bind the same
  // symbol to different addresses in different libraries.  Among
  // other effects, this can cause std::any to malfunction; std::any
  // is used inside EventContext to hold the extended context.
  // To try to avoid this, we provide out-of-line functions
  // to manipulate the extended context.

  /**
   * @brief Test whether a context object has an extended context installed.
   */
  bool hasExtendedEventContext (const EventContext& ctx);


  /**
   * @brief Retrieve an extended context from a context object.
   */
  const ExtendedEventContext& getExtendedEventContext (const EventContext& ctx);


  /**
   * @brief Retrieve an extended context from a context object.
   */
  ExtendedEventContext& getExtendedEventContext (EventContext& ctx);


  /**
   * @brief Move an extended context into a context object.
   */
  void setExtendedEventContext (EventContext& ctx,
                                ExtendedEventContext&& ectx);
} // namespace Atlas

#endif

