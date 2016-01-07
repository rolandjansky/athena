/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_CLEANERBRIDGE_H
#define TRIGJETHYPO_CLEANERBRIDGE_H
#define TRIGJETHYPO_TRIGHLTJETHYPOUTILS_H
/********************************************************************
 *
 * NAME:     CleanerBridge.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "xAODJet/Jet.h"
#include <string>
#include <memory>
#include "TrigJetHypo/TrigHLTJetHypoUtils/ICleaner.h"

class CleanerBridge{
  // Bridge object: convert polymorphic pointer to monomorthic class
  // can be passed to STL algs.
 public:
 CleanerBridge(const std::shared_ptr<ICleaner>& cleaner):
  m_pCleaner(cleaner){}

  bool operator()(const xAOD::Jet* jet) const{
    return m_pCleaner -> operator()(jet);
  }

  std::string toString() const {
    return m_pCleaner -> toString();
  }

 private:
  std::shared_ptr<ICleaner> m_pCleaner;
};
#endif
