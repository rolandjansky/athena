/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_SINGLEJETGROUPER_H
#define TRIGHLTJETHYPO_SINGLEJETGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class SingleJetGrouper: public IJetGrouper{
 public:
  HypoJetGroupVector group(HypoJetIter&,
                           HypoJetIter&) const override;
  std::string getName() const override; 
  std::string toString() const override;
};
#endif
