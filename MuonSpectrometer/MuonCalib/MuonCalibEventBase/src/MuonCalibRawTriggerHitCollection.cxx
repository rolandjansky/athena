/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawTriggerHitCollection.h"

#include <algorithm>

#include "MuonCalibEventBase/MuonCalibRawRpcTriggerHit.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

    MuonCalibRawTriggerHitCollection::MuonCalibRawTriggerHitCollection() {
        MuonCalibRawRpcTriggerHitVec newRpcTrigger(0);
        m_rawRpcTriggerHitVec = newRpcTrigger;
    }

    MuonCalibRawTriggerHitCollection::~MuonCalibRawTriggerHitCollection() {
        std::for_each(rawRpcTriggerHitCollectionBegin(), rawRpcTriggerHitCollectionEnd(), DeleteObject());
        m_rawRpcTriggerHitVec.clear();
    }

    MuonCalibRawTriggerHitCollection::MuonCalibRawTriggerHitCollection(
        const MuonCalibRawTriggerHitCollection &muonRawTriggerHitCollection) {
        MuonCalibRawRpcTriggerHitVecCit rpc_it = muonRawTriggerHitCollection.rawRpcTriggerHitCollectionBegin();
        MuonCalibRawRpcTriggerHitVecCit rpc_it_end = muonRawTriggerHitCollection.rawRpcTriggerHitCollectionEnd();
        for (; rpc_it != rpc_it_end; ++rpc_it) { m_rawRpcTriggerHitVec.push_back(new MuonCalibRawRpcTriggerHit(**rpc_it)); }
    }

    MuonCalibRawTriggerHitCollection &MuonCalibRawTriggerHitCollection::operator=(const MuonCalibRawTriggerHitCollection &rhs) {
        if (this != &rhs) {
            MuonCalibRawRpcTriggerHitVec temp_rawRpcTriggerHitVec;
            MuonCalibRawRpcTriggerHitVecCit rpc_it = rhs.rawRpcTriggerHitCollectionBegin();
            MuonCalibRawRpcTriggerHitVecCit rpc_it_end = rhs.rawRpcTriggerHitCollectionEnd();
            for (; rpc_it != rpc_it_end; ++rpc_it) { temp_rawRpcTriggerHitVec.push_back(new MuonCalibRawRpcTriggerHit(**rpc_it)); }
            m_rawRpcTriggerHitVec = temp_rawRpcTriggerHitVec;
        }
        return (*this);
    }

}  // namespace MuonCalib
