/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeaderGaudiClear.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/ArenaHeaderGaudiClear.cxx
 * @author scott snyder
 * @date May 2007
 * @brief An @c ArenaHeader that's cleared on every Gaudi event.
 *        Out-of-line implementations.
 */


#include "AthAllocators/ArenaHeaderGaudiClear.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IIncidentSvc.h"
#include <stdexcept>
#include <exception>


namespace SG {

#ifdef ATHENAHIVE
bool ArenaHeaderGaudiClear::m_disabled = true;
#else
/// True if @c disable has been called.
bool ArenaHeaderGaudiClear::m_disabled = false;
#endif

/**
 * @brief Constructor.
 */
ArenaHeaderGaudiClear::ArenaHeaderGaudiClear()
  : m_initialized (false),
    m_instanceCount (0)
{
}


/**
 * @brief Register with Gaudi.
 */
void ArenaHeaderGaudiClear::initialize()
{
  // Don't do anything if we're disabled or have already been called.
  if (m_initialized || m_disabled)
    return;

  // Find the Gaudi incident service.
  IIncidentSvc* incsvc;
  if (!(Gaudi::svcLocator()->service("IncidentSvc", incsvc)).isSuccess())
  {
    throw std::runtime_error("ArenaHeaderGaudiClear: "
                             "Could not locate Incident Service");
  }

  // Register with the service.
  const int PRIORITY = 100;
  incsvc->addListener(this, "BeginEvent", PRIORITY);

  m_initialized = true;
}


/**
 * @brief Handle a Gaudi incident.
 * @param inc The incident to handle.
 */
void ArenaHeaderGaudiClear::handle(const Incident& inc)
{
  // Reset on begin-event.
  if (inc.type() == "BeginEvent")
    reset();
}


/**
 * @brief Increase the reference count.  (Required by @c IInterface.)
 */
unsigned long ArenaHeaderGaudiClear::addRef()
{
  m_instanceCount++;
  return m_instanceCount;
}


/**
 * @brief Decrease the reference count.  (Required by @c IInterface.)
 */
unsigned long ArenaHeaderGaudiClear::release()
{
  m_instanceCount--;
  unsigned long count = m_instanceCount;
  if (count <= 0) delete this;
  return count;
}


/**
 * @brief Return the Gaudi interface for this object.
 *        (Required by @c IInterface.)
 */
StatusCode
ArenaHeaderGaudiClear::queryInterface(const InterfaceID& riid,
                                      void** ppvInterface)
{
  if (interfaceID().versionMatch(riid)) {
    *ppvInterface = this;
  }
  else {
    return StatusCode::FAILURE;
  }
  addRef();
  return StatusCode::SUCCESS;
}


/**
 * @brief Disable the Gaudi functionality.
 *
 * If this is called before @c initialize(), we will not attempt
 * to register ourselves with Gaudi.  This can be used for running
 * outside of the framework.
 */
void ArenaHeaderGaudiClear::disable()
{
  m_disabled = true;
}


} // namespace SG
