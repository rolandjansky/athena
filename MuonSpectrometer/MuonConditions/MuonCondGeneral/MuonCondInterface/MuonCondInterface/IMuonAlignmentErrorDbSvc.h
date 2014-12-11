/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMUONALIGNMENTERRORDBSVC_H
#define IMUONALIGNMENTERRORDBSVC_H

#include "GaudiKernel/IInterface.h"
#include <string>
#include <vector>
#include <algorithm>
#include "AthenaKernel/IOVSvcDefs.h"

//#include "MuonCondSvc/MuonHierarchy.h"


class Identifier;
class IdentifierHash;
class StatusCode;




class IMuonAlignmentErrorDbSvc : virtual public IInterface {
  

public:


  virtual ~IMuonAlignmentErrorDbSvc(){}


  // map initialization method
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;

  virtual const std::string& ListErrorValue() const=0 ;
  

  //virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();
 
  //protected:



 private:
  
};

inline const InterfaceID & IMuonAlignmentErrorDbSvc::interfaceID(){
  static const InterfaceID IID_IMuonAlignmentErrorDbSvc("MuonAlignmentErrorDbSvc",1,0);
  return IID_IMuonAlignmentErrorDbSvc;
}
#endif  
