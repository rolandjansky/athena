/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONTESTER_MUONIDENTIFIERBRANCH_H
#define MUONTESTER_MUONIDENTIFIERBRANCH_H

#include <MuonIdHelpers/IMuonIdHelperSvc.h>
#include <MuonTesterTree/MuonTesterBranch.h>
#include <MuonTesterTree/VectorBranch.h>

/// Class to dump the basic properties of an Identifier into an n-tuple
///  StationName -> BIS, BIL, etc.
///  StationIndex -> stationName expressed as integer
///  StationEta -> Eta index of the muon station
///  StationPhi -> Phi index of the muon station
class MuonIdentifierBranch : public MuonTesterBranch {
public:
    MuonIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    virtual void push_back(const Identifier& id);

    bool fill(const EventContext&) override final;
    bool init() override final;

protected:
    const Muon::IMuonIdHelperSvc* idHelperSvc() const;

private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{"Muon::MuonIdHelperSvc/MuonIdHelperSvc", name()};
    VectorBranch<std::string>& m_stationName{parent().newVector<std::string>(name() + "_stationName")};
    VectorBranch<uint8_t>& m_stationIndex{parent().newVector<uint8_t>(name() + "_stationIndex")};
    VectorBranch<int8_t>& m_stationEta{parent().newVector<int8_t>(name() + "_stationEta")};
    VectorBranch<uint8_t>& m_stationPhi{parent().newVector<uint8_t>(name() + "_stationPhi")};
};

/// Branch to store all information of the MDT identifier
class MdtIdentifierBranch : public MuonIdentifierBranch {
public:
    MdtIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<uint8_t>& m_multiLayer{parent().newVector<uint8_t>(name() + "_multiLayer")};
    VectorBranch<uint8_t>& m_tubeLayer{parent().newVector<uint8_t>(name() + "_tubeLayer")};
    VectorBranch<uint8_t>& m_tube{parent().newVector<uint8_t>(name() + "_tube")};
};

class RpcIdentifierBranch : public MuonIdentifierBranch {
public:
    RpcIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<uint8_t>& m_gasGap{parent().newVector<uint8_t>(name() + "_gasGap")};
    VectorBranch<uint8_t>& m_doubletR{parent().newVector<uint8_t>(name() + "_doubletR")};
    VectorBranch<uint8_t>& m_doubletZ{parent().newVector<uint8_t>(name() + "_doubletZ")};
    VectorBranch<uint8_t>& m_doubletPhi{parent().newVector<uint8_t>(name() + "_doubletPhi")};
    VectorBranch<uint8_t>& m_strip{parent().newVector<uint8_t>(name() + "_strip")};
    VectorBranch<bool>& m_measuresPhi{parent().newVector<bool>(name() + "_measuresPhi")};
};

/// Branch to store all information of the CscIdentifier
class CscIdentifierBranch : public MuonIdentifierBranch {
public:
    CscIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<uint8_t>& m_chamberLayer{parent().newVector<uint8_t>(name() + "_chamberLayer")};
    VectorBranch<uint8_t>& m_wireLayer{parent().newVector<uint8_t>(name() + "_wireLayer")};
    VectorBranch<bool>& m_measuresPhi{parent().newVector<bool>(name() + "_measuresPhi")};
    VectorBranch<uint8_t>& m_strip{parent().newVector<uint8_t>(name() + "_strip")};
};

/// Branch to store all information of the TgcIdentifier
class TgcIdentifierBranch : public MuonIdentifierBranch {
public:
    TgcIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<uint8_t>& m_gasgap{parent().newVector<uint8_t>(name() + "_GasGap")};
    VectorBranch<bool>& m_measuresPhi{parent().newVector<bool>(name() + "_measuresPhi")};
    VectorBranch<uint8_t>& m_channel{parent().newVector<uint8_t>(name() + "_channel")};
};

/// Branch to store all information of the sTgcIdentifier
class sTgcIdentifierBranch : public MuonIdentifierBranch {
public:
    sTgcIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<uint8_t>& m_gas_gap{parent().newVector<uint8_t>(name() + "_gas_gap")};
    VectorBranch<uint8_t>& m_multiplet{parent().newVector<uint8_t>(name() + "_multiplet")};
    VectorBranch<uint8_t>& m_channel_type{parent().newVector<uint8_t>(name() + "_channel_type")};
    VectorBranch<uint16_t>& m_channel{parent().newVector<uint16_t>(name() + "_channel")};
};

/// Branch to store all information of the MmIdentifier
class MmIdentifierBranch : public MuonIdentifierBranch {
public:
    MmIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<uint8_t>& m_gas_gap{parent().newVector<uint8_t>(name() + "_gas_gap")};
    VectorBranch<uint8_t>& m_multiplet{parent().newVector<uint8_t>(name() + "_multiplet")};
    VectorBranch<uint16_t>& m_channel{parent().newVector<uint16_t>(name() + "_channel")};
};

#endif
