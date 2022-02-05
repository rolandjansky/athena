/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//vitrual interface for all the Muon CSC technologies 
//
#ifndef ICSCConditionsSvc_h
#define ICSCConditionsSvc_h
//Gaudi Includes
#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"
//#include "MuonCondSvc/MuonHierarchy.h"
//STL includes
#include <list>
#include <string>
#include "CxxUtils/checker_macros.h"

//forward declarations
class Identifier;
class IdentifierHash;
class StatusCode;


class ATLAS_NOT_THREAD_SAFE ICSCConditionsSvc: virtual public IInterface{
public:
  virtual ~ICSCConditionsSvc(){}
  static const InterfaceID & interfaceID(); //!< reimplemented from IInterface
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS) = 0;
  virtual bool isGoodChamber(const Identifier & Id) const  = 0;
  virtual bool isGoodWireLayer(const Identifier & Id) const  = 0;
  virtual const std::vector<Identifier>& deadStationsId() const =0;
  virtual const std::vector<Identifier>& deadWireLayersId() const=0;

  virtual const std::vector<std::string>& deadStations() const =0;
  virtual const std::vector<std::string>& deadWireLayers() const=0;
  virtual bool isGood(const Identifier & Id) const =0;
};

inline const InterfaceID & ICSCConditionsSvc::interfaceID(){
  static const InterfaceID IID_ICSCConditionsSvc("CSCConditionsSvc",1,0);
  return IID_ICSCConditionsSvc;
}

#endif
