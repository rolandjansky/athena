/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
* @file SCT_CalibModuleListSvc.h
* Header file for the SCT_CalibModuleListSvc class
* @author Shaun Roe
**/

#ifndef SCT_CalibModuleListSvc_h
#define SCT_CalibModuleListSvc_h

// STL and boost headers
#include <string>
#include <vector>
#include <set>
#include <map>

#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/IIOVDbSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "SCT_CalibAlgs/ISCT_CalibModuleListSvc.h"       
#include "SCT_ConditionsServices/ISCT_MonitorConditionsSvc.h" 

#include "Identifier/Identifier.h"
#include "InDetIdentifier/SCT_ID.h"


//fwd declarations
template <class TYPE> class SvcFactory;


class SCT_CalibModuleListSvc:virtual public ISCT_CalibModuleListSvc, public AthService{
  friend class SvcFactory<SCT_CalibModuleListSvc>;
public:
  //@name Service methods, reimplemented
   //@{
  SCT_CalibModuleListSvc(const std::string &name, ISvcLocator * svc); 
  virtual ~SCT_CalibModuleListSvc();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode queryInterface(const InterfaceID & riid, void** ppvInterface );
   //@}
  virtual StatusCode readModuleList( std::map< Identifier, std::set<Identifier> >& moduleList );
   
private:
  const SCT_ID* m_pSCTHelper;
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<ISCT_MonitorConditionsSvc> m_MonitorConditionsSvc;
  ServiceHandle<IIOVDbSvc> m_IOVDbSvc;
  
};
#endif
