/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     SimpleJetsMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// Passes all jets to all conditions objects.
//
// ********************************************************************
//

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SimpleJetsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DescendingEt.h"

SimpleJetsMatcher::SimpleJetsMatcher(const Conditions& cs,
					 const std::string& name
					 ): Matcher(name), m_conditions(cs), m_pass(false){
}

  
void SimpleJetsMatcher::match(const HypoJetIter& jets_b,
				const HypoJetIter& jets_e){
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();

  HypoJetVector selected_jets(jets_b, jets_e);
  
  //  Hypo passes if at least one condition object accepts the
  // selected jets.
  m_pass = false;
  for(auto c : m_conditions){
    if (c.isSatisfied(selected_jets)){
      m_pass = true;
      break;
    }
  }
  
  if(m_pass){
    m_passed_jets.assign(selected_jets.begin(),
			 selected_jets.end());
  } else {
    m_failed_jets.assign(selected_jets.begin(),
			 selected_jets.end());
  }
}


bool SimpleJetsMatcher::pass() const {return m_pass;}

const HypoJetVector &
SimpleJetsMatcher::passedJets() const  noexcept {return m_passed_jets;}

const HypoJetVector &
SimpleJetsMatcher::failedJets() const noexcept {return m_failed_jets;}

std::string SimpleJetsMatcher::toString() const noexcept{
  std::stringstream ss;
  ss << "SimpleJetsMatcher/" << Matcher::toString() << '\n'
     << "Conditions: \n";
  for(auto c : m_conditions){ ss <<  c.toString() << '\n';}
  ss << '\n';

  ss << "Passed jets [" << m_passed_jets.size() << "]\n";
  for (auto j : m_passed_jets) {ss << j->toString() << '\n';}

  ss << "Failed jets [" << m_failed_jets.size() << "]\n";
  for (auto j : m_failed_jets) {ss << j->toString() << '\n';}

  return ss.str();
}


const Conditions& SimpleJetsMatcher::getConditions() const noexcept {
  return m_conditions;
}
