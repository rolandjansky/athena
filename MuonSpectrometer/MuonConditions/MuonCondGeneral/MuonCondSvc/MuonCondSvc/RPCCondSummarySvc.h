/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_RPCCONDSUMMARYSVC_H
#define MUONCONDSVC_RPCCONDSUMMARYSVC_H

#include "MuonCondInterface/IRPCConditionsSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "MuonCondSvc/MuonHierarchy.h"
#include "MuonCondInterface/IRPC_STATUSConditionsSvc.h"
#include "MuonCondInterface/IRPC_DCSConditionsSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include <set>

//forward declarations
template <class TYPE> class SvcFactory;

class RPCCondSummarySvc: virtual public IRPCConditionsSvc, public AthService{
  friend class SvcFactory<RPCCondSummarySvc>;
public:
  
  RPCCondSummarySvc( const std::string & name, ISvcLocator* svc);
  virtual ~RPCCondSummarySvc()=default;
  virtual StatusCode initialize();
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  static const InterfaceID & interfaceID();

  virtual StatusCode initInfo(IOVSVC_CALLBACK_ARGS_P(I,keys));
  StatusCode update_RPC(IOVSVC_CALLBACK_ARGS_P(I,keys));
 
  virtual bool isGoodPanel(const Identifier & Id) const;
  virtual bool isGoodStrip(const Identifier & Id) const;
  
  virtual const std::vector<Identifier>& deadPanelId() const;
  virtual const std::vector<Identifier>& offPanelId() const;

  virtual const std::vector<Identifier>& EffPanelId() const;
  virtual const std::vector<Identifier>& EffStripId() const;
  
  const std::map<Identifier,     double>& RPC_EfficiencyMap(); 
  const std::map<Identifier,     double>& RPC_EfficiencyGapMap(); 
  const std::map<Identifier,     double>& RPC_MeanClusterSizeMap(); 
  const std::map<Identifier,     double>& RPC_FracClusterSize1Map(); 
  const std::map<Identifier,     double>& RPC_FracClusterSize2Map(); 
  const std::map<Identifier,     double>& RPC_FracClusterSize3Map(); 
  const std::map<Identifier,     std::string>& RPC_DeadStripListMap(); 
  const std::map<Identifier,      float>& RPC_FracDeadStripMap(); 
  const std::map<Identifier,        int>& RPC_ProjectedTracksMap();  
  const std::map<Identifier,        int>& RPC_DeadStripList();  
  const std::map<Identifier,     std::vector<double> >& RPC_TimeMapforStrip(); 

  bool m_usesimulation;  

 protected: 
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
  static  std::map<Identifier,     std::vector<double> >& s_RPCCondSummarySvc_RPC_StripTimeMap_empty;  
private:
  
  ServiceHandleArray<IRPCConditionsSvc> m_reportingServices; //!< list of services to be used
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<IRPC_STATUSConditionsSvc> m_rpc_StatusSvc   ;
  ServiceHandle<IRPC_DCSConditionsSvc>    m_rpc_DCSSvc      ;

  bool m_noReports;
  std::vector<Identifier> m_emptyId;
  std::vector<std::string> m_empty;

  //compare method for the binary search
  static bool Compare(const Identifier &a, const Identifier &b) {return (a>b);}
};

inline const InterfaceID & RPCCondSummarySvc::interfaceID(){
  return IRPCConditionsSvc::interfaceID(); 
}

#endif
