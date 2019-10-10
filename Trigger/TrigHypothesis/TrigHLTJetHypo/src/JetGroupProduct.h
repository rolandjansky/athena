/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPPRODUCT_H
#define TRIGHLTJETHYPO_JETGROUPPRODUCT_H

#include "./ProductGen.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <vector>
#include <optional>

class JetGroupProduct{
  /*
   * Iterate through the combinations of jet grpuops talen from a vector
   * of vector of jet groups. return the merge of the groups)
   * eg for [(jg11, jj12), (jg21)], the product is [(jg11,jg21), (jg12, jg21)]
   * If jg11 contains jets j111, j112, jg12 contains jg121, jg 122 and jg21 
   * contains jg211, then the merges contain 
   * [(j111, jg112, jg211)(jg121, jg122, jg121)] from the sequence of all
   * merged groups.
   */
public:
  JetGroupProduct(const std::vector<HypoJetGroupVector>& inVecs);
  std::optional<HypoJetVector> next();

private:
  const std::vector<HypoJetGroupVector> m_inVecs;
  const std::size_t m_nVec;
  ProductGen m_productGen;
};

#endif
