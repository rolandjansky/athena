// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthenaServices/src/ConditionsCleanerSvc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief Facade class for conditions garbage collection.
 */


#ifndef ATHENASERVICES_CONDITIONSCLEANERSVC_H
#define ATHENASERVICES_CONDITIONSCLEANERSVC_H


#include "AthenaKernel/IConditionsCleanerSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"


namespace Athena {


/**
 * @brief Facade class for conditions garbage collection.
 *
 * This forwards calls to @c IConditionsCleanerSvc to the service that
 * actually implements it.  Done like this so that we need only specify
 * the concrete class in one place.  The concrete service may be left
 * unspecified, in which case no garbage collection will be done.
 */
class ConditionsCleanerSvc
  : public extends<AthService, IConditionsCleanerSvc>
{
public:
  /**
   * @brief Standard Gaudi constructor.
   * @param name Service name.
   * @param svc Service locator.
   */
  ConditionsCleanerSvc (const std::string& name, ISvcLocator* svc);


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Called at the start of each event.
   * @param ctx The current event context.
   * @param allowAsync If true, then cleaning may be run
   *                   in an asynchronous TBB task.
   */
  virtual StatusCode event (const EventContext& ctx,
                            bool allowAsync) override;


  /**
   * @brief Called after a conditions object has been added.
   * @param ctx The current event context.
   * @param cc The container to which the object was added.
   */
  virtual StatusCode condObjAdded (const EventContext& ctx,
                                   CondContBase& cc) override;


  /**
   * @brief Print some statistics about the garbage collection.
   *        Would generally be called in finalize(), but broken out
   *        as a separate interface for testing/debugging purposes.
   */
  virtual StatusCode printStats() const override;


  /**
   * @brief Clear the internal state of the service.
   * Only for testing.  Don't call if any other thread may be touching the service.
   */
  virtual StatusCode reset() override;


private:
  ServiceHandle<Athena::IConditionsCleanerSvc> m_cleanerSvc;
};


} // namespace Athena


#endif // not ATHENASERVICES_CONDITIONSCLEANERSVC_H
