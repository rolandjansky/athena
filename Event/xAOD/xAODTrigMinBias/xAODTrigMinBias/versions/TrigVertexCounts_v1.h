/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigVertexCounts_V1_H
#define xAODTrigMinBias_TrigVertexCounts_V1_H

#include "AthContainers/AuxElement.h"

namespace xAOD {

  class TrigVertexCounts_v1 : public SG::AuxElement {
  public:
    /**default constructor**/
    TrigVertexCounts_v1();

  /** Destructor **/
  ~TrigVertexCounts_v1();

  /*!getters */
  const std::vector<unsigned int>& vtxNtrks() const;
  
  const std::vector<float>& vtxTrkPtSqSum() const; 

  /*setters*/
  
  void setVtxNtrks(const std::vector<unsigned int> & m_vtxNtrks);
  
  void setVtxTrkPtSqSum(const std::vector<float> &m_vtxTrkPtSqSum);
  
  };
}

#endif

