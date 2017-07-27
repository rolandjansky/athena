/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChannel_H
#define LArBadChannel_H

#include "LArRecConditions/LArBadChannelEnum.h"
#include "LArRecConditions/LArBadChanBitPacking.h"

class  LArBadChannel : public LArBadChannelEnum {
 public:

 LArBadChannel() : m_word(0) {}

  explicit LArBadChannel( BitWord rawStatus) : m_word(rawStatus) {}
	
  /// Returns true if corresponding status bit its set		
  bool statusBad( ProblemType pb) const {
    BitWord mask = 1 << bitPosition(pb);
    return ((m_word & mask) != 0);
  }

  /// Returns true if there is no problem in corresponding status bit
  bool statusOK( ProblemType pb) const {return !statusBad(pb);}
	
  /// Returns true if no problems at all (all bits at zero)
  bool good() const {return m_word == 0;}
	
  bool deadReadout() const { return statusBad( deadReadoutBit);}
  bool deadCalib() const { return statusBad( deadCalibBit);} 
  bool deadPhys() const { return statusBad( deadPhysBit);} 
  bool almostDead() const { return statusBad( almostDeadBit);} 
  bool shortProblem() const { return statusBad( shortBit);} 
  bool unstable() const { return statusBad( unstableBit);}
  bool distorted() const { return statusBad( distortedBit);}
  bool lowNoiseHG() const { return statusBad( lowNoiseHGBit);}
  bool highNoiseHG() const { return statusBad( highNoiseHGBit);}
  bool unstableNoiseHG() const { return statusBad( unstableNoiseHGBit);}
  bool lowNoiseMG() const { return statusBad( lowNoiseMGBit);}
  bool highNoiseMG() const { return statusBad( highNoiseMGBit);}
  bool unstableNoiseMG() const { return statusBad( unstableNoiseMGBit);}
  bool lowNoiseLG() const { return statusBad( lowNoiseLGBit);}
  bool highNoiseLG() const { return statusBad( highNoiseLGBit);}
  bool unstableNoiseLG() const { return statusBad( unstableNoiseLGBit);}
  bool missingFEB() const { return statusBad( missingFEBBit);}
  bool peculiarCalibrationLine() const { return statusBad( peculiarCalibrationLineBit);}
  bool problematicForUnknownReason() const { return statusBad( problematicForUnknownReasonBit);}
  bool sporadicBurstNoise() const {return statusBad( sporadicBurstNoiseBit);}
  bool deadSCACell() const {return statusBad( deadSCACellBit);}
  bool badFirstSample() const {return statusBad( badFirstSampleBit);}
	
  bool reallyNoisy() const {return (highNoiseHG() || highNoiseMG() || highNoiseLG() ||
				    unstableNoiseHG() || unstableNoiseMG() || unstableNoiseLG());}
  bool noisy() const {return (reallyNoisy() || lowNoiseHG() || lowNoiseMG() || lowNoiseLG());}
				
  static PosType bitPosition( ProblemType pb)
  {
    // if no bit packing object set, use enumerators as bit positions
    return s_packing == 0 ? static_cast<PosType>(pb) : s_packing->bitPosition(pb);
  }

  bool operator!=(BitWord other) {return m_word != other;}
  bool operator!=(LArBadChannel other) {return m_word != other.packedData();}
  bool operator==(BitWord other) {return m_word == other;}
  bool operator==(LArBadChannel other) {return m_word == other.packedData();}

  LArBadChannel& operator|=(LArBadChannel other) {m_word|=other.m_word; return *this;}

  BitWord packedData() const {return m_word;}

 private:

	

  BitWord m_word;
  static LArBadChanBitPacking* s_packing;
  // FIXME declare a friend to set the m_packing

  BitWord& packedDataRef() {return m_word;}
	
  friend class LArBadChanBitPacking;
};


#endif
