/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     OrderedCollectionsMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <queue>
#include <sstream>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/OrderedCollectionsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DescendingEt.h"

OrderedCollectionsMatcher::OrderedCollectionsMatcher():
  Matcher("defaultName"), m_conditions{},m_pass(false), m_passed_jets{},
  m_failed_jets{}{}

OrderedCollectionsMatcher::OrderedCollectionsMatcher(const Conditions& cs,
						     const std::string& name):
  Matcher(name), m_conditions(cs),m_pass(false){
  ConditionsSorter sorterPred;

  if(not std::is_sorted(m_conditions.begin(), m_conditions.end(), sorterPred))
    {
      std::sort(m_conditions.begin(), m_conditions.end(), sorterPred);
    }
}
  
void OrderedCollectionsMatcher::match(const HypoJetIter& jets_b, 
				      const HypoJetIter& jets_e){
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();
  if (jets_b == jets_e){
    m_pass = false;
    return;
  }
  
  m_pass = true;
  
  // sort jets by descending Et
  std::sort (jets_b, jets_e, DescendingEt());

  std::queue<Conditions::value_type>  c_q;
  for(auto c : m_conditions){c_q.push(c);}

  HypoJetQueue j_q;
  for(auto c = jets_b; c != jets_e; ++c){j_q.push(*c);}

  while (!c_q.empty()){
    if (j_q.empty()){break;}
    auto jet = j_q.front();
    HypoJetVector v{jet}; // load the next jet into a container
    if (c_q.front().isSatisfied(v)){
      m_passed_jets.push_back(jet);
      c_q.pop();
    } else {
      m_failed_jets.push_back(jet);
    }
    j_q.pop();
  }

  /* to pass , all conditions need to have been matched */

  if (!c_q.empty()){m_pass = false;}  
}

bool OrderedCollectionsMatcher::pass() const {return m_pass;}

const HypoJetVector& 
OrderedCollectionsMatcher::passedJets() const noexcept {return m_passed_jets;}

const HypoJetVector& 
OrderedCollectionsMatcher::failedJets() const noexcept {return  m_failed_jets;}


std::string OrderedCollectionsMatcher::toString() const noexcept {
  std::stringstream ss;
  ss <<  "OrderedCollectionsMatcher/" + Matcher::toString() + '\n';
  for(auto c : m_conditions){ ss << c.toString() << '\n';}


  ss << "Passed jets [" << m_passed_jets.size() << "]\n";
  for (auto j : m_passed_jets) {ss << j->toString() << '\n';}

  ss << "Failed jets [" << m_failed_jets.size() << "]\n";
  for (auto j : m_failed_jets) {ss << j->toString() << '\n';}

  ss << "Pass:" << std::boolalpha << m_pass << '\n';

  return ss.str();
}


const Conditions& OrderedCollectionsMatcher::getConditions() const noexcept {
  return m_conditions;
}
