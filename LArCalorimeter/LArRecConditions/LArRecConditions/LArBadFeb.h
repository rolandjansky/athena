/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadFeb_H
#define LArBadFeb_H

#include "LArRecConditions/LArBadFebEnum.h"

class  LArBadFeb : public LArBadFebEnum {
public:

  LArBadFeb() : m_word(0) {}

  explicit LArBadFeb( BitWord rawStatus) : m_word(rawStatus) {}
	
  /// Returns true if corresponding status bit its set		
  bool statusBad( ProblemType pb) const {
    BitWord mask = 1 << bitPosition(pb);
    return ((m_word & mask) != 0);
  }

  /// Returns true if there is no problem in corresponding status bit
  bool statusOK( ProblemType pb) const {return !statusBad(pb);}
	
  /// Returns true if no problems at all (all bits at zero)
  bool good() const {return (m_word << 16) == 0;} // only use the lower 16 bits

  /// FEB is completely missing, e.g. powered off
  bool deadAll() const {return statusBad( deadAllBit);} 

  /// FEB is not sending readout data, but the L1 trigger path is working
  bool deadReadout() const {return statusBad( deadReadoutBit);}

  /// FEB has readout errors, cannot be used
  bool inError() const {return statusBad( inErrorBit);}

  /// Deactivated in OKS
  bool deactivatedInOKS() const {return statusBad( deactivatedInOKSBit);}

  unsigned int ignoreErrors() const;

  static PosType bitPosition( ProblemType pb) { return static_cast<PosType>(pb);}

  bool operator!=(BitWord other) {return m_word != other;}
  bool operator!=(LArBadFeb other) {return m_word != other.packedData();}
  bool operator==(BitWord other) {return m_word == other;}
  bool operator==(LArBadFeb other) {return m_word == other.packedData();}

  LArBadFeb& operator|=(LArBadFeb other) {m_word|=other.m_word; return *this;}

  BitWord packedData() const {return m_word;}

private:

  BitWord m_word;

  BitWord& packedDataRef() {return m_word;}
	
  friend class LArBadFebBitPacking;

};


inline unsigned int LArBadFeb::ignoreErrors() const 
{
  return (m_word >> 16);  // return the most significant 16 bits 
}

#endif
