/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_NULLCLEANER_H
#define TRIGHLTJETHYPO_NULLCLEANER_H


// ********************************************************************
//
// NAME:     NullCleaner.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ICleaner.h"
#include "./FlowNetwork.h"

namespace AOD{
  class Jet;
}

class NullCleaner: public ICleaner{
  /* apply no cleaning cuts */
public:
  ~NullCleaner(){}

  bool operator()(const pHypoJet&) const override;
  std::string toString() const noexcept override;
  std::string getName() const noexcept override;
};


#endif
