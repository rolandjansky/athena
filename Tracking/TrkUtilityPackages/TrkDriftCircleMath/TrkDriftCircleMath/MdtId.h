/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTID_H
#define DCMATH_MDTID_H

#include <ostream>

namespace TrkDriftCircleMath {

  class MdtId {
  public:
    MdtId() : m_id(0) {}
    MdtId( int isBarrel, int ml, int lay, int tube ) {
      m_id = 100000*isBarrel + 10000*ml+100*lay+tube;
    }
    int isBarrel() const { return m_id/100000; }
    int ml() const { return (m_id%100000)/10000; }
    int lay() const { return (m_id%10000)/100; }
    int tube() const { return m_id%100; }
    int value() const { return m_id; }

    bool operator==(const MdtId &other) const {
      return m_id == other.m_id;
    }
    bool operator!=(const MdtId &other) const {
      return !(*this==other);
    }

  private:
    int     m_id;
  };

//DR (clang32) operator<< to be defined inside the namespace
std::ostream& operator<<( std::ostream& os, const MdtId& id );

}

#endif
