/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2ZDCSIGNALS_H
#define TRIGT2ZDCSIGNALS_H

#include "AthenaKernel/CLASS_DEF.h"
#include "GaudiKernel/MsgStream.h"

#include <vector>

/** @class TrigT2ZdcSignals
 
@author W. H. Bell <W.Bell@cern.ch>

A class to contain the energies and times of each ZDC module after
signal processing.
*/  
class TrigT2ZdcSignals {

 public:
  TrigT2ZdcSignals(void);
  TrigT2ZdcSignals(const std::vector<float>& triggerEnergies,
                   const std::vector<float>& triggerTimes);
  TrigT2ZdcSignals(std::vector<float>&& triggerEnergies,
                   std::vector<float>&& triggerTimes);
  ~TrigT2ZdcSignals(void);

  /** Return the trigger energies of each counter */
  const std::vector<float>& triggerEnergies(void) const { return m_triggerEnergies; }

  /** Return the relative times of the triggers */
  const std::vector<float>& triggerTimes(void) const { return m_triggerTimes; }

  /**  Prints out data members to std::cout */
  void print(void) const;
  
  /**  Prints out data members to MsgStream */
  void print(MsgStream& log) const;

  /** A data member to contain the number of ZDC signals */
  static const int NUM_ZDC = 8;

 private:
  std::vector<float> m_triggerEnergies;
  std::vector<float> m_triggerTimes;
};

/// Helper function for printing the object
std::string str(const TrigT2ZdcSignals& trigT2ZdcSignals);

/// Helper operator for printing the object
MsgStream& operator<< (MsgStream& m, const TrigT2ZdcSignals& trigT2ZdcSignals);

CLASS_DEF( TrigT2ZdcSignals , 86299156 , 1 )
#endif
