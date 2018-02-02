/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGTRACKCOUNTS_H
#define TRIGINDETEVENT_TRIGTRACKCOUNTS_H

#include "TrigInDetEvent/TrigHisto2D.h"

#include "AthenaKernel/CLASS_DEF.h"

/** @class TrigTrackCounts
 * @author Regina Kwee <regina.kwee@cern.ch>
 * 
 * A class to store reconstructed track distributions: z0 vs pt and
 * eta vs phi.
 */
class TrigTrackCounts {
public:
  
  /** Default constructor used by T/P converters. */
  TrigTrackCounts();

  /** Standard constructor used by FEX algorithms. */ 
  TrigTrackCounts(const TrigHisto2D& z0_pt,
		  const TrigHisto2D& eta_phi);

  TrigTrackCounts(TrigHisto2D&& z0_pt,
		  TrigHisto2D&& eta_phi);
  
  /** Destructor */
  ~TrigTrackCounts();

  const TrigHisto2D& z0_pt(void) const;

  const TrigHisto2D& eta_phi(void) const;
  
private:
 
  /** A histogram of the reconstructed track z0 vs pt */
  TrigHisto2D m_z0_pt;

  /** A histogram of the reconstructed track eta vs phi */
  TrigHisto2D m_eta_phi;
};
 
// obtained using clid -m TrigTrackCounts
CLASS_DEF( TrigTrackCounts , 24895180 , 1 )

#endif 
