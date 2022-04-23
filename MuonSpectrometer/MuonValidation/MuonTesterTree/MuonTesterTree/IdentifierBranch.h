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
    MuonIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name, const Muon::IMuonIdHelperSvc* idHelperSvc);

    virtual void push_back(const Identifier& id);

    bool fill(const EventContext&) override final;
    bool init() override final;

protected:
    const Muon::IMuonIdHelperSvc* idHelperSvc() const;

private:
    const Muon::IMuonIdHelperSvc* m_idHelperSvc{nullptr};
    VectorBranch<std::string>& m_stationName{parent().newVector<std::string>(name() + "_stationName")};
    VectorBranch<int>& m_stationIndex{parent().newVector<int>(name() + "_stationIndex")};
    VectorBranch<int>& m_stationEta{parent().newVector<int>(name() + "_stationEta")};
    VectorBranch<int>& m_stationPhi{parent().newVector<int>(name() + "_stationPhi")};
};

/// Branch to store all information of the MdtIdentifier
class MdtIdentifierBranch : public MuonIdentifierBranch {
public:
    MdtIdentifierBranch(MuonTesterTree& tree, const std::string& grp_name, const Muon::IMuonIdHelperSvc* idHelperSvc);

    void push_back(const Identifier& id) override final;

private:
    VectorBranch<int>& m_multiLayer{parent().newVector<int>(name() + "_multiLayer")};
    VectorBranch<int>& m_tubeLayer{parent().newVector<int>(name() + "_tubeLayer")};
    VectorBranch<int>& m_tube{parent().newVector<int>(name() + "_tube")};
};

#endif
