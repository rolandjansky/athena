/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_CLEANERMATCHER_H
#define TRIGJETHYPO_CLEANERMATCHER_H
/********************************************************************
 *
 * NAME:     CleanerMatcher.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include <string>
#include <memory>
#include "TrigJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/CleanerBridge.h"

class CleanerMatcher{
  /*
    Object that takes a jet cleaner and a matcher.
    A matcher checks whether there is a subset of the jets
    that fulfill the hypo conditions.
  */
 public:
 CleanerMatcher(const std::shared_ptr<ICleaner>& cleaner,
                const std::shared_ptr<IMatcher>& matcher):
  m_cleaner(CleanerBridge(cleaner)), m_matcher(matcher){}
  
  void process(JetIter begin, JetIter end) {
    JetIter cleanEnd = std::partition(begin, end, m_cleaner);
    m_matcher->match(begin, cleanEnd);
  }

  bool pass() const{return m_matcher->pass();}

  std::pair<JetCIter, JetCIter> 
  passedJetIters() const {return m_matcher->passed_iters();}

  std::pair<JetCIter, JetCIter> 
  failedJetIters() const {return m_matcher->failed_iters();}

  std::string toString() const{
    return  "CleanerMatcher start\n" + 
      m_cleaner.toString() + 
      '\n' + m_matcher->toString() +
      "CleanerMatcher end";
  }

  const Conditions& getConditions() const {  //for debug print eg
    return m_matcher->getConditions();
  }

private:
  CleanerBridge m_cleaner;
  std::shared_ptr<IMatcher> m_matcher;
};

#endif
