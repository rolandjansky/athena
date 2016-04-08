/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_CLEANERMATCHERFACTORY_H
#define TRIGHLTJETHYPO_CLEANERMATCHERFACTORY_H
/********************************************************************
 *
 * NAME:     CleanerMatcherFactory.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include <string>
#include <memory>
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/matcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerMatcher.h"

// class CleanerMatcher;

class CleanerMatcherFactory {
 public: 
  CleanerMatcherFactory(const CleanerFactory& cf,
			const std::shared_ptr<IMatcherFactory>& mf);

  CleanerMatcher make() const;
 
 private:
  CleanerFactory m_cleanerFactory;
  std::shared_ptr<IMatcherFactory>  m_matcherFactory;
};

#endif

