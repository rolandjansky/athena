/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     SingleEtaRegionMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "./SingleEtaRegionMatcher.h"
#include "xAODJet/Jet.h"
#include "./DescendingEt.h"
#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h"

SingleEtaRegionMatcher::SingleEtaRegionMatcher(const Conditions& cs):m_conditions(cs), m_pass(false){
  ConditionsSorter sorterPred;

  if(not std::is_sorted(m_conditions.begin(), m_conditions.end(), sorterPred))
    {
      std::sort(m_conditions.begin(), m_conditions.end(), sorterPred);
    }
  
  Condition first = m_conditions[0];
  for (auto c : m_conditions) {
    if (c.etaMin() != first.etaMin()){
      throw std::runtime_error("more than 1 eta region");
    }
    if (c.etaMax() != first.etaMax()){
      throw std::runtime_error("more than 1 eta region");
    }
  }
}
  
void SingleEtaRegionMatcher::match(JetIter b, JetIter e){
  // sort jets by descending eta
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();
  if (b == e){
    m_pass = false;
    return;
  }
  
  m_pass = true;
  
  // select jets in the eta range - which is common for all conditions
  // caller guarantees m_conditions.size() > 0 
  const Condition& c = m_conditions[0];

  e = std::partition(b, e, [&](const xAOD::Jet* j){return c.etaOK(j);});
  
  int n_conds = m_conditions.size();
  int n_jets = e - b;
  
  bool not_enough_jets = n_jets < n_conds;

  /* continue processing even if the number of jets is less
     than the number of conditions to allow debug of good and bad jets */
  if (not_enough_jets){
    std::sort (b, e, DescendingEt());
    }else{
    /* Only the first n_conds jets from the ordered jet vector
       can contribute to the success of the event. Copy the rest to 
       the bad jet vector */
    std::partial_sort (b, b + n_conds, e, DescendingEt());
    m_failed_jets.assign(b + n_conds,  e);
    }
  
  // zip the conditions and jet collection iterators
  // with length being the shorter of the two collections.
  using IterPair = std::pair<ConditionsIter, JetCIter>;
  
  std::vector<IterPair> zip;
  std::size_t zipLen = std::min(n_conds, n_jets);

  for(std::size_t i = 0; i != zipLen; ++i) {
    zip.push_back(IterPair(m_conditions.begin() + i, b + i));}
  
  /* while there are are more jet-condition pairs, check whether
     the jet matches the condition. If it does, and there has
     been no previous failure, add it to the good jets vector.
     Otherwise add it to the bad jets vector */

  for(auto iters : zip){
    auto ci = iters.first; auto ji = iters.second;
    if(m_pass){

      if (ci->allOK((*ji))){m_passed_jets.push_back(*ji);
      } else {
        m_pass = false;
        m_failed_jets.push_back(*ji);
      }

    } else {
      m_failed_jets.push_back(*ji);
    }
  }

  /* to pass , all thresholds in zipped jets - conditions need to pass
     and there have to be enough jets */
  if (not_enough_jets){m_pass = false;}  
}

bool SingleEtaRegionMatcher::pass() const {return m_pass;}

std::pair<JetCIter, JetCIter> SingleEtaRegionMatcher::passed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_passed_jets.begin(),
                                         m_passed_jets.end());}

std::pair<JetCIter, JetCIter> SingleEtaRegionMatcher::failed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_failed_jets.begin(),
                                         m_failed_jets.end());}

