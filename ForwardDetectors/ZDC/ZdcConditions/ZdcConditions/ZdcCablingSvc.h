/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCCONDITIONS_ZDCCABLINGSVC_H
#define ZDCCONDITIONS_ZDCCABLINGSVC_H

//#include "GaudiKernel/Service.h"
#include "AthenaBaseComps/AthService.h"
#include "ZdcConditions/ZdcCablingService.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/StoreGateSvc.h"

template <class TYPE> class SvcFactory;
//NGO This is a quick hack to make ZdcCablingService self contained, i.e. this
//NGO wrapper tool takes care of the initialization.
//NGO At some point ZdcCablingService should be made a tool, but this will introduce
//NGO many upstream code changes.

class ZdcCablingSvc : public AthService
{
  friend class SvcFactory<ZdcCablingSvc>;
  
 public:
  static const InterfaceID& interfaceID();
  ZdcCablingSvc(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~ZdcCablingSvc(){}

  StatusCode initialize();
  StatusCode finalize();
  StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  const ZdcCablingService* cablingService() const { return m_cablingService; }

 private:

  const ZdcCablingService* m_cablingService;

  ServiceHandle<StoreGateSvc> m_detStore;

  int m_cablingType;
};

#endif
