/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SINGLEJETGROUPER_H
#define TRIGHLTJETHYPO_SINGLEJETGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class SingleJetGrouper: public IJetGrouper{
 public:
  SingleJetGrouper();
  SingleJetGrouper(const HypoJetVector&);
  SingleJetGrouper(const HypoJetCIter& b, const HypoJetCIter& e);

  std::vector<HypoJetGroupVector> group(HypoJetIter&,
					HypoJetIter&) const override;

  std::optional<HypoJetVector> next() override;
  std::string getName() const override; 
  std::string toString() const override;

private:
  HypoJetVector m_jets{};
  std::size_t m_size{0};
  std::size_t m_index{0};
};
#endif
