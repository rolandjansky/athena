/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawTriggerHitCollection.h"

#include <algorithm>

#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {
    MuonCalibRawTriggerHitCollection::MuonCalibRawTriggerHitCollection(MuonCalibRawRpcTriggerHitVec rawRpcTriggerHitVec) :
        m_rawRpcTriggerHitVec{std::move(rawRpcTriggerHitVec)} {}
    MuonCalibRawTriggerHitCollection::MuonCalibRawTriggerHitCollection(const MuonCalibRawTriggerHitCollection &other) { copy(other); }
    void MuonCalibRawTriggerHitCollection::copy(const MuonCalibRawTriggerHitCollection &other) {
        m_rawRpcTriggerHitVec.clear();
        for (const CalibRawRpcTriggerHitPtr &rpc_it : other.get()) { addMuonCalibRawTriggerHit(rpc_it); }
    }

    MuonCalibRawTriggerHitCollection &MuonCalibRawTriggerHitCollection::operator=(const MuonCalibRawTriggerHitCollection &rhs) {
        if (this != &rhs) { copy(rhs); }
        return (*this);
    }
    int MuonCalibRawTriggerHitCollection::numberOfMuonCalibRawRpcTriggerHits() const { return m_rawRpcTriggerHitVec.size(); }

    void MuonCalibRawTriggerHitCollection::addMuonCalibRawTriggerHit(const CalibRawRpcTriggerHitPtr& rawTriggerHit) {
        if (rawTriggerHit) {
            m_rawRpcTriggerHitVec.emplace_back(rawTriggerHit);
        } else {
            MsgStream log(Athena::getMessageSvc(), "MuonCalibRawTriggerHitCollection");
            log << MSG::WARNING << "Refuse to put an empty MuonCalibRawRpcTriggerHit in MuonCalibRawTriggerHitCollection" << endmsg;
        }
    }
    const MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVec &MuonCalibRawTriggerHitCollection::get() const {
        return m_rawRpcTriggerHitVec;
    }
    MuonCalibRawTriggerHitCollection::MuonCalibRawRpcTriggerHitVec &MuonCalibRawTriggerHitCollection::get() {
        return m_rawRpcTriggerHitVec;
    }

}  // namespace MuonCalib
