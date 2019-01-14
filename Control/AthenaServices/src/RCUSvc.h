// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaServices/src/RCUSvc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2016
 * @brief Service to allow cleaning up RCU objects at the EndEvent.
 */


#ifndef ATHENASERVICES_RCUSVC_H
#define ATHENASERVICES_RCUSVC_H


#include "AthenaKernel/IRCUSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IHiveWhiteBoard.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include <unordered_map>
#include <mutex>


namespace Athena {


/**
 * @brief Service to allow cleaning up RCU objects at the EndEvent.
 *
 * For a summary of RCU usage, see AthenaKernel/RCUObject.h.
 * This service keeps a registry of RCU objects.  At EndEvent,
 * it declares them quiescent for the current event slot.
 */
class RCUSvc
  : public extends<AthService, IRCUSvc, IIncidentListener>
{
public:
  /**
   * @brief Standard Gaudi constructor.
   * @param name Service name.
   * @param svc Service locator.
   */
  RCUSvc (const std::string& name, ISvcLocator* svc);


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Return the current number of event slots.
   */
  virtual size_t getNumSlots() const override;


  /**
   * @brief Add a new RCU object to the set being managed.
   * @param obj The RCU object to add.
   */
  virtual void add (IRCUObject* obj) override;


  /**
   * @brief Remove an RCU object to the set being managed.
   * @param obj The RCU object to remove.
   *
   * Returns failure if the requested object isn't in the managed set.
   */
  virtual StatusCode remove (IRCUObject* obj) override;


  /**
   * @brief Gaudi incident handler.
   *
   * Declare all managed objects quiescent at EndEvent.
   */
  virtual void handle (const Incident& inc) override;


private:
  /// Set of managed objects.
  typedef std::unordered_set<IRCUObject*> set_t;
  set_t m_objs;

  /// The hive WB service.  Used to get the current number of slots.
  ServiceHandle<IInterface> m_hiveWhiteBoardSvc;

  /// Gaudi incident service.
  ServiceHandle<IIncidentSvc> m_incidentSvc;

  /// Mutex protecting access to m_objs.
  std::mutex m_mutex;
  typedef std::lock_guard<std::mutex> lock_t;
};


} // namespace Athena


#endif // not ATHENASERVICES_RCUSVC_H
