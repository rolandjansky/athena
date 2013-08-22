/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITRT_CONDITIONSTESTSVC_H
#define ITRT_CONDITIONSTESTSVC_H

/** @file TRT_ConditionsTestSvc.h
 *  @AlgTool for example and test of TRT_ConditionsSummarySvc.
 *  Simply returns InDet::TRT_GOOD.
 *  @author Denver Whittington
 */

#include "GaudiKernel/IService.h"

class Identifier;

class ITRT_ConditionsTestSvc : virtual public IService {

 public:

  //virtual ~ITRT_ConditionsTestSvc();

  /// Interface ID
  static const InterfaceID& interfaceID();

  /// Test!
  virtual StatusCode test( const Identifier& ) = 0;

 private:

};

inline const InterfaceID& ITRT_ConditionsTestSvc::interfaceID() {
  static const InterfaceID IID("ITRT_ConditionsTestSvc",1,0);
  return IID;
}

#endif // ITRT_CONDITIONSTESTSVC_H
