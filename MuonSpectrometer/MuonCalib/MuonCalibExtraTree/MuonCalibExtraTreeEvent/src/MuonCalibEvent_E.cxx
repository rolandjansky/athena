/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibExtraTreeEvent/MuonCalibEvent_E.h"

#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibStl/DeleteObject.h"

namespace MuonCalib {
    MuonCalibEvent_E::MuonCalibEvent_E(const MuonCalibEvent& event) : MuonCalibEvent(event) {}

    MuonCalibEvent_E::MuonCalibEvent_E(const MuonCalibEvent_E& event) : MuonCalibEvent(event) { copy(event); }

    void MuonCalibEvent_E::copy(const MuonCalibEvent_E& event) {
        m_phiPats.clear();
        m_tracks.clear();
        m_ctp.clear();
        m_mctpi.clear();
        m_rpcsl.clear();
        m_rpcco.clear();
        m_rpctr.clear();
        m_mbts.clear();
        m_l1calo.clear();

        for (const MuonCalibTrkPtr& phi : event.phiPat()) { m_phiPats.emplace_back(new MuonCalibTrack_E(*phi)); }
        for (const MuonCalibTrkPtr& trk : event.tracks()) { m_tracks.emplace_back(new MuonCalibTrack_E(*trk)); }
        for (const TrigInfoPtr& ctpi : event.ctp()) { m_ctp.emplace_back(new MuonCalibTriggerInfo(*ctpi)); }
        for (const MuCTPiPtr& mit : event.MUCTPI()) { m_mctpi.emplace_back(new MuonCalibMUCTPI(*mit)); }
        for (const RpcSectorLogicPtr& logic : event.RpcSL()) { m_rpcsl.emplace_back(new MuonCalibRpcSectorLogic(*logic)); }
        for (const RpcCoinPtr& coin : event.RpcCoin()) { m_rpcco.emplace_back(new MuonCalibRawRpcCoin(*coin)); }
        for (const RpcTrigHitPtr& trig_hit : event.rawRpcTrig()) { m_rpctr.emplace_back(new MuonCalibRawRpcTriggerHit(*trig_hit)); }
        for (const CaloHitPtr& calo_hit : event.MBTS()) { m_mbts.emplace_back(new MuonCalibCaloHit(*calo_hit)); }
        for (const CaloHitPtr& calo_hit : event.L1Calo()) { m_l1calo.emplace_back(new MuonCalibCaloHit(*calo_hit)); }
    }

    MuonCalibEvent_E& MuonCalibEvent_E::operator=(const MuonCalibEvent_E& event) {
        if (this != &event) { copy(event); }
        return (*this);
    }
    const MuonCalibEvent_E::PhiPatVec& MuonCalibEvent_E::phiPat() const { return m_phiPats; }
    MuonCalibEvent_E::PhiPatVec& MuonCalibEvent_E::phiPat() { return m_phiPats; }
    /// Accesses the tracks of the event directly
    const MuonCalibEvent_E::TrackVec& MuonCalibEvent_E::tracks() const { return m_tracks; }
    MuonCalibEvent_E::TrackVec& MuonCalibEvent_E::tracks() { return m_tracks; }

    /// Access to the Ctp triggerinfo hits / bits
    const MuonCalibEvent_E::CtpVec& MuonCalibEvent_E::ctp() const { return m_ctp; }
    MuonCalibEvent_E::CtpVec& MuonCalibEvent_E::ctp() { return m_ctp; }

    //!< retrieve an iterator of the first MUCTPI triggerinfo bit
    const MuonCalibEvent_E::MUCTPIVec& MuonCalibEvent_E::MUCTPI() const { return m_mctpi; }
    MuonCalibEvent_E::MUCTPIVec& MuonCalibEvent_E::MUCTPI() { return m_mctpi; }

    const MuonCalibEvent_E::RpcSLVec& MuonCalibEvent_E::RpcSL() const { return m_rpcsl; }
    MuonCalibEvent_E::RpcSLVec& MuonCalibEvent_E::RpcSL() { return m_rpcsl; }

    MuonCalibEvent_E::RpcCoinVec& MuonCalibEvent_E::RpcCoin() { return m_rpcco; }
    const MuonCalibEvent_E::RpcCoinVec& MuonCalibEvent_E::RpcCoin() const { return m_rpcco; }

    const MuonCalibEvent_E::RawRpcTrigVec& MuonCalibEvent_E::rawRpcTrig() const { return m_rpctr; }
    MuonCalibEvent_E::RawRpcTrigVec& MuonCalibEvent_E::rawRpcTrig() { return m_rpctr; }

    const MuonCalibEvent_E::MBTSVec& MuonCalibEvent_E::MBTS() const { return m_mbts; }
    MuonCalibEvent_E::MBTSVec& MuonCalibEvent_E::MBTS() { return m_mbts; }
    const MuonCalibEvent_E::L1CaloVec& MuonCalibEvent_E::L1Calo() const { return m_l1calo; }
    MuonCalibEvent_E::L1CaloVec& MuonCalibEvent_E::L1Calo() { return m_l1calo; }

    unsigned int MuonCalibEvent_E::nrPhiPat() const { return m_phiPats.size(); }
    unsigned int MuonCalibEvent_E::nrTracks() const { return m_tracks.size(); }
    unsigned int MuonCalibEvent_E::nrCtp() const { return m_ctp.size(); }
    unsigned int MuonCalibEvent_E::nrMUCTPI() const { return m_mctpi.size(); }
    unsigned int MuonCalibEvent_E::nrRpcSL() const { return m_rpcsl.size(); }
    unsigned int MuonCalibEvent_E::nrRpcCoin() const { return m_rpcco.size(); }
    unsigned int MuonCalibEvent_E::nrRawRpcTrigHit() const { return m_rpctr.size(); }
    unsigned int MuonCalibEvent_E::nrMBTS() const { return m_mbts.size(); }
    unsigned int MuonCalibEvent_E::nrL1Calo() const { return m_l1calo.size(); }

    void MuonCalibEvent_E::addPattern(const MuonCalibTrkPtr& pat) {
        if (pat) { m_phiPats.emplace_back(pat); }
    }

    void MuonCalibEvent_E::addTrack(const MuonCalibTrkPtr& trk) {
        if (trk) { m_tracks.emplace_back(trk); }
    }

    void MuonCalibEvent_E::addCtp(const TrigInfoPtr& trig) {
        if (trig) { m_ctp.emplace_back(trig); }
    }

    void MuonCalibEvent_E::addMUCTPI(const MuCTPiPtr& mctpi) {
        if (mctpi) { m_mctpi.emplace_back(mctpi); }
    }

    void MuonCalibEvent_E::addRpcSL(const RpcSectorLogicPtr& rpcsl) {
        if (rpcsl) { m_rpcsl.emplace_back(rpcsl); }
    }

    void MuonCalibEvent_E::addRpcCoin(const RpcCoinPtr& rpcco) {
        if (rpcco) { m_rpcco.emplace_back(rpcco); }
    }

    void MuonCalibEvent_E::addRawRpcTrigHit(const RpcTrigHitPtr& rpctr) {
        if (rpctr) { m_rpctr.emplace_back(rpctr); }
    }

    void MuonCalibEvent_E::addMBTS(const CaloHitPtr& hit) {
        if (hit) { m_mbts.emplace_back(hit); }
    }

    void MuonCalibEvent_E::addL1Calo(const CaloHitPtr& hit) {
        if (hit) { m_l1calo.emplace_back(hit); }
    }

    void MuonCalibEvent_E::addPattern(MuonCalibTrack_E* pat) {
        if (pat) { m_phiPats.emplace_back(pat); }
    }

    void MuonCalibEvent_E::addTrack(MuonCalibTrack_E* trk) {
        if (trk) { m_tracks.emplace_back(trk); }
    }

    void MuonCalibEvent_E::addCtp(MuonCalibTriggerInfo* trig) {
        if (trig) { m_ctp.emplace_back(trig); }
    }

    void MuonCalibEvent_E::addMUCTPI(MuonCalibMUCTPI* mctpi) {
        if (mctpi) { m_mctpi.emplace_back(mctpi); }
    }

    void MuonCalibEvent_E::addRpcSL(MuonCalibRpcSectorLogic* rpcsl) {
        if (rpcsl) { m_rpcsl.emplace_back(rpcsl); }
    }

    void MuonCalibEvent_E::addRpcCoin(MuonCalibRawRpcCoin* rpcco) {
        if (rpcco) { m_rpcco.emplace_back(rpcco); }
    }

    void MuonCalibEvent_E::addRawRpcTrigHit(MuonCalibRawRpcTriggerHit* rpctr) {
        if (rpctr) { m_rpctr.emplace_back(rpctr); }
    }

    void MuonCalibEvent_E::addMBTS(MuonCalibCaloHit* hit) {
        if (hit) { m_mbts.emplace_back(hit); }
    }

    void MuonCalibEvent_E::addL1Calo(MuonCalibCaloHit* hit) {
        if (hit) { m_l1calo.emplace_back(hit); }
    }

}  // namespace MuonCalib
