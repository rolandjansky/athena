/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ALLJETSGROUPER_H
#define TRIGHLTJETHYPO_ALLJETSGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class AllJetsGrouper: public IJetGrouper{
 public:
  AllJetsGrouper();
  AllJetsGrouper(const HypoJetIter& b, const HypoJetIter& e);
  AllJetsGrouper(const HypoJetVector&);
  
  std::vector<HypoJetGroupVector> group(HypoJetIter&,
					HypoJetIter&) const override;
  
  std::optional<HypoJetGroupVector> next();
  
  std::string getName() const override; 
  std::string toString() const override;

private:
  HypoJetVector m_jets{};
  bool m_done{false};

};
#endif
