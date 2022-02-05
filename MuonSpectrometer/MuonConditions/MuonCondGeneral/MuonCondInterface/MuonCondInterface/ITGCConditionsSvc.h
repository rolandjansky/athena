/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//vitrual interface for all the Muon TGC technologies 
//
#ifndef ITGCConditionsSvc_h
#define ITGCConditionsSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
//#include "MuonCondSvc/MuonHierarchy.h"
//STL includes
#include <list>
#include <string>
#include "AthenaKernel/IOVSvcDefs.h"
#include "CxxUtils/checker_macros.h"
//forward declarations
class Identifier;
class IdentifierHash;
class StatusCode;


class ATLAS_NOT_THREAD_SAFE ITGCConditionsSvc: virtual public IInterface{
public:
  virtual ~ITGCConditionsSvc(){}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  virtual bool isGoodChamber(const Identifier & Id) = 0;
  virtual const std::vector<Identifier>& deadStationsId()=0;
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
};

inline const InterfaceID & ITGCConditionsSvc::interfaceID(){
  static const InterfaceID IID_ITGCConditionsSvc("TGCConditionsSvc",1,0);
  return IID_ITGCConditionsSvc;
}

#endif
