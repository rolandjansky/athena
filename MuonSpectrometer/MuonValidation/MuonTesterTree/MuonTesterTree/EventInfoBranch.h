
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTERTREE_EVENTINFOBRANCH_H
#define MUONTESTERTREE_EVENTINFOBRANCH_H
#include <MuonTesterTree/ScalarBranch.h>
#include <StoreGate/ReadHandleKey.h>
#include <xAODEventInfo/EventInfo.h>

class EventInfoBranch : public IMuonTesterBranch {
public:
    EventInfoBranch(MuonTesterTree& tree, bool is_mc);

    bool fill(const EventContext& ctx) override final;
    bool init() override final;
    std::string name() const override final;

    std::vector<DataDependency> data_dependencies() override final;

private:
    SG::ReadHandleKey<xAOD::EventInfo> m_key{"EventInfo"};
    MuonTesterTree& m_parent;
    bool m_ismc{false};
    ScalarBranch<int>& m_avgIntPerCrossing{m_parent.newScalar<int>("avgIntPerCrossing")};
    ScalarBranch<unsigned long long>& m_evtNumber{m_parent.newScalar<unsigned long long>("evtNumber")};
    ScalarBranch<uint32_t>& m_runNumber{m_parent.newScalar<uint32_t>("runNumber")};
    ScalarBranch<uint32_t>& m_lbNumber{m_parent.newScalar<uint32_t>("lbNumber")};

    ScalarBranch<uint32_t>& m_mcChannel{m_parent.newScalar<uint32_t>("mcChanneNumber")};
    ScalarBranch<double>& m_weight{m_parent.newScalar<double>("evtWeight")};
};
#endif