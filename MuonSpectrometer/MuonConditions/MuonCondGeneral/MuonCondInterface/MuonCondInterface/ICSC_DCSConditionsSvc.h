/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ICSC_DCSCONDITIONSSVC_H
#define ICSC_DCSCONDITIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"


#include "MuonCondInterface/ICSCConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;


class ATLAS_NOT_THREAD_SAFE ICSC_DCSConditionsSvc : virtual public ICSCConditionsSvc {

public:

  virtual ~ICSC_DCSConditionsSvc(){}

  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
  static const InterfaceID & interfaceID();

 private:
  
};

inline const InterfaceID & ICSC_DCSConditionsSvc::interfaceID(){
  static const InterfaceID IID_CSC_DCSConditionsSvc("CSC_DCSConditionsSvc",1,0);
  return IID_CSC_DCSConditionsSvc;
}
#endif  
