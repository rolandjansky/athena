/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <MuonPRDTest/PrdTesterModule.h>
#include <StoreGate/ReadCondHandle.h>
namespace MuonPRDTest {
    PrdTesterModule::PrdTesterModule(MuonTesterTree& tree, const std::string& grp_name, bool useCondGeo, MSG::Level msglvl) :
        MuonTesterBranch(tree, " prd module " + grp_name), AthMessaging(grp_name), m_useCondDetMgr{useCondGeo} {
        setLevel(msglvl);
        m_idHelperSvc.retrieve().ignore();
    }
    const Muon::IMuonIdHelperSvc* PrdTesterModule::idHelperSvc() const { return m_idHelperSvc.get(); }
    const MuonGM::MuonDetectorManager* PrdTesterModule::getDetMgr(const EventContext& ctx) const {
        if (!m_useCondDetMgr) return m_detMgr;
        SG::ReadCondHandle<MuonGM::MuonDetectorManager> handle{m_detMgrKey, ctx};
        if (!handle.isValid()) {
            ATH_MSG_ERROR("Failed to retrieve MuonDetectorManager " << m_detMgrKey.fullKey());
            return nullptr;
        }
        return handle.cptr();
    }
    bool PrdTesterModule::init() {
        if (!m_useCondDetMgr && (!m_detStore.retrieve().isSuccess() || !m_detStore->retrieve(m_detMgr).isSuccess())) return false;       
        if (!m_useCondDetMgr) {
            m_detMgrKey = std::string{""};
        }
        if (!declare_dependency(m_detMgrKey)) return false;       
        if (!m_idHelperSvc.retrieve().isSuccess()) return false;
        return declare_keys();
    }
    bool PrdTesterModule::declare_keys() { return true; }
}  // namespace MuonPRDTest