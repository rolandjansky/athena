/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPCSTATUSDBSVC_H
#define MUONCONDSVC_RPCSTATUSDBSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"

#include "MuonCondInterface/IRpcDetectorStatusDbTool.h"
//#include "MuonCondData/RpcCalibDataContainer.h"
//#include "MuonCondData/RpcStripStatus.h"


class IIOVSvc;

const InterfaceID InterfaceID_IRpcStatusDbSvc("RpcStatusDbSvc", 1, 0);
class IRpcStatusDBTool;


class RpcStatusDbSvc : public AthService, virtual public IInterface {
  friend class SvcFactory<RpcStatusDbSvc>; 
 public:
   RpcStatusDbSvc (const std::string& name, ISvcLocator* pSvcLocator);

  
  
  static const InterfaceID& interfaceID() { return InterfaceID_IRpcStatusDbSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
  
  
 public:
  
  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);
 
 private:
  


   /** Tool handling the DB access */
  IRpcDetectorStatusDbTool * m_dbTool;

/** Indexed with MdtRegionHash for rt-regions. */ 
/** Properties: */


 std::string         m_rpcStatusDataLocation;
 std::string         m_dbToolType;
 std::string         m_dbToolName;

 
};
//}
#endif
