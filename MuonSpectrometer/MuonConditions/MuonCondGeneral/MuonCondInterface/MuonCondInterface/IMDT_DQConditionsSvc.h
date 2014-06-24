/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMDT_DQCONDITIONSSVC_H
#define IMDT_DQCONDITIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"
class Identifier;
class IdentifierHash;
class StatusCode;




class IMDT_DQConditionsSvc : virtual public IMDTConditionsSvc
{
  public:

  // map initialization method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
  


  static const InterfaceID & interfaceID();

};

inline const InterfaceID & IMDT_DQConditionsSvc::interfaceID(){
  static const InterfaceID IID_MDT_DQConditionsSvc("MDT_DQConditionsSvc",1,0);
  return IID_MDT_DQConditionsSvc;
}
#endif  //

