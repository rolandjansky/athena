/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETEVENT_TRIGVERTEXCOUNTS_H
#define TRIGINDETEVENT_TRIGVERTEXCOUNTS_H

#include "AthenaKernel/CLASS_DEF.h"

/** @class TrigVertexCounts
 * @author W. H. Bell <W.Bell@cern.ch>
 * 
 * A class to store the number of tracks and the pT^2 sum per reconstructed
 * primary vertex. 
 */
class TrigVertexCounts {
public:
  
  /** Default constructor used by T/P converters. */
  TrigVertexCounts();

  /** Standard constructor used by FEX algorithms. */ 
  TrigVertexCounts(const std::vector<unsigned int>& vtxNtrks,
		   const std::vector<float>& vtxTrkPtSqSum);

  TrigVertexCounts(std::vector<unsigned int>&& vtxNtrks,
		   std::vector<float>&& vtxTrkPtSqSum);
  
  /** Destructor */
  ~TrigVertexCounts();

  const std::vector<unsigned int>& vtxNtrks(void) const { return m_vtxNtrks; }
  const std::vector<float>& vtxTrkPtSqSum(void) const { return m_vtxTrkPtSqSum; }
  
private:
 
  std::vector<unsigned int> m_vtxNtrks;
  std::vector<float> m_vtxTrkPtSqSum;
};
 
// obtained using clid -m TrigVertexCounts
CLASS_DEF( TrigVertexCounts , 64641956 , 1 )

#endif 
