/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include <MuonTesterTree/IdentifierBranch.h>

MuonIdentifierBranch::MuonIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name) : MuonTesterBranch(tree, grp_name) {}

bool MuonIdentifierBranch::fill(const EventContext&) { return true; }
bool MuonIdentifierBranch::init() { return m_idHelperSvc.retrieve().isSuccess(); }
const Muon::IMuonIdHelperSvc* MuonIdentifierBranch::idHelperSvc() const { return m_idHelperSvc.get(); }

void MuonIdentifierBranch::push_back(const Identifier& id) {
    const int st_idx = m_idHelperSvc->stationName(id);
    m_stationIndex.push_back(st_idx);
    if (m_stationName.initialized()) {
        if (m_idHelperSvc->isMdt(id))
            m_stationName.push_back(m_idHelperSvc->mdtIdHelper().stationNameString(st_idx));
        else if (m_idHelperSvc->isRpc(id))
            m_stationName.push_back(m_idHelperSvc->rpcIdHelper().stationNameString(st_idx));
        else if (m_idHelperSvc->isTgc(id))
            m_stationName.push_back(m_idHelperSvc->tgcIdHelper().stationNameString(st_idx));
        else if (m_idHelperSvc->isCsc(id))
            m_stationName.push_back(m_idHelperSvc->cscIdHelper().stationNameString(st_idx));

        else if (m_idHelperSvc->isMM(id))
            m_stationName.push_back(m_idHelperSvc->mmIdHelper().stationNameString(st_idx));
        else if (m_idHelperSvc->issTgc(id))
            m_stationName.push_back(m_idHelperSvc->stgcIdHelper().stationNameString(st_idx));
    }
    m_stationEta.push_back(m_idHelperSvc->stationEta(id));
    m_stationPhi.push_back(m_idHelperSvc->stationPhi(id));
}
///###############################################################
///                 MdtIdentifierBranch
///###############################################################
MdtIdentifierBranch::MdtIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name) : MuonIdentifierBranch(tree, grp_name) {}

void MdtIdentifierBranch::push_back(const Identifier& id) {
    MuonIdentifierBranch::push_back(id);
    m_multiLayer.push_back(idHelperSvc()->mdtIdHelper().multilayer(id));
    m_tubeLayer.push_back(idHelperSvc()->mdtIdHelper().tubeLayer(id));
    m_tube.push_back(idHelperSvc()->mdtIdHelper().tube(id));
}
///###############################################################
///                 RpcIdentifierBranch
///###############################################################
RpcIdentifierBranch::RpcIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name) : MuonIdentifierBranch(tree, grp_name) {}

void RpcIdentifierBranch::push_back(const Identifier& id) {
    MuonIdentifierBranch::push_back(id);
    m_gasGap.push_back(idHelperSvc()->rpcIdHelper().gasGap(id));
    m_doubletR.push_back(idHelperSvc()->rpcIdHelper().doubletR(id));
    m_doubletZ.push_back(idHelperSvc()->rpcIdHelper().doubletZ(id));
    m_doubletPhi.push_back(idHelperSvc()->rpcIdHelper().doubletPhi(id));
    m_measuresPhi.push_back(idHelperSvc()->rpcIdHelper().measuresPhi(id));
    m_strip.push_back(idHelperSvc()->rpcIdHelper().strip(id));
}
