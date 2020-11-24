/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTID_H
#define DCMATH_MDTID_H

#include <ostream>

namespace {
  // the tube number of a tube in a tubeLayer in encoded in the GeoSerialIdentifier (modulo maxNTubesPerLayer)
  static constexpr unsigned int const maxNTubesPerLayer = 120;
}

namespace TrkDriftCircleMath {

  class MdtId {
  public:
    MdtId() : m_id(0) {}
    MdtId( int isBarrel, int ml, int lay, int tube ) {
      m_id = 100000*isBarrel + 10000*ml+maxNTubesPerLayer*lay+tube;
    }
    int isBarrel() const { return m_id/100000; }
    int ml() const { return (m_id%100000)/10000; }
    int lay() const { return (m_id%10000)/maxNTubesPerLayer; }
    int tube() const { return (m_id%10000)%maxNTubesPerLayer; }
    int value() const { return m_id; }

    bool operator==(const MdtId &other) const {
      return m_id == other.m_id;
    }
    bool operator!=(const MdtId &other) const {
      return !(*this==other);
    }

  private:
    int m_id;
  };

//DR (clang32) operator<< to be defined inside the namespace
std::ostream& operator<<( std::ostream& os, const MdtId& id );

}

#endif
