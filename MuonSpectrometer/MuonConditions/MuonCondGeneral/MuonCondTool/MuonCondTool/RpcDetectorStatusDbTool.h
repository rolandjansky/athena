/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDTOOL_RPCDETECTORDBTOOL_H
#define MUONCONDTOOL_RPCDETECTORDBTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "MuonCondInterface/IRpcDetectorStatusDbTool.h" 
//#include "MuonCondData/Rpc_ConditionDataContainer.h"
//#include "MuonCondData/RpcConditionPar.h"
#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
class Identifier; 
class StoreGateSvc; 

class RpcIdHelper;
class IIOVSvc;
class StatusCode;


class RpcDetectorStatusDbTool: public AthAlgTool,
	             virtual public IRpcDetectorStatusDbTool
{

public:    

  RpcDetectorStatusDbTool(const std::string& type, 
		const std::string& name, 
		const IInterface* parent); 

  /** required by the IAddressProvider interface */
  //  virtual StatusCode updateAddress(SG::TransientAddress* tad);
  virtual StatusCode updateAddress(StoreID::type storeID,
                                   SG::TransientAddress* tad,
                                   const EventContext& ctx) override;

private: 
  bool m_FirstCall ;
  virtual StatusCode initialize() override;
  
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS) override;
  virtual StatusCode loadRpcDqStatus(IOVSVC_CALLBACK_ARGS) override;
  virtual std::string FolderName() const  override {return m_FolderName;}
  StoreGateSvc* m_detStore;
  IIOVSvc* m_IOVSvc;

  virtual const std::vector<Identifier>&  EffPanelId() const override { return  m_effPanelId;}
  virtual const std::vector<Identifier>&  EffStripId() const override { return  m_effStripId;}

  virtual const std::map<Identifier,     std::vector<double> >& RPC_TimeMapforStrip() const override { return m_RPC_StripTimeMap;}

  virtual const std::map<Identifier,     double>& RPC_EfficiencyMap      () const override { return m_RPC_PanelEfficiencyMap      ;}
  virtual const std::map<Identifier,     double>& RPC_EfficiencyGapMap   () const override { return m_RPC_GapEfficiencyMap        ;}
  virtual const std::map<Identifier,     double>& RPC_MeanClusterSizeMap () const override { return m_RPC_PanelMeanClusterSizeMap ;}
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize1Map() const override { return m_RPC_PanelFracClusterSize1Map;}
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize2Map() const override { return m_RPC_PanelFracClusterSize2Map;}
  virtual const std::map<Identifier,     double>& RPC_FracClusterSize3Map() const override { return m_RPC_PanelFracClusterSize3Map;}
  virtual const std::map<Identifier,std::string>& RPC_DeadStripListMap   () const override { return m_RPC_PanelDeadStripListMap   ;}
  virtual const std::map<Identifier,      float>& RPC_FracDeadStripMap   () const override { return m_RPC_PanelFracDeadStripMap   ;}
  virtual const std::map<Identifier,        int>& RPC_ProjectedTracksMap () const override { return m_RPC_PanelProjectedTracksMap ;}
  virtual const std::map<Identifier,        int>& RPC_DeadStripList      () const override { return m_RPC_PanelDeadStripList      ;}

  const RpcIdHelper* m_rpcIdHelper;
  
  //mutable Rpc_ConditionDataContainer * m_rpcDqStatusData;

  double  m_RPC_PanelEfficiency;
  std::string      m_FolderName;
  std::string      m_rpcDqFolder;
  std::string      m_rpcDqStatusDataLocation;
  std::vector<Identifier> m_effPanelId;
  std::vector<Identifier> m_effStripId;
  std::map<Identifier ,     double> m_RPC_PanelEfficiencyMap      ;
  std::map<Identifier ,     double> m_RPC_GapEfficiencyMap        ;
  std::map<Identifier ,     double> m_RPC_PanelMeanClusterSizeMap ;
  std::map<Identifier ,     double> m_RPC_PanelFracClusterSize1Map;
  std::map<Identifier ,     double> m_RPC_PanelFracClusterSize2Map;
  std::map<Identifier ,     double> m_RPC_PanelFracClusterSize3Map;
  std::map<Identifier ,std::string> m_RPC_PanelDeadStripListMap   ;
  std::map<Identifier ,      float> m_RPC_PanelFracDeadStripMap   ;
  std::map<Identifier ,        int> m_RPC_PanelProjectedTracksMap ;
  std::map<Identifier ,        int> m_RPC_PanelDeadStripList      ;
  std::map<Identifier ,     std::vector<double> > m_RPC_StripTimeMap;

  MsgStream m_log;
  bool      m_debug;
  bool      m_verbose;

};


#endif
