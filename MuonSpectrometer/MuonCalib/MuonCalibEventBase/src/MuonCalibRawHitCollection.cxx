/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibRawHitCollection.h"

#include <algorithm>
#include <utility>

#include "MuonCalibEventBase/MuonCalibRawCscHit.h"
#include "MuonCalibEventBase/MuonCalibRawMdtHit.h"
#include "MuonCalibEventBase/MuonCalibRawRpcHit.h"
#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {

    MuonCalibRawHitCollection::MuonCalibRawHitCollection(MuonCalibRawMdtHitVec rawMdtHitVec, MuonCalibRawRpcHitVec rawRpcHitVec,
                                                         MuonCalibRawTgcHitVec rawTgcHitVec, MuonCalibRawTgcCoinVec rawTgcCoinVec,
                                                         MuonCalibRawCscHitVec rawCscHitVec) :
        m_rawMdtHitVec{std::move(rawMdtHitVec)},
        m_rawRpcHitVec{std::move(rawRpcHitVec)},
        m_rawTgcHitVec{std::move(rawTgcHitVec)},
        m_rawTgcCoinVec{std::move(rawTgcCoinVec)},
        m_rawCscHitVec{std::move(rawCscHitVec)} {}

    const MuonCalibRawHitCollection::MuonCalibRawMdtHitVec& MuonCalibRawHitCollection::MdtContainer() const { return m_rawMdtHitVec; }
    MuonCalibRawHitCollection::MuonCalibRawMdtHitVec& MuonCalibRawHitCollection::MdtContainer() { return m_rawMdtHitVec; }

    const MuonCalibRawHitCollection::MuonCalibRawRpcHitVec& MuonCalibRawHitCollection::RpcContainer() const { return m_rawRpcHitVec; }
    MuonCalibRawHitCollection::MuonCalibRawRpcHitVec& MuonCalibRawHitCollection::RpcContainer() { return m_rawRpcHitVec; }

    const MuonCalibRawHitCollection::MuonCalibRawTgcHitVec& MuonCalibRawHitCollection::TgcContainer() const { return m_rawTgcHitVec; }
    MuonCalibRawHitCollection::MuonCalibRawTgcHitVec& MuonCalibRawHitCollection::TgcContainer() { return m_rawTgcHitVec; }

    const MuonCalibRawHitCollection::MuonCalibRawCscHitVec& MuonCalibRawHitCollection::CscContainer() const { return m_rawCscHitVec; }
    MuonCalibRawHitCollection::MuonCalibRawCscHitVec& MuonCalibRawHitCollection::CscContainer() { return m_rawCscHitVec; }

    const MuonCalibRawHitCollection::MuonCalibRawTgcCoinVec& MuonCalibRawHitCollection::TgcCoinContainer() const { return m_rawTgcCoinVec; }
    MuonCalibRawHitCollection::MuonCalibRawTgcCoinVec& MuonCalibRawHitCollection::TgcCoinContainer() { return m_rawTgcCoinVec; }

    void MuonCalibRawHitCollection::copy(const MuonCalibRawHitCollection& other) {
        m_rawMdtHitVec.clear();
        m_rawRpcHitVec.clear();
        m_rawTgcHitVec.clear();
        m_rawTgcCoinVec.clear();
        m_rawCscHitVec.clear();

        for (const MdtCalibRawHitPtr& mdt_it : other.m_rawMdtHitVec) { m_rawMdtHitVec.emplace_back(new MuonCalibRawMdtHit(*mdt_it)); }
        for (const RpcCalibRawHitPtr& rpc_it : other.m_rawRpcHitVec) { m_rawRpcHitVec.emplace_back(rpc_it); }
        for (const TgcCalibRawHitPtr& tgc_it : other.m_rawTgcHitVec) { m_rawTgcHitVec.emplace_back(tgc_it); }
        for (const TgcCoinRawPtr& tgcCoin_it : other.m_rawTgcCoinVec) { m_rawTgcCoinVec.emplace_back(tgcCoin_it); }
        for (const CscCalibRawHitPtr& csc_it : other.m_rawCscHitVec) { m_rawCscHitVec.emplace_back(csc_it); }
    }
    MuonCalibRawHitCollection::MuonCalibRawHitCollection(const MuonCalibRawHitCollection& other) { copy(other); }

    MuonCalibRawHitCollection& MuonCalibRawHitCollection::operator=(const MuonCalibRawHitCollection& other) {
        if (this != &other) { copy(other); }
        return (*this);
    }
    int MuonCalibRawHitCollection::numberOfMuonCalibRawMdtHits() const { return m_rawMdtHitVec.size(); }
    int MuonCalibRawHitCollection::numberOfMuonCalibRawRpcHits() const { return m_rawRpcHitVec.size(); }
    int MuonCalibRawHitCollection::numberOfMuonCalibRawTgcHits() const { return m_rawTgcHitVec.size(); }
    int MuonCalibRawHitCollection::numberOfMuonCalibRawTgcCoins() const { return m_rawTgcCoinVec.size(); }
    int MuonCalibRawHitCollection::numberOfMuonCalibRawCscHits() const { return m_rawCscHitVec.size(); }

    void MuonCalibRawHitCollection::addMuonCalibRawHit(MuonCalibRawMdtHit* rawHit) {
        if (rawHit) { m_rawMdtHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(MuonCalibRawRpcHit* rawHit) {
        if (rawHit) { m_rawRpcHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(MuonCalibRawTgcHit* rawHit) {
        if (rawHit) { m_rawTgcHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(MuonCalibRawTgcCoin* rawHit) {
        if (rawHit) { m_rawTgcCoinVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(MuonCalibRawCscHit* rawHit) {
        if (rawHit) { m_rawCscHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(const MdtCalibRawHitPtr& rawHit) {
        if (rawHit) { m_rawMdtHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(const RpcCalibRawHitPtr& rawHit) {
        if (rawHit) { m_rawRpcHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(const TgcCalibRawHitPtr& rawHit) {
        if (rawHit) { m_rawTgcHitVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(const TgcCoinRawPtr& rawHit) {
        if (rawHit) { m_rawTgcCoinVec.emplace_back(rawHit); }
    }
    void MuonCalibRawHitCollection::addMuonCalibRawHit(const CscCalibRawHitPtr& rawHit) {
        if (rawHit) { m_rawCscHitVec.emplace_back(rawHit); }
    }
}  // namespace MuonCalib
