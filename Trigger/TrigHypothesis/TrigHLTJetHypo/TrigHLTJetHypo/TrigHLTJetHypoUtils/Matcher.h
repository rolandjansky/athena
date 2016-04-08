/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MATCHER_H
#define TRIGHLTJETHYPO_MATCHER_H


// ********************************************************************
//
// NAME:     MatcherImp.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcher.h"

class Matcher: virtual public IMatcher {
 public:
  Matcher(const std::string& name);
  std::string toString() const noexcept override;
 private:
  std::string m_name;
};
#endif
