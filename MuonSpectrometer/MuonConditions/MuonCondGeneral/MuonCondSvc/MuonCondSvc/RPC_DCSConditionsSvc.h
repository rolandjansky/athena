/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPC_DCSCONDITIONSSVC_H
#define MUONCONDSVC_RPC_DCSCONDITIONSSVC_H

#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "AthenaBaseComps/AthService.h"
#include "MuonCondInterface/IRPC_DCSConditionsSvc.h"
#include "MuonCondInterface/IRPC_DCSConditionsTool.h"
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
class IRPC_DCSConditionsTool;
class IIOVSvc;

class RPC_DCSConditionsSvc : public AthService, virtual public IRPC_DCSConditionsSvc {
friend class SvcFactory<RPC_DCSConditionsSvc>;   
 public:
  RPC_DCSConditionsSvc (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RPC_DCSConditionsSvc();
  


  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  static const InterfaceID& interfaceID();  
  
  // Interface implementation
  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  
  virtual StatusCode initialize(void);

  virtual StatusCode finalize(void);


  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  //  virtual bool canReportAbout(MuonConditions::Hierarchy h);
    
  virtual bool isGoodPanel(const Identifier & Id) const;
  virtual bool isGoodStrip(const Identifier & Id) const;


  virtual const std::vector<Identifier>& deadPanelId() const;
  virtual const std::vector<Identifier>& offPanelId() const;

   virtual const std::vector<Identifier>& EffPanelId() const;
  virtual const std::vector<Identifier>& EffStripId() const;

  virtual const std::map<Identifier,     double>& RPC_EfficiencyMap()              ; 
  virtual const std::map<Identifier,     double>& RPC_EfficiencyGapMap()           ;
  virtual const std::map<Identifier,     double>& RPC_MeanClusterSizeMap()         ;
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize1Map()        ;
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize2Map()        ;
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize3Map()        ;
  virtual const std::map<Identifier,std::string>& RPC_DeadStripListMap()           ;
  virtual const std::map<Identifier,      float>& RPC_FracDeadStripMap()           ;
  virtual const std::map<Identifier,        int>& RPC_ProjectedTracksMap()         ;
  virtual const std::map<Identifier,        int>& RPC_DeadStripList()              ;
 
  virtual const std::map<Identifier,std::vector<double> >& RPC_TimeMapforStrip()           ;  

 protected: 


  std::vector<Identifier> m_cachedMaskPanelId;
  std::vector<Identifier> m_cachedeffStripId;
  std::vector<Identifier> m_cachedeffPanelId;


static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelEfficiencyMap_empty;
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_GapEfficiencyMap_empty;
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelMeanClusterSizeMap_empty;
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelFracClusterSize1Map_empty;
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelFracClusterSize2Map_empty;
static  std::map<Identifier ,double> s_RPCCondSummarySvc_RPC_PanelFracClusterSize3Map_empty;
static  std::map<Identifier ,std::string> s_RPCCondSummarySvc_RPC_PanelDeadStripListMap_empty;
static  std::map<Identifier ,float> s_RPCCondSummarySvc_RPC_PanelFracDeadStripMap_empty;
static  std::map<Identifier ,int> s_RPCCondSummarySvc_RPC_PanelProjectedTracksMap_empty;
static  std::map<Identifier ,int> s_RPCCondSummarySvc_RPC_PanelDeadStripList_empty;
 static  std::map<Identifier ,std::vector<double> > s_RPCCondSummarySvc_RPC_StripTimeMap_empty;
 private:
  
  
  std::string         m_DataLocation;
  std::string         m_dbToolType;
  std::string         m_dbToolName;
  BooleanProperty     m_dcsInfofromCool;
  
  ToolHandle<IRPC_DCSConditionsTool> m_condDataTool;
 
};

#endif
