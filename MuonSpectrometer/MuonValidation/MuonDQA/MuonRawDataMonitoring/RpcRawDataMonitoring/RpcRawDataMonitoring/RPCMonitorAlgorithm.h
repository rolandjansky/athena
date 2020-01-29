/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RPCMONITORDQ_RPCMONITORALGORITHM_H
#define RPCMONITORDQ_RPCMONITORALGORITHM_H

#include "AthenaMonitoring/AthMonitorAlgorithm.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonRDO/RpcPadContainer.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonSelectorTools/IMuonSelectionTool.h"

class RPCMonitorAlgorithm : public AthMonitorAlgorithm
{

  public:

    RPCMonitorAlgorithm(const std::string& name, ISvcLocator* svcLocator);
    virtual ~RPCMonitorAlgorithm();
    virtual StatusCode initialize() override;
    virtual StatusCode fillHistograms( const EventContext& ctx ) const override;

  private:

    SG::ReadHandleKey<xAOD::MuonContainer> m_MuonContainerKey
      { this, "MuonContainerKey", "Muons", "Key for Muon Containers" };
    SG::ReadHandleKey<RpcPadContainer> m_rpcPadContainerKey
      { this, "RpcPadContainerKey", "RPCPAD", "Key for Rpc Pad" };
    SG::ReadHandleKey<xAOD::MuonRoIContainer> m_l1RoiContainerKey
      { this, "L1RoiContainerKey", "LVL1MuonRoIs", "Key for L1 ROIs" };

    ToolHandle<CP::IMuonSelectionTool> m_muonSelectionTool;

    //
    // Define configurable cuts
    //

    // cuts for muon and roi matching
    float m_minRoIDR;

    // cuts for the selected muons
    float m_minPt;
    float m_minEta;
    float m_maxEta;

    // xAOD::Muon::Quality m_quality;
    int m_quality;
};


#endif
