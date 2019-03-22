/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_COMBINATIONSGROUPER_H
#define TRIGHLTJETHYPO_COMBINATIONSGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class CombinationsGrouper: public IJetGrouper{
 public:
  CombinationsGrouper(unsigned int);
  HypoJetGroupVector group(HypoJetIter&,
                           HypoJetIter&) const override;
  std::string getName() const override; 
  std::string toString() const override;
 private:
  unsigned int m_groupSize;
};
#endif
