/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file AthenaKernel/src/RCUObject.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2016
 * @brief read-copy-update (RCU) style synchronization for Athena.
 */


#include "AthenaKernel/RCUObject.h"
#include "AthenaKernel/IRCUSvc.h"
#include <cstdlib>


namespace Athena {


/**
 * @brief Constructor, with RCUSvc.
 * @param svc Service with which to register.
 *
 * The service will call @c quiescent at the end of each event.
 */
IRCUObject::IRCUObject (IRCUSvc& svc)
  : m_svc (&svc),
    m_grace (svc.getNumSlots()),
    m_oldGrace (svc.getNumSlots()),
    m_nold(0),
    m_dirty(false)
{
  m_svc->add (this);
}


/**
 * @brief Constructor, with event slot count.
 * @param nslots Number of active event slots.
 *
 * This version does not register with a service.
 */
IRCUObject::IRCUObject (size_t nslots)
  : m_svc (nullptr),
    m_grace (nslots),
    m_oldGrace (nslots),
    m_nold(0),
    m_dirty(false)
{
}


/**
 * @brief Destructor.
 *
 * Remove this object from the service if it has been registered.
 */
IRCUObject::~IRCUObject()
{
  if (m_svc && m_svc->remove (this).isFailure())
    std::abort();
}


/**
 * @brief Move constructor.
 *
 * Allow passing these objects via move.
 */
IRCUObject::IRCUObject (IRCUObject&& other)
  : m_svc (other.m_svc),
    m_grace (std::move (other.m_grace)),
    m_oldGrace (std::move (other.m_oldGrace)),
    m_nold (other.m_nold),
    m_dirty (false)
{
  other.m_nold = 0;
  if (other.m_dirty) {
    m_dirty = true;
  }
  other.m_dirty = false;
  if (m_svc) {
    if (m_svc->remove (&other).isFailure()) {
      std::abort();
    }
    other.m_svc = nullptr;
    m_svc->add (this);
  }
}


} // namespace Athena
