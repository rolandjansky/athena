/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2MBTSBITS_H
#define TRIGT2MBTSBITS_H

#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

/** @class TrigT2MbtsBits
 
@author W. H. Bell <W.Bell@cern.ch>

A class to contain the DSP energies and times of each MBTS counter.
*/  
class TrigT2MbtsBits {

 public:
  TrigT2MbtsBits(void);
  TrigT2MbtsBits(const std::vector<float>& triggerEnergies,
                 const std::vector<float>& triggerTimes);
  ~TrigT2MbtsBits(void);

  /** Return the trigger energies of each counter */
  const std::vector<float>& triggerEnergies(void) const { return m_triggerEnergies; }

  /** Return the relative times of the triggers */
  const std::vector<float>& triggerTimes(void) const { return m_triggerTimes; }

  /**  Prints out data members to std::cout */
  void print(void) const;
  
  /**  Prints out data members to MsgStream */
  void print(MsgStream& log) const;

  /** A data member to contain the number of MBTS counters */
  static const int NUM_MBTS = 32;

 private:
  std::vector<float> m_triggerEnergies;
  std::vector<float> m_triggerTimes;
};

/// Helper function for printing the object
std::string str(const TrigT2MbtsBits& trigT2MbtsBits);

/// Helper operator for printing the object
MsgStream& operator<< (MsgStream& m, const TrigT2MbtsBits& trigT2MbtsBits);

CLASS_DEF( TrigT2MbtsBits , 100986084 , 1 )
#endif
