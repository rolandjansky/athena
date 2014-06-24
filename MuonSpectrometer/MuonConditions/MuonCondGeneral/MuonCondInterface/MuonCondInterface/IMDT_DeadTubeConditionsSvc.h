/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMDT_DEADTUBECONDITIONSSVC_H
#define IMDT_DEADTUBECONDITIONSSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"
//#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IMDTConditionsSvc.h"
class Identifier;
class IdentifierHash;
class StatusCode;


//static const InterfaceID IID_MDT_DeadTubeConditionsSvc("MDT_DeadTubeConditionsSvc", 1, 0);

class IMDT_DeadTubeConditionsSvc : virtual public IMDTConditionsSvc
{
  public:

  // / Retrieve interface ID
  //static const InterfaceID& interfaceID() { return IID_MDT_DeadTubeConditionsSvc; }

  // map initialization method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
  
  
  //  virtual const std::vector<std::string>& deadTubes() const =0 ;
  
  // virtual const std::vector<Identifier>& deadTubesId()const =0 ;
  //virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const =0;  


  static const InterfaceID & interfaceID();

};

inline const InterfaceID & IMDT_DeadTubeConditionsSvc::interfaceID(){
  static const InterfaceID IID_MDT_DeadTubeConditionsSvc("MDT_DeadTubeConditionsSvc",1,0);
  return IID_MDT_DeadTubeConditionsSvc;
}
#endif  //

