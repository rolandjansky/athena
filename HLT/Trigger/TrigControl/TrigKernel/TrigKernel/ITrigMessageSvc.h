/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**@class ITrigMessageSvc
 * @brief Additional MessageSvc interface for TrigMessageSvc
 *
 * Provides additional methods for the online message service
 *
 * @author  Frank Winklmeier
 * @version $Id: ITrigMessageSvc.h 2 2013-05-10 13:12:14Z ricab $
 */

#ifndef TRIGKERNEL_ITRIGMESSAGESVC_H
#define TRIGKERNEL_ITRIGMESSAGESVC_H

#include "GaudiKernel/IInterface.h"

class ITrigMessageSvc: virtual public IInterface {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID();

  /*
   * Reset all individual output levels
   */
  virtual void resetOutputLevels() = 0;
  
};

inline const InterfaceID& ITrigMessageSvc::interfaceID()
{
  static const InterfaceID IID_ITrigMessageSvc("ITrigMessageSvc", 1, 0);
  return IID_ITrigMessageSvc;
}

#endif
