/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONHISTUTILS_MUONENUMDEFS_H
#define MUONHISTUTILS_MUONENUMDEFS_H

#include "xAODMuon/Muon.h"

namespace Muon {
  namespace EnumDefs {

    enum DetRegion { GLOBAL,BA,BC,EA,EC,nDetRegions };
    static inline const char* toString(Muon::EnumDefs::DetRegion reg) {
      switch (reg) {
      case Muon::EnumDefs::GLOBAL: return "Global";
      case Muon::EnumDefs::BA: return "BA";
      case Muon::EnumDefs::BC: return "BC";
      case Muon::EnumDefs::EA: return "EA";
      case Muon::EnumDefs::EC: return "EC";
      default: return "UnknownDetRegion";
      }
    }
      
    static inline unsigned int nMuonTypes( ) {return 5;}
    static inline const char* toString(xAOD::Muon::MuonType type) {
      switch (type) {
      case xAOD::Muon::Combined: return "Combined";
      case xAOD::Muon::MuonStandAlone: return "StandAlone";
      case xAOD::Muon::SegmentTagged: return "SegmentTagged";
      case xAOD::Muon::CaloTagged: return "CaloTagged";
      case xAOD::Muon::SiliconAssociatedForwardMuon: return "SiForward";
      default: return "UnknownType";
      }
    }

    static inline unsigned int nMuonAuthors( ) {return xAOD::Muon::NumberOfMuonAuthors;}
    static inline const char* toString(xAOD::Muon::Author author) {
      switch (author) {
      case xAOD::Muon::unknown: return "unknown";
      case xAOD::Muon::MuidCo: return "MuidCo";
      case xAOD::Muon::STACO: return "STACO";
      case xAOD::Muon::MuTag: return "MuTag";
      case xAOD::Muon::MuTagIMO: return "MuTagIMO";
      case xAOD::Muon::MuidSA: return "MuidSA";
      case xAOD::Muon::MuGirl: return "MuGirl";
      case xAOD::Muon::MuGirlLowBeta: return "MuGirlLowBeta";
      case xAOD::Muon::CaloTag: return "CaloTag";
      case xAOD::Muon::CaloLikelihood: return "CaloLikelihood";
      case xAOD::Muon::ExtrapolateMuonToIP: return "ExtrMuonToIP";
      case xAOD::Muon::CaloScore: return "CaloScore";
      default: return "UnknownAuthor";
      }
    }

    static inline unsigned int nMuonQualities() {return 4;}
    static inline const char* toString(xAOD::Muon::Quality quality) {
      switch (quality) {
      case xAOD::Muon::Tight: return "Tight";
      case xAOD::Muon::Medium: return "Medium";
      case xAOD::Muon::Loose: return "Loose";
      case xAOD::Muon::VeryLoose: return "VeryLoose";
      default: return "UnknownQuality";
      }
    }
    
  }//namespace HistParamDefs
}//namespace Muon

#endif
