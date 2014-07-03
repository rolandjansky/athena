/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MDTSTATIONID_H
#define DCMATH_MDTSTATIONID_H

#include <ostream>
#include <iostream>

namespace TrkDriftCircleMath {

  class MdtStationId {
  public:
    MdtStationId() : m_id(0) {}
    MdtStationId( int isBarrel, int stName, int eta, int phi) {
      eta+=50;
      m_id = 1000000*isBarrel + 10000*stName+100*eta+phi;
    }
    int isBarrel() const { return m_id/1000000; }
    int stName() const { return (m_id%1000000)/10000; }
    int eta() const { return (m_id%10000)/100-50; }
    int phi() const { return m_id%100; }
  private:
    int     m_id;
  };
  std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::MdtStationId& id );
}

#endif
