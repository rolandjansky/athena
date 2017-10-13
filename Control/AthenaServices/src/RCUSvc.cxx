/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaServices/src/RCUSvc.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2016
 * @brief Service to allow cleaning up RCU objects at the EndEvent.
 */


#include "RCUSvc.h"


namespace Athena {


/**
 * @brief Standard Gaudi constructor.
 * @param name Service name.
 * @svc Service locator.
 */
RCUSvc::RCUSvc (const std::string& name, ISvcLocator* svc)
  : extends<AthService, IRCUSvc, IIncidentListener> (name, svc),
  m_hiveWhiteBoardSvc ("EventDataSvc", name),
  m_incidentSvc ("IncidentSvc", name)
{
  declareProperty ("HiveWhiteBoardSvc", m_hiveWhiteBoardSvc);
  declareProperty ("IncidentSvc", m_incidentSvc);
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode RCUSvc::initialize()
{
  ATH_CHECK( AthService::initialize() );
  ATH_CHECK( m_hiveWhiteBoardSvc.retrieve() );
  ATH_CHECK( m_incidentSvc.retrieve() );

  m_incidentSvc->addListener (this, IncidentType::EndEvent);
  return StatusCode::SUCCESS;
}


/**
 * @brief Return the current number of event slots.
 */
size_t RCUSvc::getNumSlots() const
{
  // In non-MT mode, `EventDataSvc' does not have an IHiveWhiteBoard interface.
  // Fall back to one slot in that case.
  const SmartIF<IHiveWhiteBoard> wb (m_hiveWhiteBoardSvc.get());
  if (wb) {
    return wb->getNumberOfStores();
  }
  return 1;
}


/**
 * @brief Add a new RCU object to the set being managed.
 * @param obj The RCU object to add.
 */
void RCUSvc::add (IRCUObject* obj)
{
  lock_t g (m_mutex);
  m_objs.insert (obj);
}


/**
 * @brief Remove an RCU object to the set being managed.
 * @param obj The RCU object to remove.
 *
 * Returns failure if the requested object isn't in the managed set.
 */
StatusCode RCUSvc::remove (IRCUObject* obj)
{
  lock_t g (m_mutex);
  set_t::iterator it = m_objs.find (obj);
  if (it == m_objs.end())
    return StatusCode::FAILURE;
  m_objs.erase (it);
  return StatusCode::SUCCESS;
}


/**
 * @brief Gaudi incident handler.
 *
 * Declare all managed objects quiescent at EndEvent.
 */
void RCUSvc::handle (const Incident& inc)
{
  lock_t g (m_mutex);
  if (inc.type() == IncidentType::EndEvent) {
    for (IRCUObject* p : m_objs)
      p->quiescent (inc.context());
  }
}


} // namespace Athena

