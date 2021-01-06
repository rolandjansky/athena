/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_COMBINATIONSGROUPER_H
#define TRIGHLTJETHYPO_COMBINATIONSGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class CombinationsGrouper: public IJetGrouper{
 public:
  CombinationsGrouper();

  CombinationsGrouper(unsigned int);

  CombinationsGrouper(unsigned int, const HypoJetVector&);

  CombinationsGrouper(unsigned int,
		      const HypoJetCIter& b,
		      const HypoJetCIter& e
		      );
  
  std::vector<HypoJetGroupVector> group(HypoJetIter&,
					HypoJetIter&) const override;
  std::optional<HypoJetVector> next() override;

  std::string getName() const override; 
  std::string toString() const override;
 private:
  unsigned int m_groupSize;
  HypoJetVector m_jets;

};
#endif
