/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_CSC_DCSCONDITIONSSVC_H
#define MUONCONDSVC_CSC_DCSCONDITIONSSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "MuonCondInterface/ICSC_DCSConditionsSvc.h"
#include "MuonCondInterface/ICSC_DCSConditionsTool.h"
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

template <class TYPE> class SvcFactory;
class ISvcLocator;
class IdentifierHash;
class StatusCode;
class ICSC_DCSConditionsTool;
class IIOVSvc;

class CSC_DCSConditionsSvc : public AthService, virtual public ICSC_DCSConditionsSvc {
friend class SvcFactory<CSC_DCSConditionsSvc>;   
 public:
  CSC_DCSConditionsSvc (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CSC_DCSConditionsSvc();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  static const InterfaceID& interfaceID();  
  
  // Interface implementation
  StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  
  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);


  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(MuonConditions::Hierarchy h);
    
  
  virtual bool isGoodChamber(const Identifier & Id) const;
  virtual bool isGoodWireLayer(const Identifier & Id) const;
  virtual bool isGood(const Identifier & Id) const;


  virtual const std::vector<Identifier>& deadStationsId() const;
  virtual const std::vector<Identifier>& deadWireLayersId() const;

  virtual const std::vector<std::string>& deadStations() const;
  virtual const std::vector<std::string>& deadWireLayers() const;

 protected: 

  std::vector<Identifier> m_cachedDeadWireLayersId;
  std::vector<Identifier> m_cachedDeadStationsId;

  std::vector<std::string> m_cachedDeadWireLayers;
  std::vector<std::string> m_cachedDeadStationsStr;

 private:
  
  
  std::string         m_DataLocation;
  std::string         m_dbToolType;
  std::string         m_dbToolName;
  BooleanProperty     m_dcsInfofromCool;
  
  ToolHandle<ICSC_DCSConditionsTool> m_condDataTool;
 
};

#endif
