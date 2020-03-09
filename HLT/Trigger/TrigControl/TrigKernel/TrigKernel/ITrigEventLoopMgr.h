/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGKERNEL_ITRIGEVENTLOOPMGR_H
#define TRIGKERNEL_ITRIGEVENTLOOPMGR_H

#include "GaudiKernel/IInterface.h"
#include "CxxUtils/checker_macros.h"
#include <boost/property_tree/ptree.hpp>

/**@class ITrigEventLoopMgr
 * @brief EventLoopMgr interface implemented by the HLT event loop manager
 *
 * Provides additional interfaces for TDAQ state transitions used by the PSC.
 *
 * @author  Frank Winklmeier
 */
class ITrigEventLoopMgr: virtual public IInterface {
public:
  /// Interface ID
  DeclareInterfaceID(ITrigEventLoopMgr, 21, 0);

  /**
   * prepareForRun method invoked by framework
   */
  virtual StatusCode prepareForRun  ATLAS_NOT_THREAD_SAFE (const boost::property_tree::ptree &) = 0;

  /**
   * update parameters if necessary after forking workers and issue incident
   */
  virtual StatusCode hltUpdateAfterFork(const boost::property_tree::ptree &) = 0;
};

#endif
