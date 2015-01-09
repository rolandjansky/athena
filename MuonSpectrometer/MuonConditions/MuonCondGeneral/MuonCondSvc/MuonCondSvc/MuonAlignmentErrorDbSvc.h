/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MUONALIGNMENTERRORDBSVC_H
#define MUONCONDSVC_MUONALIGNMENTERRORDBSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"

//#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonCondInterface/IMuonAlignmentErrorDbSvc.h"
#include "MuonCondInterface/IMuonAlignmentErrorDbTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <vector>
#include <list>
#include <string>
#include <map>
//
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonCondSvc/MuonHierarchy.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h" 


template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class StatusCode;
class IMuonAlignmentErrorDbTool;
class IIOVSvc;

class MuonAlignmentErrorDbSvc : public AthService, virtual public IMuonAlignmentErrorDbSvc {
  friend class SvcFactory<MuonAlignmentErrorDbSvc>;   
 public:
  MuonAlignmentErrorDbSvc (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MuonAlignmentErrorDbSvc();
  
  
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  
  static const InterfaceID& interfaceID();  
  
  // Interface implementation
  StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));  
  virtual StatusCode initialize(void);
  
  virtual StatusCode finalize(void);
  
  
  virtual const std::string& ListErrorValue() const;
    
  
 protected: 


  
 private:
  
  
  std::string         m_DataLocation;
  std::string         m_dbToolType;
  std::string         m_dbToolName;
  
  
  ToolHandle<IMuonAlignmentErrorDbTool> m_condDataTool;
  
};

#endif
