/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthenaServices/src/ConditionsCleanerSvc.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief Facade class for conditions garbage collection.
 */


#include "ConditionsCleanerSvc.h"


namespace Athena {


/**
 * @brief Standard Gaudi constructor.
 * @param name Service name.
 * @param svc Service locator.
 */
ConditionsCleanerSvc::ConditionsCleanerSvc (const std::string& name,
                                            ISvcLocator* svc)
  : base_class (name, svc),
    m_cleanerSvc ("", name)
{
  declareProperty ("CleanerSvc", m_cleanerSvc,
                   "Service implementing conditions garbage collection.  "
                   "If blank, no garbage collection will be done.");
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode ConditionsCleanerSvc::initialize()
{
  if (!m_cleanerSvc.empty()) {
    ATH_CHECK( m_cleanerSvc.retrieve() );
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Called at the start of each event.
 * @param ctx The current event context.
 * @param allowAsync If true, then cleaning may be run
 *                   in an asynchronous TBB task.
 */
StatusCode
ConditionsCleanerSvc::event (const EventContext& ctx,
                             bool allowAsync)
{
  if (!m_cleanerSvc.empty()) {
    return m_cleanerSvc->event (ctx, allowAsync);
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Called after a conditions object has been added.
 * @param ctx The current event context.
 * @param cc The container to which the object was added.
 */
StatusCode ConditionsCleanerSvc::condObjAdded (const EventContext& ctx,
                                               CondContBase& cc)
{
  if (!m_cleanerSvc.empty()) {
    return m_cleanerSvc->condObjAdded (ctx, cc);
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Print some statistics about the garbage collection.
 *        Would generally be called in finalize(), but broken out
 *        as a separate interface for testing/debugging purposes.
 */
StatusCode ConditionsCleanerSvc::printStats() const
{
  if (!m_cleanerSvc.empty()) {
    return m_cleanerSvc->printStats();
  }
  return StatusCode::SUCCESS;
}


/**
 * @brief Clear the internal state of the service.
 * Only for testing.  Don't call if any other thread may be touching the service.
 */
StatusCode ConditionsCleanerSvc::reset()
{
  if (!m_cleanerSvc.empty()) {
    return m_cleanerSvc->reset();
  }
  return StatusCode::SUCCESS;
}


} // namespace Athena
