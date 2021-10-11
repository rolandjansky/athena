/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_TGC_STATUSCONDITIONSSVC_H
#define MUONCONDSVC_TGC_STATUSCONDITIONSSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
//#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"

#include "MuonCondInterface/ITGC_STATUSConditionsSvc.h"
#include "MuonCondInterface/ITGC_STATUSConditionsTool.h"
#include "MuonCondSvc/MuonHierarchy.h"
#include "AthenaKernel/IOVSvcDefs.h"

/* #include <vector> */
/* #include <list> */
/* #include <string> */
/* #include <map> */




//#include "AthenaKernel/IIOVDbSvc.h" 


template <class TYPE> class SvcFactory;

class ISvcLocator;
class IdentifierHash;
class StatusCode;
class ITGC_STATUSConditionsTool;
class IIOVSvc;


class ATLAS_NOT_THREAD_SAFE TGC_STATUSConditionsSvc : public AthService, virtual public ITGC_STATUSConditionsSvc {

  friend class SvcFactory<TGC_STATUSConditionsSvc>;   
  
 public:
  TGC_STATUSConditionsSvc (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TGC_STATUSConditionsSvc();
  
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  
  virtual StatusCode initialize(void);
  virtual StatusCode finalize(void);

  StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  
  
  
  virtual bool canReportAbout(MuonConditions::Hierarchy h);

  static const InterfaceID& interfaceID();  
  virtual bool isGoodChamber(const Identifier & Id);
  virtual const std::vector<Identifier>& deadStationsId();

  
 protected: 
  std::vector<Identifier> m_cachedDeadStationsId;

  
 private:
  
  std::string         m_DataLocation;
  std::string         m_dbToolType;
  std::string         m_dbToolName;
  BooleanProperty     m_useCool;
  
  ToolHandle<ITGC_STATUSConditionsTool> m_condDataTool;
 
};

#endif
