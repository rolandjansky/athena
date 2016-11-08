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
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IGroupsMatcher.h"

class GroupsMatcher: virtual public IGroupsMatcher {
 public:
  GroupsMatcher(const std::string& name);
  std::string toString() const noexcept override;
 private:
  std::string m_name;
};
#endif
