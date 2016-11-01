/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_INDEXEDJETSGOUPER_H
#define TRIGHLTJETHYPO_INDEXEDJETSGOUPER_H


// ********************************************************************
//
// NAME:     IndexedJetsGrouper.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJetGrouper.h"

class IndexedJetsGrouper: public IJetGrouper{
  /* Select jets ordered all jets in descnding Et at goven index positions.*/
public:
  IndexedJetsGrouper(const std::vector<unsigned int>& indices);
  virtual ~IndexedJetsGrouper(){}
  HypoJetGroupVector group(HypoJetIter&, HypoJetIter&) const override;
  std::string getName() const override;
  std::string toString() const override;
private:
  std::vector<unsigned int> m_indices;
};
#endif
