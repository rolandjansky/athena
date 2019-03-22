/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ALLJETSGROUPER_H
#define TRIGHLTJETHYPO_ALLJETSGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class AllJetsGrouper: public IJetGrouper{
 public:
  HypoJetGroupVector group(HypoJetIter&,
                           HypoJetIter&) const override;
  std::string getName() const override; 
  std::string toString() const override;
};
#endif
