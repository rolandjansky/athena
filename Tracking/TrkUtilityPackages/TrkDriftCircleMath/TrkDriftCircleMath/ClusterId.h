/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_CLUSTERID_H
#define DCMATH_CLUSTERID_H

#include <ostream>

namespace TrkDriftCircleMath {

  class ClusterId {
  public:
    ClusterId() : m_id(0) {}
    ClusterId( int stName, int eta, int phi, int barrel, int measuresPhi ) {
      m_id = 1000000*stName+ 10000*eta + 100*phi + 10*barrel + measuresPhi;
    }
    
    int id() const { return m_id; }
    int stName() const { return m_id/1000000; }
    int eta() const { return (m_id%1000000)/10000; }
    int phi() const { return (m_id%10000)/100; }
    int isTgc() const { return (m_id%100)/10; }
    int measuresPhi() const { return m_id%10; }
  private:
    int     m_id;
  };

  std::ostream& operator<<( std::ostream& os, const TrkDriftCircleMath::ClusterId& id );

}

#endif
