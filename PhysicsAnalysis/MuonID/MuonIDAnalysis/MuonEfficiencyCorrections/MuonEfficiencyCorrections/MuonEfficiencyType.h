// Dear emacs, this is -*- c++ -*-

/*
 Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
    enum MuonEfficiencySystType {
        Nominal = 0,
        Sys1Down = 1,
        Sys1Up = 2,
        Stat1Down = 3,
        Stat1Up = 4,
        LowPtSys1Down = 5,
        LowPtSys1Up = 6,
        LowPtStat1Down = 7,
        LowPtStat1Up = 8
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
    inline static std::string EfficiencySystName(MuonEfficiencySystType S) {
        if (S == MuonEfficiencySystType::Nominal) return "Nominal";
        if (S == MuonEfficiencySystType::Sys1Down) return "Sys1Down";
        if (S == MuonEfficiencySystType::Sys1Up) return "Sys1Up";
        if (S == MuonEfficiencySystType::Stat1Down) return "Stat1Down";
        if (S == MuonEfficiencySystType::Stat1Up) return "Stat1Up";
        if (S == MuonEfficiencySystType::LowPtSys1Down) return "LowPtSys1Down";
        if (S == MuonEfficiencySystType::LowPtSys1Up) return "LowPtSys1Up";
        if (S == MuonEfficiencySystType::LowPtStat1Down) return "LowPtStat1Down";
        if (S == MuonEfficiencySystType::LowPtStat1Up) return "LowPtStat1Up";
        return "Unknown";
    }
    inline static bool IsSystVariation(MuonEfficiencySystType S) {
        if (S == MuonEfficiencySystType::Sys1Down) return true;
        if (S == MuonEfficiencySystType::Sys1Up) return true;
        if (S == MuonEfficiencySystType::LowPtSys1Down) return true;
        if (S == MuonEfficiencySystType::LowPtSys1Up) return true;

        return false;
    }

}

namespace std {

   /// Specialisation of @c std::hash for @c CP::MuonEfficiencyType
   ///
   /// This is necessary to be able to use @c CP::MuonEfficiencyType as the
   /// key in an @c std::unordered_map. Weirdly enough it's only GCC 5.4 that
   /// freaks out without this specialisation though...
   ///
   template<>
   struct hash< CP::MuonEfficiencyType > {
   public:
      size_t operator()( CP::MuonEfficiencyType value ) const {
         static const hash< unsigned int > hasher;
         return hasher( static_cast< unsigned int >( value ) );
      }
   };

   /// Specialisation of @c std::hash for @c CP::MuonEfficiencySystType
   ///
   /// This is necessary to be able to use @c CP::MuonEfficiencySystType as the
   /// key in an @c std::unordered_map. Weirdly enough it's only GCC 5.4 that
   /// freaks out without this specialisation though...
   ///
   template<>
   struct hash< CP::MuonEfficiencySystType > {
   public:
      size_t operator()( CP::MuonEfficiencySystType value ) const {
         static const hash< unsigned int > hasher;
         return hasher( static_cast< unsigned int >( value ) );
      }
   };

} // namespace std

#endif // MUONEFFICIENCYCORRECTIONS_MUONEFFICIENCYTYPE_H
