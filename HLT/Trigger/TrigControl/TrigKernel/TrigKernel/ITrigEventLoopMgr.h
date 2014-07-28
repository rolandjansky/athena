/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**@class ITrigEventLoopMgr
 * @brief EventLoopMgr interface implemented by the HLT event loop manager
 *
 * Provides additional interfaces for TDAQ state transitions used by the PSC.
 * It follows the usual Gaudi philosophy of separation framework and client
 * implemenation into sysXYZ and XYZ methods.
 *
 * @author  Frank Winklmeier
 * @version $Id: ITrigEventLoopMgr.h 39 2013-06-10 17:21:44Z ricab $
 */

#ifndef TRIGKERNEL_ITRIGEVENTLOOPMGR_H
#define TRIGKERNEL_ITRIGEVENTLOOPMGR_H

#include "eformat/ROBFragment.h"
#include "GaudiKernel/IInterface.h"
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <stdint.h>

namespace hltinterface
{
  class EventId;
  class HLTResult;
}

class ITrigEventLoopMgr: virtual public IInterface {
public:
  /*
   * Retrieve interface ID
   */
  static const InterfaceID& interfaceID();

  /**
   * prepareForRun method invoked by framework
   */
  virtual StatusCode prepareForRun(const boost::property_tree::ptree &) = 0;

  /**
   * propagate a processing time out via the HLT framework to the steering/algorithms
   */
  virtual StatusCode timeOutReached() = 0;

  /**
   * selective conditions update for HLT steering/algorithms
   */
  virtual StatusCode hltConditionsUpdate(std::vector<uint32_t>&) = 0;

  /**
   * HLT prescale update
   */
  virtual StatusCode hltPrescaleUpdate(uint32_t counter) = 0;

  /// process one event/RoI
  virtual StatusCode
  processRoIs(const std::vector<eformat::ROBFragment<const uint32_t*> >&,
              hltinterface::HLTResult&,
              const hltinterface::EventId& evId) = 0;

};

inline const InterfaceID& ITrigEventLoopMgr::interfaceID()
{
  static const InterfaceID IID_ITrigEventLoopMgr("ITrigEventLoopMgr", 20, 0);
  return IID_ITrigEventLoopMgr;
}

#endif
