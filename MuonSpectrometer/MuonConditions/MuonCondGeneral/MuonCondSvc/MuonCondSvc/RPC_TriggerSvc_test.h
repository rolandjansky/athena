/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPC_TRIGGERSVC_TEST_H
#define MUONCONDSVC_RPC_TRIGGERSVC_TEST_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "MuonCondInterface/IRPCTriggerDbTool.h"
#include "MuonCondInterface/IRPC_TriggerSvc_test.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCondSvc/MuonHierarchy.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h" 

#include <vector>
#include <list>
#include <string>
#include <map>

class ISvcLocator;
class IdentifierHash;
class StatusCode;
class IRPCTriggerDbTool;
class IIOVSvc;

class RPC_TriggerSvc_test : public AthService, virtual public IRPC_TriggerSvc_test {
 
 
 public:
  RPC_TriggerSvc_test (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RPC_TriggerSvc_test();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvIF );

  //static const InterfaceID& interfaceID();  
  
  
    
  virtual StatusCode initialize(void);
  virtual StatusCode finalize(void);

  StatusCode loadParameterInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));

 
 private:

  
  std::string         m_DataLocation;
  std::string         m_dbToolType;
  std::string         m_dbToolName;

  
  ToolHandle<IRPCTriggerDbTool> m_condDataTool;
 
};

#endif
