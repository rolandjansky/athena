/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     OrderedCollectionsMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <queue>

#include "TrigJetHypo/TrigHLTJetHypoUtils/OrderedCollectionsMatcher.h"
#include "xAODJet/Jet.h"
#include "./DescendingEt.h"

OrderedCollectionsMatcher::OrderedCollectionsMatcher(const Conditions& cs,
						     const std::string& name):
  Matcher(name), m_conditions(cs),m_pass(false){
  ConditionsSorter sorterPred;

  if(not std::is_sorted(m_conditions.begin(), m_conditions.end(), sorterPred))
    {
      std::sort(m_conditions.begin(), m_conditions.end(), sorterPred);
    }
}
  
void OrderedCollectionsMatcher::match(JetIter b, JetIter e){
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();
  if (b == e){
    m_pass = false;
    return;
  }
  
  m_pass = true;
  
  // sort jets by descending eta
  std::sort (b, e, DescendingEt());

  std::queue<Conditions::value_type>  c_q;


  std::queue<const xAOD::Jet*>  j_q;
  for(auto i = b; i!=e; i++){j_q.push(*i);}

  while (!c_q.empty()){
    if (j_q.empty()){break;}
    auto jet = j_q.front();
    j_q.pop();
    if (c_q.front().isSatisfied(jet)){
      m_passed_jets.push_back(jet);
      c_q.pop();
    } else {
      m_failed_jets.push_back(jet);
    }
  }

  /* to pass , all conditions need to have been matched */

  if (!c_q.empty()){m_pass = false;}  
}

bool OrderedCollectionsMatcher::pass() const {return m_pass;}

std::pair<JetCIter, JetCIter> OrderedCollectionsMatcher::passed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_passed_jets.begin(),
                                         m_passed_jets.end());}

std::pair<JetCIter, JetCIter> OrderedCollectionsMatcher::failed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_failed_jets.begin(),
                                         m_failed_jets.end());}



std::string OrderedCollectionsMatcher::toString() const noexcept {
  auto s = Matcher::toString();
  for(auto c : m_conditions){ s += "\n" + c.toString();}
  return s;
}


const Conditions& OrderedCollectionsMatcher::getConditions() const noexcept {
  return m_conditions;
}
