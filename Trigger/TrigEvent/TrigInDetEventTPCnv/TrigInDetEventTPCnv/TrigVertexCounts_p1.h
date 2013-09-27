/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCounts_p1
 *
 * @brief persistent partner for TrigVertexCounts
 *
 * @author W. H. Bell <W.Bell@cern.ch>
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTS_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCOUNTS_P1_H

#include <vector>

class TrigVertexCounts_p1 {
  friend class TrigVertexCountsCnv_p1;

 public:
  TrigVertexCounts_p1 () {}
  virtual ~TrigVertexCounts_p1 () {}

 private:
  std::vector<unsigned int> m_vtxNtrks;
  std::vector<float> m_vtxTrkPtSqSum;
};

#endif
