/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_PARTITIONSGROUPER_H
#define TRIGHLTJETHYPO_PARTITIONSGROUPER_H

/*
 * Find all non-overlapping allocation of jet groups to conditions.
 * The number pf jets needed by a contition is input.
 *
 * std::vector<HypoJetGroupVector is a vector of ..
 *  ... a vector of a vector of jet indicies.
 * The permutations engine finds all the index  allocactions of jets to
 * condtions. Schematically, if the engine is told that c0 needs 2 jets
 * and c3 needs 3 jets,
 *
 * (i0, i1) (i3, i4, i5) assigns indices i0, i1 to c0 and  i3, i4, i5
 * to c1.
 * There will be many such assignments. The next one may be
 * (i0, i2) (i1, i3, i4)
 */


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"
#include <vector>
#include <fstream>

class PartitionsGrouper: public IJetGrouper{
 public:
  PartitionsGrouper(const std::vector<std::size_t>& mults);
  std::vector<HypoJetGroupVector> group(HypoJetIter&,
                           HypoJetIter&) const override;
  std::string getName() const override; 
  std::string toString() const override;
 private:

  // m_mults entry at position i gives the number of jets required by the
  // ith condition.
  std::vector<std::size_t> m_mults;
};
#endif
