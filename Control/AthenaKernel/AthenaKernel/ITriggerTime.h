/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAKERNEL_ITRIGGERTIME_H
#define ATHENAKERNEL_ITRIGGERTIME_H
/** @file ITriggerTime.h
 * @brief  interface to a tool that returns the
 *         time offset of the current trigger. Used by PileUpMergeSvc
 * @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration
 *$Id: ITriggerTime.h,v 1.2 2007-06-23 01:12:06 calaf Exp $	
 */

#include "GaudiKernel/IAlgTool.h"

/** @class ITriggerTime
 * @brief  interface to a tool that returns the
 *         time offset of the current trigger. Used by PileUpMergeSvc
 */
class ITriggerTime : public virtual IAlgTool {
public:
  /// returns the time offset of the current trigger
  virtual double time() = 0;
  static const InterfaceID& interfaceID() {
    static const InterfaceID IID( "ITriggerTime", 1, 0 );
    return IID;
  }
};
#endif
