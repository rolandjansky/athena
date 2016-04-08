/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
* TrigInDetTrackTruth Helper class to keep numbers of hits in ID sub-detectors
*
* Copied from Tracking/TrkEvent/TrkTruthData/TrkTruthData/SubDetHitStatistics.h
*/

#ifndef TRIGIDHITSTATS_TRUTH_H
#define TRIGIDHITSTATS_TRUTH_H

#include <cstring>
#include "CLIDSvc/CLASS_DEF.h"


class TrigIDHitStats{

  public:

  enum IDSubDetType { PIX=0, SCT, TRT, NUM_SUBDETECTORS };

  /** Constructors: POOL needs default constructor */
  TrigIDHitStats() {
    memset(m_numHits, 0, NUM_SUBDETECTORS);
  }

  virtual ~TrigIDHitStats() { }

  unsigned char& operator[](IDSubDetType i) { return m_numHits[i]; }

  const unsigned char& operator[](IDSubDetType i) const  { return m_numHits[i]; }

  unsigned int total() const { 
    unsigned char tot=0;
    for(unsigned i=0; i<NUM_SUBDETECTORS; i++) {
      tot += m_numHits[i];
    }
    return (unsigned int)tot;
  }

  unsigned int pixhits() const { return (unsigned int)m_numHits[PIX];}
  unsigned int scthits() const { return (unsigned int)m_numHits[SCT];}
  unsigned int trthits() const { return (unsigned int)m_numHits[TRT];}


  TrigIDHitStats& operator+=(const TrigIDHitStats& b) {
    for(unsigned i=0; i<NUM_SUBDETECTORS; i++) {
      m_numHits[i] += b.m_numHits[i];
    }
    return *this;
  }

private:

  // For InDet, the largest typical number of measurements per track is 36 in  
  // the TRT. One byte is enough to keep any of the numbers.
  unsigned char m_numHits[NUM_SUBDETECTORS];

};

template<class Ostream> Ostream& operator<<(Ostream& os, const TrigIDHitStats& m) {
  os<<"IDSubDetStat(";
  for(unsigned i=0; i<TrigIDHitStats::NUM_SUBDETECTORS; i++) {
    os<<unsigned(m[TrigIDHitStats::IDSubDetType(i)]);
    if(i!=TrigIDHitStats::NUM_SUBDETECTORS) os<<",";
  }
  return os<<")";
}

CLASS_DEF( TrigIDHitStats , 212921831 , 1 )
#endif
