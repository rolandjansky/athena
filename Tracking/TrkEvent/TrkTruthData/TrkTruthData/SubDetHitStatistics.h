/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/**
 * @file
 *
 * A struture to keep per-subdetector numbers of hits for truth
 * matching.  The numbers are typically small, so only one byte
 * (unsigned char) is per subdetector is allocated.  However it's
 * been observed that a low-energy track looping in the TRT may
 * sometimes produce more than 255 hits, causing an integer overflow
 * and producing "impossible" Tanimoto similarity measure > 1.
 * This is now addressed by the OverflowControlledRef helper class
 * that provides a protection layer for the hit count storage.
 * 
 * Counts of up to OVERFLOW_VALUE-1 hits can be stored unambiguously.
 * If a SubDetHitStatistics counter is set to OVERFLOW_VALUE, that means
 * there were OVERFLOW_VALUE or more hits.
 *
 * @author Andrei Gaponenko, 2006-2009
 */

#ifndef SUBDETHITSTATISTICS_H
#define SUBDETHITSTATISTICS_H

#include "CLIDSvc/CLASS_DEF.h"

class TrackTruthCollectionAccessor;


class SubDetHitStatistics {
public:

  /** 
   * A simple class to retrofit overflow protection into the existing
   * code.  The intention is not to provide a full-blown "tiny int"
   * facility but rather a class that is sufficiently functional to
   * compile existing code in other packages without modifications.
   */
  class OverflowControlledRef {
    unsigned char *m_p;
  public:
    static const unsigned int OVERFLOW_VALUE = 255;
    
    /** constructor from a ref to the storage we control */
    OverflowControlledRef(unsigned char& c) : m_p(&c) {}

    /** conversion to a number */
    operator unsigned int () const { return *m_p; }
    unsigned int value() const { return *m_p; }

    /** protected assignment */
    OverflowControlledRef operator=(unsigned int rhs) { *m_p = (rhs > OVERFLOW_VALUE) ? OVERFLOW_VALUE : rhs; return *this; }

    /** protected (prefix) increment */
    OverflowControlledRef operator++() { unsigned int tmp = 1 + *m_p; *m_p = (tmp > OVERFLOW_VALUE) ? OVERFLOW_VALUE : tmp; return *this; }

    /** protected add-to */
    OverflowControlledRef operator+=(unsigned int rhs) {
      unsigned int tmp = *m_p + rhs;
      *m_p = ((tmp > OVERFLOW_VALUE)
	      ||(tmp < rhs)/*protection agains overflowing tmp */
	      ) ? OVERFLOW_VALUE : tmp;
      return *this;
    }
  };  // end of class OverflowControlledRef

  //----------------------------------------------------------------
  // SubDetHitStatistics non-class members

  enum SubDetType { Pixel=0, SCT, TRT, MDT, RPC, TGC, CSC, MM, STGC, NUM_SUBDETECTORS };
  static const unsigned int OVERFLOW_VALUE = OverflowControlledRef::OVERFLOW_VALUE;

  SubDetHitStatistics();

  /** Counts of up to (OVERFLOW_VALUE-1) hits */
  unsigned int operator[](SubDetType i) const { return m_numPRDs[i]; }

  /** Write access protected agains overflows*/
  OverflowControlledRef operator[](SubDetType i) { return m_numPRDs[i]; }

private:
  /** For InDet, the largest typical number of measurements per track is 36 in the TRT. 
   *  Perhaps muons do not bit that.  One byte is enough to keep any of the numbers
   *  in a typical event. (But not for a track looping in the TRT!)
   */
  unsigned char m_numPRDs[NUM_SUBDETECTORS];

  friend class TrackTruthCollectionAccessor; // in TrackTruthTPCnv

public:
};

template<class Ostream> Ostream& operator<<(Ostream& os, const SubDetHitStatistics& m) {
  os<<"SubDetStat(";
  for(unsigned i=0; i<SubDetHitStatistics::NUM_SUBDETECTORS; i++) {
    os<<unsigned(m[SubDetHitStatistics::SubDetType(i)]);
    if(i!=SubDetHitStatistics::NUM_SUBDETECTORS) os<<",";
  }
  return os<<")";
}

// That method would be the *only* one in a .so file.
// So inline it and get rid of the library.  
// Also note that memset()-ing 7 bytes can be inlined by gcc.
#include <cstring>
inline SubDetHitStatistics::SubDetHitStatistics() {
  memset(m_numPRDs, 0, NUM_SUBDETECTORS);
}

CLASS_DEF(SubDetHitStatistics, 73400190, 1)

#endif/*SUBDETHITSTATISTICS_H*/
