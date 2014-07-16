/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEEVENT_TILEROI_ID_H
#define TILEEVENT_TILEROI_ID_H

#include <iostream> 

class TileRoI_ID 
{
public:

  TileRoI_ID():m_sample(-999),m_eta(-999),m_phi(-999) { }  
  TileRoI_ID(int s, int e, int p):m_sample(s),m_eta(e),m_phi(p) { }  

  ~TileRoI_ID() { }

  /** Comparator 
   */ 
  bool operator < (  const TileRoI_ID& id2) const {

    if(m_sample == id2.m_sample) {
      if(m_eta == id2.m_eta) { 
        return m_phi < id2.m_phi; 
      } else {
        return m_eta<id2.m_eta; 
      }
    } else {
      return m_sample< id2.m_sample; 
    }
  }

  bool operator == (  const TileRoI_ID& id2 ) const {
    return m_sample == id2.m_sample && 
              m_eta == id2.m_eta && 
              m_phi == id2.m_phi ; 
  }

  int sample() const {return m_sample;}

  int eta() const {return m_eta;}

  int phi() const {return m_phi;}

private: 

  int m_sample; 
  int m_eta; 
  int m_phi; 
};

// Output stream.
inline std::ostream& operator<<(std::ostream& lhs, const TileRoI_ID& rhs)
{
  lhs<<"TileRoI_ID(sample,eta,phi)="<<rhs.sample()<<","
     <<rhs.eta()   <<","
     <<rhs.phi() ; 
  return lhs; 
}

#endif

