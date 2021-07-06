/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  
  virtual HypoJetVector next() override;
  virtual std::string getName() const override; 
  virtual std::string toString() const override;

private:
  unsigned int m_groupSize{0u};
  HypoJetVector m_jets;

};
#endif
