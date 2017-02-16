// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: errorcheck.h 299732 2014-03-27 17:41:34Z krasznaa $
#ifndef MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
#define MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
namespace CP{
    enum MuonEfficiencyType{
              Undefined = 0,
              Reco=1, 
              TTVA=2, 
              Iso=3
            };
    inline static std::string EfficiencyTypeName(CP::MuonEfficiencyType M){
        if (M == CP::MuonEfficiencyType::Reco) return "EFF";
        if (M == CP::MuonEfficiencyType::TTVA) return "TTVA";
        if (M == CP::MuonEfficiencyType::Iso) return "ISO";
        return "Unknown";
    }
    inline static CP::MuonEfficiencyType EfficiencyType(const std::string &S){
        if (S == "EFF" || S=="RECO" ) return CP::MuonEfficiencyType::Reco;
        if (S == "TTVA") return CP::MuonEfficiencyType::TTVA;
        if (S == "ISO") return CP::MuonEfficiencyType::Iso;
        return CP::MuonEfficiencyType::Undefined;
    }
    
}
#endif // CPTOOLTESTS_ERRORCHECK_H
