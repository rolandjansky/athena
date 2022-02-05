/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibEventBase/MuonCalibTruthCollection.h"

#include <algorithm>

namespace MuonCalib {

    MuonCalibTruthCollection::MuonCalibTruthCollection(const MuonCalibTruthCollection& other) { copy(other); }
    void MuonCalibTruthCollection::copy(const MuonCalibTruthCollection& other) {
        m_truthVec.clear();
        m_mdtTruthVec.clear();
        m_rpcTruthVec.clear();
        m_cscTruthVec.clear();
        m_tgcTruthVec.clear();

        for (const TruthPtr& it : other.m_truthVec) { m_truthVec.emplace_back(new MuonCalibTruth(*it)); }
        for (const TruthMdtPtr& mdt_it : other.m_mdtTruthVec) { m_mdtTruthVec.emplace_back(new MuonCalibMdtTruthHit(*mdt_it)); }
        for (const TruthRpcPtr& rpc_it : other.m_rpcTruthVec) { m_rpcTruthVec.emplace_back(new MuonCalibRpcTruthHit(*rpc_it)); }
        for (const TruthCscPtr& csc_it : other.m_cscTruthVec) { m_cscTruthVec.emplace_back(new MuonCalibCscTruthHit(*csc_it)); }
        for (const TruthTgcPtr& tgc_it : other.m_tgcTruthVec) { m_tgcTruthVec.emplace_back(new MuonCalibTgcTruthHit(*tgc_it)); }
    }

    MuonCalibTruthCollection::TruthVec& MuonCalibTruthCollection::TruthContainer() { return m_truthVec; }
    const MuonCalibTruthCollection::TruthVec& MuonCalibTruthCollection::TruthContainer() const { return m_truthVec; }

    /// Retrieve a reference to the full Mdt container
    MuonCalibTruthCollection::MdtTruthVec& MuonCalibTruthCollection::MdtContainer() { return m_mdtTruthVec; }
    const MuonCalibTruthCollection::MdtTruthVec& MuonCalibTruthCollection::MdtContainer() const { return m_mdtTruthVec; }

    /// Retrieve a refernece to the full Rpc container
    MuonCalibTruthCollection::RpcTruthVec& MuonCalibTruthCollection::RpcContainer() { return m_rpcTruthVec; }
    const MuonCalibTruthCollection::RpcTruthVec& MuonCalibTruthCollection::RpcContainer() const { return m_rpcTruthVec; }

    /// Retrieve a reference to the full Csc container
    MuonCalibTruthCollection::CscTruthVec& MuonCalibTruthCollection::CscContainer() { return m_cscTruthVec; }
    const MuonCalibTruthCollection::CscTruthVec& MuonCalibTruthCollection::CscContainer() const { return m_cscTruthVec; }

    /// Retrieve a referece to the full Tgc container
    MuonCalibTruthCollection::TgcTruthVec& MuonCalibTruthCollection::TgcContainer() { return m_tgcTruthVec; }
    const MuonCalibTruthCollection::TgcTruthVec& MuonCalibTruthCollection::TgcContainer() const { return m_tgcTruthVec; }

    MuonCalibTruthCollection& MuonCalibTruthCollection::operator=(const MuonCalibTruthCollection& rhs) {
        if (this != &rhs) { copy(rhs); }
        return (*this);
    }
    int MuonCalibTruthCollection::numberOfTruth() const { return m_truthVec.size(); }

    void MuonCalibTruthCollection::addTruth(MuonCalibTruth* truth) {
        if (truth) m_truthVec.emplace_back(truth);
    }
    void MuonCalibTruthCollection::addTruth(MuonCalibMdtTruthHit* truth) {
        if (truth) m_mdtTruthVec.emplace_back(truth);
    }

    void MuonCalibTruthCollection::addTruth(MuonCalibRpcTruthHit* truth) {
        if (truth) m_rpcTruthVec.emplace_back(truth);
    }

    void MuonCalibTruthCollection::addTruth(MuonCalibTgcTruthHit* truth) {
        if (truth) m_tgcTruthVec.emplace_back(truth);
    }

    void MuonCalibTruthCollection::addTruth(MuonCalibCscTruthHit* truth) {
        if (truth) m_cscTruthVec.emplace_back(truth);
    }
    void MuonCalibTruthCollection::addTruth(const TruthPtr& truth) {
        if (truth) m_truthVec.emplace_back(truth);
    }
    void MuonCalibTruthCollection::addTruth(const TruthMdtPtr& truth) {
        if (truth) m_mdtTruthVec.emplace_back(truth);
    }

    void MuonCalibTruthCollection::addTruth(const TruthRpcPtr& truth) {
        if (truth) m_rpcTruthVec.emplace_back(truth);
    }

    void MuonCalibTruthCollection::addTruth(const TruthTgcPtr& truth) {
        if (truth) m_tgcTruthVec.emplace_back(truth);
    }

    void MuonCalibTruthCollection::addTruth(const TruthCscPtr& truth) {
        if (truth) m_cscTruthVec.emplace_back(truth);
    }

}  // namespace MuonCalib
