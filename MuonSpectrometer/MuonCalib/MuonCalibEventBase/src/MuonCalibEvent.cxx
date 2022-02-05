/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibEvent.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "MuonCalibEventBase/MuonCalibPattern.h"
#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"
#include "MuonCalibEventBase/MuonCalibRawTriggerHitCollection.h"
#include "MuonCalibEventBase/MuonCalibTriggerTimeInfo.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {
    MuonCalibEvent::~MuonCalibEvent() = default;
    MuonCalibEvent::MuonCalibEvent(MuonCalibEvent::MCPVec patvec) : m_patternVec{std::move(patvec)} {}

    MuonCalibEvent::MuonCalibEvent(MuonCalibEvent::MCPVec patvec, std::shared_ptr<const MuonCalibRawHitCollection> Coll) :
        m_patternVec{std::move(patvec)}, m_rawColl{std::move(Coll)} {}

    MuonCalibEvent::MuonCalibEvent(MuonCalibEvent::MCPVec patvec, std::shared_ptr<MuonCalibEventInfo> eventInfo,
                                   std::shared_ptr<const MuonCalibRawHitCollection> Coll) :
        m_patternVec{std::move(patvec)}, m_eventInfo(std::move(eventInfo)), m_rawColl(std::move(Coll)) {}

    MuonCalibEvent::MCPVec &MuonCalibEvent::pattern() { return m_patternVec; }
    const MuonCalibEvent::MCPVec &MuonCalibEvent::pattern() const { return m_patternVec; }

    const MuonCalibRawHitCollection *MuonCalibEvent::rawHitCollection() const { return m_rawColl.get(); }
    const MuonCalibRawTriggerHitCollection *MuonCalibEvent::rawTriggerHitCollection() const { return m_rawTriggerColl.get(); }
    const MuonCalibTruthCollection *MuonCalibEvent::calibTruthCollection() const { return m_truthColl.get(); }
    const MuonCalibEventInfo &MuonCalibEvent::eventInfo() const { return *m_eventInfo; }
    const RpcSectorLogicContainer *MuonCalibEvent::rpcSectorLogicContainer() const { return m_rpcSlLogicContainer.get(); }
    const MuonCalibTriggerTimeInfo *MuonCalibEvent::triggerTimeInfo() const { return m_triggerTimeInfo.get(); }

    void MuonCalibEvent::setMuonCalibEventInfo(std::shared_ptr<MuonCalibEventInfo> eventInfo) { m_eventInfo = std::move(eventInfo); }
    void MuonCalibEvent::setRpcSectorLogicContainer(std::shared_ptr<const RpcSectorLogicContainer> rpcSlContainer) {
        m_rpcSlLogicContainer = std::move(rpcSlContainer);
    }

    int MuonCalibEvent::numberOfPatterns() const { return m_patternVec.size(); }

    int MuonCalibEvent::numberOfSegments() const {
        int segs = 0;
        for (const PatternPtr &pat : pattern()) segs += pat->muonSegments();

        return segs;
    }

    int MuonCalibEvent::numberOfHits() const {
        int hits = 0;
        for (const PatternPtr &pat : pattern()) {
            hits += pat->nmdtHits() % 1000;
            hits += (pat->nmdtHits() - (pat->nmdtHits() % 1000)) / 1000;
            hits += pat->nrpcHits() % 1000;
            hits += (pat->nrpcHits() - (pat->nrpcHits() % 1000)) / 1000;
            hits += pat->ntgcHits() % 1000;
            hits += (pat->ntgcHits() - (pat->ntgcHits() % 1000)) / 1000;
            hits += pat->ncscHits() % 1000;
            hits += (pat->ncscHits() - (pat->ncscHits() % 1000)) / 1000;
        }
        return hits;
    }
    void MuonCalibEvent::setMuonCalibTriggerTimeInfo(std::shared_ptr<const MuonCalibTriggerTimeInfo> triggerTimeInfo) {
        m_triggerTimeInfo = std::move(triggerTimeInfo);
    }
    void MuonCalibEvent::setMuonCalibTruthCollection(std::shared_ptr<const MuonCalibTruthCollection> MCTColl) {
        m_truthColl = std::move(MCTColl);
    }
    void MuonCalibEvent::setMuonCalibRawHitCollection(std::shared_ptr<const MuonCalibRawHitCollection> RHColl) {
        m_rawColl = std::move(RHColl);
    }
    void MuonCalibEvent::setMuonCalibRawTriggerHitCollection(std::shared_ptr<const MuonCalibRawTriggerHitCollection> THColl) {
        m_rawTriggerColl = std::move(THColl);
    }
    void MuonCalibEvent::setMuonCalibPatternVec(MuonCalibEvent::MCPVec mcpvec) { m_patternVec = std::move(mcpvec); }

}  // namespace MuonCalib
