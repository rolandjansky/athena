/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ALLJETSGROUPER_H
#define TRIGHLTJETHYPO_ALLJETSGROUPER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class AllJetsGrouper: public IJetGrouper{
 public:
  AllJetsGrouper();
  AllJetsGrouper(const HypoJetCIter& b, const HypoJetCIter& e);
  AllJetsGrouper(const HypoJetVector&);
  
  virtual HypoJetVector next() override;
  virtual std::string getName() const override; 
  virtual std::string toString() const override;

private:
  HypoJetVector m_jets{};
  bool m_done{false};

};
#endif
