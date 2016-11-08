/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     CleanerWrapper.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerWrapper.h"
#include <algorithm>

CleanerWrapper::CleanerWrapper(const CleanerBridge& cleaner): m_cleaner(cleaner){}

pairHypoJetIter CleanerWrapper::operator()(const pairHypoJetIter& iters) const {
  auto end = std::partition(iters.first, iters.second, m_cleaner);
  return pairHypoJetIter(iters.first, end);
}

std::string CleanerWrapper::toString() const noexcept {
  return m_cleaner.toString();
}

std::string CleanerWrapper::getName() const noexcept  {return "CleanerWrapper";}
