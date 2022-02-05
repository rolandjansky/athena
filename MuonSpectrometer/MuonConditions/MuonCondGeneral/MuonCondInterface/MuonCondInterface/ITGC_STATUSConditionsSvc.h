/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITGC_STATUSCONDITIONSSVC_H
#define ITGC_STATUSCONDITIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"

//#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/ITGCConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;

//static const InterfaceID IID_TGC_STATUSConditionsSvc("TGC_STATUSConditionsSvc", 1, 0);


class ATLAS_NOT_THREAD_SAFE ITGC_STATUSConditionsSvc : virtual public ITGCConditionsSvc {
  

public:


  virtual ~ITGC_STATUSConditionsSvc(){}
  // / Retrieve interface ID
  //static const InterfaceID& interfaceID() { return IID_TGC_STATUSConditionsSvc; }

  // map initialization method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;

  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
 
  //protected:


 private:
  
};

inline const InterfaceID & ITGC_STATUSConditionsSvc::interfaceID(){
  static const InterfaceID IID_TGC_STATUSConditionsSvc("TGC_STATUSConditionsSvc",1,0);
  return IID_TGC_STATUSConditionsSvc;
}
#endif  
