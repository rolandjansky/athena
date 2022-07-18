
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <AthenaKernel/getMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <MuonTesterTree/EventInfoBranch.h>
#include <StoreGate/ReadCondHandle.h>
EventInfoBranch::EventInfoBranch(MuonTesterTree& tree, bool is_mc) : m_parent{tree}, m_ismc{is_mc} {
    if (!is_mc) {
        tree.disableBranch(m_mcChannel.name());
        tree.disableBranch(m_weight.name());
    }
}

bool EventInfoBranch::fill(const EventContext& ctx) {
    SG::ReadHandle<xAOD::EventInfo> evt_info{m_key, ctx};
    if (!evt_info.isValid()) {
        MsgStream log(Athena::getMessageSvc(), "EventInfoBranch");
        log << MSG::ERROR << "Could not retrieve the EventInfo " << m_key.fullKey() << endmsg;
        return false;
    }

    m_avgIntPerCrossing = evt_info->averageInteractionsPerCrossing();
    m_evtNumber = evt_info->eventNumber();
    m_runNumber = evt_info->runNumber();
    m_lbNumber = evt_info->lumiBlock();
    if (m_ismc) {
        m_mcChannel = evt_info->mcChannelNumber();
        m_weight = evt_info->mcEventWeight(0);
    }
    return true;
}
bool EventInfoBranch::init() { return m_key.initialize().isSuccess();}
std::vector<EventInfoBranch::DataDependency> EventInfoBranch::data_dependencies() { return {&m_key}; }
std::string EventInfoBranch::name() const { return " event info "; }
