/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <queue>
#include <sstream>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MultiJetMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGen.h"

MultiJetMatcher::MultiJetMatcher():
  Matcher("defaultName"), m_conditions{}, m_njets(0), 
  m_pass(false), m_passed_jets{}, m_failed_jets{}{}


MultiJetMatcher::MultiJetMatcher(const Conditions& cs,
				 std::size_t njets,
				 const std::string& name):
  Matcher(name), m_conditions(cs), m_njets(njets),
  m_pass(false), m_passed_jets{}, m_failed_jets(){}


void MultiJetMatcher::match(const HypoJetIter& jets_b, 
			    const HypoJetIter& jets_e){
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();
  unsigned int njets_in = jets_e - jets_b;
  if (njets_in < m_njets){
    m_failed_jets.assign(jets_b, jets_e);
    m_pass = false;
    return;
  }
  

  // create a combinations generator. Used to select the jets
  // to be tested by the condition objects
  CombinationsGen combgen(njets_in, m_njets);
  
  // set of jets passing conditions. As a jet may or not be selected,
  // or selected many times, according to the accompanying jets,
  // a set is used to provide uniqueness.
  HypoJetSet selected;

  // test all combinations of jets against all conditions.
  for(auto c : m_conditions){
    while(true){
      auto combs = combgen.next();
      if(!combs.second){break;} // combination indices are  invalid
      
      // copy the combination of jets into a vector and test.
      HypoJetVector candidates;
      for(auto i : combs.first){candidates.push_back(*(jets_b + i));}
      if(c.isSatisfied(candidates))
	{
	  selected.insert(candidates.begin(), candidates.end());
	}
    }   
  }

  m_pass = (!selected.empty());
  
  if (not m_pass){
    m_failed_jets.assign(jets_b, jets_e);
    return;
  }

  m_passed_jets.assign(selected.begin(), selected.end());

  auto selEnd = selected.end();
  for(auto c= jets_b; c != jets_e; ++c){
    if(selected.find(*c) == selEnd){m_failed_jets.push_back(*c);}
  }
}


bool MultiJetMatcher::pass() const {return m_pass;}


const HypoJetVector& 
MultiJetMatcher::passedJets() const noexcept {return m_passed_jets;}


const HypoJetVector& 
MultiJetMatcher::failedJets() const noexcept {return  m_failed_jets;}


std::string MultiJetMatcher::toString() const noexcept {
  std::stringstream ss;
  ss <<  "MultiJetMatcher/" + Matcher::toString() + '\n';
  for(auto c : m_conditions){ ss << c.toString() << '\n';}


  ss << "Passed jets [" << m_passed_jets.size() << "]\n";
  for (auto j : m_passed_jets) {ss << j->toString() << '\n';}

  ss << "Failed jets [" << m_failed_jets.size() << "]\n";
  for (auto j : m_failed_jets) {ss << j->toString() << '\n';}

  ss << "Pass:" << std::boolalpha << m_pass << '\n';

  return ss.str();
}


const Conditions& MultiJetMatcher::getConditions() const noexcept {
  return m_conditions;
}
