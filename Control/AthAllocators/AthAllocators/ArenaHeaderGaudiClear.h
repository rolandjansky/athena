// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeaderGaudiClear.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHeaderGaudiClear.h
 * @author scott snyder
 * @date May 2007
 * @brief An @c ArenaHeader that's cleared on every Gaudi event.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHEADERGAUDICLEAR_H
#define ATLALLOCATORS_ARENAHEADERGAUDICLEAR_H


#include "AthAllocators/ArenaHeader.h"
#include "GaudiKernel/IIncidentListener.h"


namespace SG {


/**
 * @brief An @c ArenaHeader that's cleared on every Gaudi event.
 *
 * This is a version of @c ArenaHeader on which @c reset will be called
 * automatically on every new event.
 */
class ArenaHeaderGaudiClear
  : public ArenaHeader,
    public IIncidentListener
{
public:
  /**
   * @brief Constructor.
   */
  ArenaHeaderGaudiClear();


  /**
   * @brief Register with Gaudi.
   */
  void initialize();


  /**
   * @brief Handle a Gaudi incident.
   * @param inc The incident to handle.
   */
  virtual void handle(const Incident& inc);


  /**
   * @brief Increase the reference count.  (Required by @c IInterface.)
   */
  virtual unsigned long addRef();


  /**
   * @brief Decrease the reference count.  (Required by @c IInterface.)
   */
  virtual unsigned long release();


  /**
   * @brief Return the Gaudi interface for this object.
   *        (Required by @c IInterface.)
   */
  virtual StatusCode queryInterface(const InterfaceID& riid,
                                    void** ppvInterface);


  /**
   * @brief Disable the Gaudi functionality.
   *
   * If this is called before @c initialize(), we will not attempt
   * to register ourselves with Gaudi.  This can be used for running
   * outside of the framework.
   */
  static void disable();


private:
  /// True after we've called @c initialize().
  bool m_initialized;

  /// Current reference count.
  unsigned int m_instanceCount;

  /// True if @c disable has been called.
  static bool m_disabled;
};


} // namespace SG


#endif // not ATLALLOCATORS_ARENAHEADERGAUDICLEAR_H

