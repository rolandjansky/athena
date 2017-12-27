/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRTHITCOUNTER_H
#define TRIGINDETEVENT_TRTHITCOUNTER_H

#include "TrigInDetEvent/TrigHisto1D.h"

#include "AthenaKernel/CLASS_DEF.h"

/** @class TrigTrtHitCounts
 * @author Regina Kwee <regina.kweeATcern.ch>
 * 
 * @brief A class to store the distributions of time of threshold from
 * the TRT raw data.
 *
 */
class TrigTrtHitCounts {
 public:
  
  /** Default constructor used by T/P converters. */
  TrigTrtHitCounts(void);

  /** Standard constructor used by FEX algorithms. */
  TrigTrtHitCounts(const TrigHisto1D& endcapC,
		   const TrigHisto1D& barrel,
		   const TrigHisto1D& endcapA);

  TrigTrtHitCounts(TrigHisto1D&& endcapC,
		   TrigHisto1D&& barrel,
		   TrigHisto1D&& endcapA);

  /** Destructor */
  ~TrigTrtHitCounts(void);

  /** Return a histogram of time over threshold for endcap C pixel clusters */
  const TrigHisto1D& endcapC(void) const;
  
  /** Return a histogram of time over threshold for barrel pixel clusters */
  const TrigHisto1D& barrel(void) const; 
  
  /** Return a histogram of time over threshold for endcap A pixel clusters */
  const TrigHisto1D& endcapA(void) const;

private:
  TrigHisto1D m_endcapC;
  TrigHisto1D m_barrel;
  TrigHisto1D m_endcapA;
};
 
// obtained using clid -m TrigTrtHitCounts
CLASS_DEF( TrigTrtHitCounts , 168287091 , 1 )

#endif 
