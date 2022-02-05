/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventCnvTool.h"

#include <cassert>
#include <iostream>
#include <vector>

#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/MsgStream.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"

Muon::MuonEventCnvTool::MuonEventCnvTool(const std::string& t, const std::string& n, const IInterface* p) : base_class(t, n, p) {}

StatusCode Muon::MuonEventCnvTool::initialize() {
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_rpcPrdKey.initialize());
    ATH_CHECK(m_tgcPrdKey.initialize());
    ATH_CHECK(m_mdtPrdKey.initialize());
    ATH_CHECK(m_DetectorManagerKey.initialize());
    ATH_CHECK(m_cscPrdKey.initialize(!m_cscPrdKey.empty()));    // check for layouts without CSCs
    ATH_CHECK(m_mmPrdKey.initialize(!m_mmPrdKey.empty()));      // check for layouts without MicroMegas
    ATH_CHECK(m_stgcPrdKey.initialize(!m_stgcPrdKey.empty()));  // check for layouts without STGCs

    return StatusCode::SUCCESS;
}

void Muon::MuonEventCnvTool::checkRoT(const Trk::RIO_OnTrack& rioOnTrack) const {
    MuonConcreteType type = TypeUnknown;
    const Identifier& id = rioOnTrack.identify();
    if (m_idHelperSvc->isRpc(id))
        type = RPC;
    else if (m_idHelperSvc->isTgc(id))
        type = TGC;
    else if (m_idHelperSvc->isMdt(id))
        type = MDT;
    else if (m_idHelperSvc->isMM(id))
        type = MM;
    else if (m_idHelperSvc->issTgc(id))
        type = STGC;
    else if (m_idHelperSvc->isCsc(id))
        type = CSC;

    if (type == TypeUnknown) {
        ATH_MSG_ERROR("Type does not match known concrete type of MuonSpectrometer! Dumping RoT:" << rioOnTrack);
    } else {
        ATH_MSG_VERBOSE("Type = " << type << "(RPC=" << RPC << ", CSC=" << CSC << ", TGC=" << TGC << "MDT=" << MDT << ", STGC=" << STGC
                                  << "MM=" << MM << ")");
    }
}

std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> Muon::MuonEventCnvTool::getLinks(Trk::RIO_OnTrack& rioOnTrack) const {
    using namespace Trk;
    using namespace MuonGM;

    const Identifier& id = rioOnTrack.identify();
    const TrkDetElementBase* detEl = getDetectorElement(id);
    const PrepRawData* prd = m_manuallyFindPRDs ? getLink(id, rioOnTrack.idDE()) : nullptr;

    if (m_fixTGCs && m_idHelperSvc->isTgc(id) && !rioOnTrack.prepRawData()) {
        // Okay, so we might have hit the nasty issue that the TGC EL is broken in some samples
        // Need to fix by pointing to the key defined here (assumung it has been configured correctly for this sample)
        Muon::TgcClusterOnTrack* tgc = dynamic_cast<Muon::TgcClusterOnTrack*>(&rioOnTrack);
        ElementLinkToIDC_TGC_Container& el = tgc->m_rio;
        el.resetWithKeyAndIndex(m_tgcPrdKey.key(), el.index());
    }
    if (!detEl) ATH_MSG_ERROR("Apparently could not find detector element for " << m_idHelperSvc->toString(id));
    ATH_MSG_VERBOSE("Found PRD at : " << prd);
    return std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>(detEl, prd);
}

void Muon::MuonEventCnvTool::prepareRIO_OnTrack(Trk::RIO_OnTrack* RoT) const {
    Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<Muon::MdtDriftCircleOnTrack*>(RoT);
    if (mdt) {
        prepareRIO_OnTrackElementLink<const Muon::MdtPrepDataContainer, Muon::MdtDriftCircleOnTrack>(mdt);
        return;
    }
    Muon::CscClusterOnTrack* csc = dynamic_cast<Muon::CscClusterOnTrack*>(RoT);
    if (csc) {
        prepareRIO_OnTrackElementLink<const Muon::CscPrepDataContainer, Muon::CscClusterOnTrack>(csc);
        return;
    }
    Muon::RpcClusterOnTrack* rpc = dynamic_cast<Muon::RpcClusterOnTrack*>(RoT);
    if (rpc) {
        prepareRIO_OnTrackElementLink<const Muon::RpcPrepDataContainer, Muon::RpcClusterOnTrack>(rpc);
        return;
    }
    Muon::TgcClusterOnTrack* tgc = dynamic_cast<Muon::TgcClusterOnTrack*>(RoT);
    if (tgc) {
        prepareRIO_OnTrackElementLink<const Muon::TgcPrepDataContainer, Muon::TgcClusterOnTrack>(tgc);
        return;
    }
    Muon::sTgcClusterOnTrack* stgc = dynamic_cast<Muon::sTgcClusterOnTrack*>(RoT);
    if (stgc) {
        prepareRIO_OnTrackElementLink<const Muon::sTgcPrepDataContainer, Muon::sTgcClusterOnTrack>(stgc);
        return;
    }
    Muon::MMClusterOnTrack* mm = dynamic_cast<Muon::MMClusterOnTrack*>(RoT);
    if (mm) {
        prepareRIO_OnTrackElementLink<const Muon::MMPrepDataContainer, Muon::MMClusterOnTrack>(mm);
        return;
    }
}

void Muon::MuonEventCnvTool::prepareRIO_OnTrackLink(const Trk::RIO_OnTrack* RoT, ELKey_t& key, ELIndex_t& index) const {
    const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(RoT);
    if (mdt) {
        prepareRIO_OnTrackElementLink<const Muon::MdtPrepDataContainer, Muon::MdtDriftCircleOnTrack>(mdt, key, index);
        return;
    }
    const Muon::CscClusterOnTrack* csc = dynamic_cast<const Muon::CscClusterOnTrack*>(RoT);
    if (csc) {
        prepareRIO_OnTrackElementLink<const Muon::CscPrepDataContainer, Muon::CscClusterOnTrack>(csc, key, index);
        return;
    }
    const Muon::RpcClusterOnTrack* rpc = dynamic_cast<const Muon::RpcClusterOnTrack*>(RoT);
    if (rpc) {
        prepareRIO_OnTrackElementLink<const Muon::RpcPrepDataContainer, Muon::RpcClusterOnTrack>(rpc, key, index);
        return;
    }
    const Muon::TgcClusterOnTrack* tgc = dynamic_cast<const Muon::TgcClusterOnTrack*>(RoT);
    if (tgc) {
        prepareRIO_OnTrackElementLink<const Muon::TgcPrepDataContainer, Muon::TgcClusterOnTrack>(tgc, key, index);
        return;
    }
    const Muon::sTgcClusterOnTrack* stgc = dynamic_cast<const Muon::sTgcClusterOnTrack*>(RoT);
    if (stgc) {
        prepareRIO_OnTrackElementLink<const Muon::sTgcPrepDataContainer, Muon::sTgcClusterOnTrack>(stgc, key, index);
        return;
    }
    const Muon::MMClusterOnTrack* mm = dynamic_cast<const Muon::MMClusterOnTrack*>(RoT);
    if (mm) {
        prepareRIO_OnTrackElementLink<const Muon::MMPrepDataContainer, Muon::MMClusterOnTrack>(mm, key, index);
        return;
    }
    return;
}

void Muon::MuonEventCnvTool::recreateRIO_OnTrack(Trk::RIO_OnTrack* RoT) const {
    std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> pair = getLinks(*RoT);
    if (pair.first) Trk::ITrkEventCnvTool::setRoT_Values(pair, RoT);
}

const Trk::TrkDetElementBase* Muon::MuonEventCnvTool::getDetectorElement(const Identifier& id, const IdentifierHash& /**idHash*/) const {
    return getDetectorElement(id);
}

const Trk::TrkDetElementBase* Muon::MuonEventCnvTool::getDetectorElement(const Identifier& id) const {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle muonMgr{m_DetectorManagerKey, ctx};
    if (!muonMgr.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve the Muon detector manager from the conditions store");
        return nullptr;
    }
    const Trk::TrkDetElementBase* detEl = nullptr;

    // TODO Check that these are in the most likely ordering, for speed. EJWM.
    if (m_idHelperSvc->isRpc(id)) {
        detEl = muonMgr->getRpcReadoutElement(id);
    } else if (m_idHelperSvc->isCsc(id)) {
        detEl = muonMgr->getCscReadoutElement(id);
    } else if (m_idHelperSvc->isTgc(id)) {
        detEl = muonMgr->getTgcReadoutElement(id);
    } else if (m_idHelperSvc->isMdt(id)) {
        detEl = muonMgr->getMdtReadoutElement(id);
    } else if (m_idHelperSvc->issTgc(id)) {
        detEl = muonMgr->getsTgcReadoutElement(id);
    } else if (m_idHelperSvc->isMM(id)) {
        detEl = muonMgr->getMMReadoutElement(id);
    }
    if (!detEl) ATH_MSG_ERROR("Apparently could not find detector element for Identifier: " << m_idHelperSvc->toString(id));
    return detEl;
}

const Trk::PrepRawData* Muon::MuonEventCnvTool::getLink(const Identifier& id, const IdentifierHash& idHash) const {
    const EventContext& ctx = Gaudi::Hive::currentContext();
    if (m_idHelperSvc->isMdt(id)) {
        return getLink(id, idHash, m_mdtPrdKey, ctx);
    } else if (m_idHelperSvc->isRpc(id)) {
        return getLink(id, idHash, m_rpcPrdKey, ctx);
    } else if (m_idHelperSvc->isTgc(id)) {
        return getLink(id, idHash, m_tgcPrdKey, ctx);
    } else if (m_idHelperSvc->isMM(id)) {
        return getLink(id, idHash, m_mmPrdKey, ctx);
    } else if (m_idHelperSvc->issTgc(id)) {
        return getLink(id, idHash, m_stgcPrdKey, ctx);
    }
    ATH_MSG_ERROR("The given Identifier is not a muon one " << m_idHelperSvc->toString(id));
    return nullptr;
}

template <class CONT>
const Trk::PrepRawData* Muon::MuonEventCnvTool::getLink(const Identifier& id, const IdentifierHash& idHash,
                                                        const SG::ReadHandleKey<CONT>& prdKey, const EventContext& ctx) const {
    SG::ReadHandle<CONT> handle{prdKey, ctx};
    if (!handle.isValid()) {
        ATH_MSG_ERROR("PRD container not found at " << prdKey);
        return nullptr;
    } else {
        ATH_MSG_DEBUG("PRD Cluster container found at " << prdKey);
    }

    auto ptr = handle->indexFindPtr(idHash);
    if (!ptr) {
        ATH_MSG_DEBUG("No matching PRD found");
        return nullptr;
    }
    // if we find PRD, then recreate link
    // loop though collection to find matching PRD.
    // there MUST be a faster way to do this!!
    for (const auto& collIt : *ptr) {
        if (collIt->identify() == id) return collIt;
    }
    ATH_MSG_DEBUG("No matching PRD found");
    return nullptr;
}
