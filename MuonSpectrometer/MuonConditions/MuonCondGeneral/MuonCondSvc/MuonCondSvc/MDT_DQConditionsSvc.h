/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MDT_DQCONDITIONSSVC_H
#define MUONCONDSVC_MDT_DQCONDITIONSSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IMDT_DQConditionsSvc.h"
#include "MuonCondInterface/IMDT_DQConditionsTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

class IMDT_DQConditionsTool;
class IIOVSvc;
class Identifier;

class MDT_DQConditionsSvc : public AthService, virtual public IMDT_DQConditionsSvc {
friend class SvcFactory<MDT_DQConditionsSvc>;    
 public:
  MDT_DQConditionsSvc (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MDT_DQConditionsSvc();
  
  //static const InterfaceID& interfaceID() { return InterfaceID_IMDT_DQConditionsSvc; }
  virtual StatusCode queryInterface(const InterfaceID& riid,void** ppvIF);
  
   static const InterfaceID& interfaceID();  
  // Interface implementation
  StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  //StatusCode initDQChamberInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));

  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);
    virtual const std::vector<std::string>& deadTubes() const;
  virtual const std::vector<std::string>& deadStations() const;
  virtual const std::vector<std::string>& deadMultiLayers() const;

  virtual const std::vector<Identifier>& deadTubesId() const;
  virtual const std::vector<Identifier>& deadStationsId() const;
  virtual const std::vector<Identifier>& deadMultiLayersId() const;
  virtual const std::vector<Identifier>& deadLayersId() const;

  virtual bool canReportAbout(MuonConditions::Hierarchy h);

  virtual const std::vector<Identifier>& List_Chambers_with_deadTube() const;
    
  virtual bool isGoodChamber(const Identifier & Id) const;
  virtual bool isGoodMultiLayer(const Identifier & Id) const;
  virtual bool isGoodChannel(const Identifier & Id) const;  
  
 protected:

  std::vector<std::string> m_cachedDeadTubes;
  std::vector<std::string> m_cachedDeadMultiLayers;
  std::vector<std::string> m_cachedDeadStations;
 
  std::vector<Identifier> m_cachedDeadTubesId;
  std::vector<Identifier> m_cachedDeadMultiLayersId;
  std::vector<Identifier> m_cachedDeadLayersId;
  std::vector<Identifier> m_cachedDeadStationsId;
  std::vector<Identifier> m_Chamber_with_deadTube;


private:
  
  
  std::string         m_DataLocation;
  
  BooleanProperty     m_dqInfofromCool;
  
  ToolHandle<IMDT_DQConditionsTool> m_condDataTool;
 
};

#endif
