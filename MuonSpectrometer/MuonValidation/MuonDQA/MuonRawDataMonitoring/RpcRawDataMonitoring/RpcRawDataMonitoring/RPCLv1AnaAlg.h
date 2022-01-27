/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCMONITORDQ_RPCLV1ANAALG_H
#define RPCMONITORDQ_RPCLV1ANAALG_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonRDO/RpcPadContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"

class RPCLv1AnaAlg : public AthMonitorAlgorithm
{

  public:

    RPCLv1AnaAlg(const std::string& name, ISvcLocator* svcLocator);
    virtual ~RPCLv1AnaAlg();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:
    DoubleProperty   m_l1trigMatchWindow{this,"L1TrigMatchingWindow",0.3,"Window size in R for L1 trigger matching"};
    StringProperty   m_packageName{this,"PackageName", "RPCLv1AnaAlg","group name for histograming"};

    SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey
      { this, "MuonContainerKey", "Muons", "Key for Muon Containers" };
    SG::ReadHandleKey<RpcPadContainer> m_rpcPadContainerKey
      { this, "RpcPadContainerKey", "RPCPAD", "Key for Rpc Pad" };
    SG::ReadHandleKey<xAOD::MuonRoIContainer> m_l1RoiContainerKey
      { this, "MuonRoIContainerName", "LVL1MuonRoIs", "Key for L1 ROIs" };

    //
    // Define configurable cuts
    //
    // cuts for muon and roi matching
    Gaudi::Property<float> m_minRoIDR {this, "MinRoIDR", 0.3};
    
    // cuts for the selected muons
    Gaudi::Property<float> m_minPt    {this, "MinPt",     2.0e3};
    Gaudi::Property<float> m_minEta   {this, "MinEta",    0.0};
    Gaudi::Property<float> m_maxEta   {this, "MaxEta",    1.05};
    
    // xAOD::Muon::Quality m_quality;
    Gaudi::Property<int>   m_quality  {this, "MuQuality", 1};

    std::map<std::string,int> m_TriggerThrGroup;

};


#endif
