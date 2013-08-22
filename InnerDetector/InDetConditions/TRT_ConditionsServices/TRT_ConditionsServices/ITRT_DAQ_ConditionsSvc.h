/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_DAQ_CONDITIONSSVC_H
#define ITRT_DAQ_CONDITIONSSVC_H

/*----------------------------------------------------
 *  @file ITRT_DAQ_ConditionsSvc.h
 *  @IService Interface for TRT_DAQ_ConditionsSvc
 *  @author Denver Whittington
 *///-------------------------------------------------

// Header Includes
#include "GaudiKernel/IService.h"
#include "TRT_ConditionsServices/ITRT_ConditionsSvc.h" // for InDet::TRT_CondFlag

// Class definition for this Service
class ITRT_DAQ_ConditionsSvc : virtual public IService {

 public:

  /// Interface ID
  static const InterfaceID& interfaceID();

  /// ConditionsSummaryStatus for a ROD ID
  /** Similar to ITRT_ConditionsSvc::condSummaryStatus,
   *  but specific to this service)
   */
  virtual InDet::TRT_CondFlag condSummaryStatus( unsigned int ) = 0;

};

inline const InterfaceID& ITRT_DAQ_ConditionsSvc::interfaceID() {
  static const InterfaceID IID("ITRT_DAQ_ConditionsSvc",1,0);
  return IID;
}

#endif // ITRT_DAQ_CONDITIONSSVC_H
