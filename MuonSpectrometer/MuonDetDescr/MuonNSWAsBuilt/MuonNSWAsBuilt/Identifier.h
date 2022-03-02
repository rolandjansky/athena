/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONNSWASBUILT_IDENTIFIER_H
#define MUONNSWASBUILT_IDENTIFIER_H

#include <functional>

namespace NswAsBuilt {

  // This file defines structs with the list of indices needed to address a
  // quadruplet, a PCB or a strip. (Probably something similar already exists
  // somewhere in the code)

  /**
  * Athena indices for a MM quadruplet:
  *  - stationName: MMS(55) or MML(56) for Micromegas chambers
  *  - eta: 1,2 (A side), -1,-2 (C side)
  *  - phi: in [1,8]
  *  - multilayer: 1 for IP quadruplet, 2 for HO quadruplet
  */
  struct quadrupletIdentifier_t {
    enum StationName {
      MMS=55,
      MML=56
    };

    StationName stationName{};
    int stationEta{0};
    int stationPhi{0};
    int multilayer{0};

    std::string stationNameStr() const {
      return (stationName==MMS) ? "MMS" : "MML";
    }
    
    bool operator<(const quadrupletIdentifier_t& o) const {
      return std::tie(stationName,stationEta,stationPhi,multilayer) < std::tie(o.stationName,o.stationEta,o.stationPhi,o.multilayer);
    }
    
    bool operator==(const quadrupletIdentifier_t& o) const {
      return std::tie(stationName,stationEta,stationPhi,multilayer) == std::tie(o.stationName,o.stationEta,o.stationPhi,o.multilayer);
    }
    
  };

  /**
  * The indices of a MM PCB
  *  - ilayer: in [1,4], layer 1 is the closest to IP
  *  - ipcb: in [1,5] for LM1 and SM1, in [6,8] for LM2 and SM2
  */
  struct pcbIdentifier_t {
    quadrupletIdentifier_t quadruplet{};
    int ilayer{0};
    int ipcb{0};

    bool operator<(const pcbIdentifier_t& o) const {
      return std::tie(quadruplet, ilayer, ipcb) < std::tie(o.quadruplet, o.ilayer, o.ipcb);
    }
    
    bool operator==(const pcbIdentifier_t& o) const {
      return std::tie(quadruplet, ilayer, ipcb) == std::tie(o.quadruplet, o.ilayer, o.ipcb);
    }
  };

  /**
  * Athena indices of a MM strip
  *  - ilayer: in [1,4], layer 1 is the closest to IP
  *  - istrip: from 1 to 5*1024 (LM1 and SM1) or 1 to 3*1024 (LM2 and SM2)
  */
  struct stripIdentifier_t {
    quadrupletIdentifier_t quadruplet{};
    int ilayer{0};
    int istrip{0};

    bool operator<(const stripIdentifier_t& o) const {
      return std::tie(quadruplet, ilayer, istrip) < std::tie(o.quadruplet, o.ilayer, o.istrip);
    }
    bool operator==(const stripIdentifier_t& o) const {
      return std::tie(quadruplet, ilayer, istrip) == std::tie(o.quadruplet, o.ilayer, o.istrip);
    }
  };

}


// Define std::hash for the types above, to be able to use them is std::unordered_map
// the hex value (0x9e3779b9) is just a trick to combine hash values
namespace std {
  template<> struct hash<NswAsBuilt::quadrupletIdentifier_t> {
    std::size_t operator()(const NswAsBuilt::quadrupletIdentifier_t& v) const noexcept {
      // Computing hash directly (avoid dependency to boost::hash_combine)
      std::hash<int> hasher;
      std::size_t h1 = hasher(v.stationName);
      std::size_t h2 = hasher(v.stationEta);
      std::size_t h3 = hasher(v.stationPhi);
      std::size_t h4 = hasher(v.multilayer);
      std::size_t ret = h1;
      ret ^= h2 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      ret ^= h3 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      ret ^= h4 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      return ret;
    }
  };

  template<> struct hash<NswAsBuilt::pcbIdentifier_t> {
    std::size_t operator()(const NswAsBuilt::pcbIdentifier_t& v) const noexcept {
      // Computing hash directly (avoid dependency to boost::hash_combine)
      std::size_t h1 = std::hash<NswAsBuilt::quadrupletIdentifier_t>{}(v.quadruplet);
      std::size_t h2 = std::hash<int>{}(v.ilayer);
      std::size_t h3 = std::hash<int>{}(v.ipcb);
      std::size_t ret = h1;
      ret ^= h2 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      ret ^= h3 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      return ret;
    }
  };

  template<> struct hash<NswAsBuilt::stripIdentifier_t> {
    std::size_t operator()(const NswAsBuilt::stripIdentifier_t& v) const noexcept {
      // Computing hash directly (avoid dependency to boost::hash_combine)
      std::size_t h1 = std::hash<NswAsBuilt::quadrupletIdentifier_t>{}(v.quadruplet);
      std::size_t h2 = std::hash<int>{}(v.ilayer);
      std::size_t h3 = std::hash<int>{}(v.istrip);
      std::size_t ret = h1;
      ret ^= h2 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      ret ^= h3 + 0x9e3779b9 + (ret << 6) + (ret >> 2);
      return ret;
    }
  };
}

#endif

