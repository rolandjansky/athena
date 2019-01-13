/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */

#ifndef MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
#define MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H

#include <string>
#include <functional>

namespace CP {
    enum MuonEfficiencyType {
        Undefined = 0,
        Reco = 1,
        TTVA = 2,
        Iso = 3,
        BadMuonVeto = 4
    };
    inline static std::string EfficiencyTypeName(CP::MuonEfficiencyType M) {
        if (M == CP::MuonEfficiencyType::Reco) return "RECO";
        if (M == CP::MuonEfficiencyType::TTVA) return "TTVA";
        if (M == CP::MuonEfficiencyType::Iso) return "ISO";
        if (M == CP::MuonEfficiencyType::BadMuonVeto) return "BADMUON";
        return "Unknown";
    }
    inline static CP::MuonEfficiencyType EfficiencyType(const std::string &S) {
        if (S == "RECO") return CP::MuonEfficiencyType::Reco;
        if (S == "TTVA") return CP::MuonEfficiencyType::TTVA;
        if (S == "ISO") return CP::MuonEfficiencyType::Iso;
        if (S == "BADMUON") return CP::MuonEfficiencyType::BadMuonVeto;
        return CP::MuonEfficiencyType::Undefined;
    }
}

#endif // MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
