/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MDT_DCSCONDITIONSSVC_H
#define MUONCONDSVC_MDT_DCSCONDITIONSSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "MuonCondInterface/IMDT_DCSConditionsSvc.h"
#include "MuonCondInterface/IMDT_DCSConditionsTool.h"
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
class IMDT_DCSConditionsTool;
class IIOVSvc;

class MDT_DCSConditionsSvc : public AthService, virtual public IMDT_DCSConditionsSvc {
friend class SvcFactory<MDT_DCSConditionsSvc>;   
 public:
  MDT_DCSConditionsSvc (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MDT_DCSConditionsSvc();
  
  //static const InterfaceID& interfaceID() { return InterfaceID_IMDT_DCSConditionsSvc; }
  //virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  static const InterfaceID& interfaceID();  
  
  // Interface implementation
  StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));  
  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);


  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(MuonConditions::Hierarchy h);
    
  
  virtual bool isGoodChamber(const Identifier & Id) const;
  virtual bool isGoodMultiLayer(const Identifier & Id) const;
  virtual bool isGoodChannel(const Identifier & Id) const;


  virtual const std::vector<std::string>& deadStations() const;
  virtual const std::vector<std::string>& deadMultiLayers() const;
  virtual const std::vector<std::string>& deadTubes() const;

  virtual const std::vector<Identifier>& deadTubesId() const;
  virtual const std::vector<Identifier>& deadStationsId() const;
  virtual const std::vector<Identifier>& deadMultiLayersId() const;
  virtual const std::vector<Identifier>& deadLayersId() const;
  virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const;  
 protected: 

  std::vector<std::string> m_cachedDeadTubes;
  std::vector<std::string> m_cachedDeadMultiLayers;
  std::vector<std::string> m_cachedDeadStations;

  std::vector<Identifier> m_cachedDeadTubesId;
  std::vector<Identifier> m_cachedDeadMultiLayersId;
  std::vector<Identifier> m_cachedDeadLayersId;
  std::vector<Identifier> m_cachedDeadStationsId;
  std::vector<Identifier> m_Chambers_with_deadTube;
 private:
  
  
  std::string         m_DataLocation;
  std::string         m_dbToolType;
  std::string         m_dbToolName;
  BooleanProperty     m_dcsInfofromCool;
  
  ToolHandle<IMDT_DCSConditionsTool> m_condDataTool;
 
};

#endif
