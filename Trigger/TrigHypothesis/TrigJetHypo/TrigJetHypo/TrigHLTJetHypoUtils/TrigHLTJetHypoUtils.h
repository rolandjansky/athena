/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_TRIGHLTJETHYPOUTILS_H
#define TRIGJETHYPO_TRIGHLTJETHYPOUTILS_H
/********************************************************************
 *
 * NAME:     TrigHLTJetHypo.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

/* WORK IN PROGRESS _ DO NOT USE ! */
// #include "TrigJetHypo/TrigHLTJetHypo.h"
#include "xAODJet/Jet.h"

using JetVector = std::vector<const xAOD::Jet*>;
using JetIter = JetVector::iterator;
using JetCIter = JetVector::const_iterator;


//////////////////////////////////////

/*
class ICleanerMatcher{
  // Abstract interface used by a factory function
 public:
  virtual void process(JetIter, JetIter) = 0;
  virtual bool pass() const = 0;
  virtual std::pair<JetCIter, JetCIter> passedJetIters() const = 0;
  virtual std::pair<JetCIter, JetCIter> failedJetIters() const = 0;
};
*/
//////////////////////////////////////

class ICleaner{
 public:
  // Abstract interface used by a factory function
  virtual bool operator()(const xAOD::Jet* jet) const = 0;
};

//////////////////////////////////////

class IMatcher{
 public:
// Abstract interface used by a factory function
  virtual void match(JetIter b, JetIter e) = 0;
  virtual bool pass() const = 0;
  virtual std::pair<JetCIter, JetCIter> passed_iters() const = 0;
  virtual std::pair<JetCIter, JetCIter> failed_iters() const = 0;
};

//////////////////////////////////////
/*
class MatcherBridge{
  // Bridge object: convert polymorphic pointer to monomorthic class
  // can be passed to STL algs.
 public:
 MatcherBridge(const std::shared_ptr<IMatcher>& matcher):
  m_pMatcher(matcher){}
  
  bool match(JetIter b, JetIter e) const {
    return m_pMatcher->match(b, e);
  }
  bool pass() const{return m_pMatcher->pass();}
  
  std::pair<JetCIter, JetCIter> passed_iters() const{
    return m_pMatcher->passed_iters();}
  
  std::pair<JetCIter, JetCIter> failed_iters() const{
    return m_pMatcher->failed_iters();}

 private:
  std::shared_ptr<IMatcher> m_pMatcher;

};
*/

//////////////////////////////////////
class CleanerBridge{
  // Bridge object: convert polymorphic pointer to monomorthic class
  // can be passed to STL algs.
 public:
 CleanerBridge(const std::shared_ptr<ICleaner>& cleaner):
  m_pCleaner(cleaner){}

  bool operator()(const xAOD::Jet* jet) const{
    return m_pCleaner -> operator()(jet);
  }

 private:
  std::shared_ptr<ICleaner> m_pCleaner;
};

//////////////////////////////////////

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
  
private:
  CleanerBridge m_cleaner;
  std::shared_ptr<IMatcher> m_matcher;
};

// Define a class to hold etaMin, etaMax and threshold (Et in MeV)
// that jets will be matched against
class Condition{
 public:
  Condition(double etaMin, double etaMax, double threshold);

  bool etaOK(const xAOD::IParticle* ip) const noexcept;

  bool etOK(const xAOD::IParticle* ip) const noexcept;
  bool allOK(const xAOD::IParticle* ip) const noexcept;

  double etaMin() const noexcept;
  double etaMax() const noexcept;
  double threshold() const noexcept;

 private:
  double m_etaMin;
  double m_etaMax;
  double m_threshold;
};

std::ostream& operator<< (std::ostream& os, const Condition& c);

using Conditions = std::vector<Condition>;
using ConditionsIter = std::vector<Condition>::iterator;

struct ConditionsSorter{
  bool operator()(const Condition& l, const Condition& r);
};

#endif

