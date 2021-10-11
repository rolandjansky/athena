/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRPC_DCSCONDITIONSSVC_H
#define IRPC_DCSCONDITIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"

//#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IRPCConditionsSvc.h"

class Identifier;
class IdentifierHash;
class StatusCode;




class ATLAS_NOT_THREAD_SAFE IRPC_DCSConditionsSvc : virtual public IRPCConditionsSvc {
  

public:


  virtual ~IRPC_DCSConditionsSvc(){}


  // map initialization method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS)=0;

 

  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
 
  //protected:

 


 private:




  
};

inline const InterfaceID & IRPC_DCSConditionsSvc::interfaceID(){
  static const InterfaceID IID_RPC_DCSConditionsSvc("RPC_DCSConditionsSvc",1,0);
  return IID_RPC_DCSConditionsSvc;
}
#endif  
