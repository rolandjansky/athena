/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     SelectedJetsMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// Takes a vector of the indices used to select the jets from
// the total jet collection, and pass them to the Conditions object.
// This matcher was first used for the TLA analysis which
// used TLAConditions objects.
// ********************************************************************
//

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SelectedJetsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DescendingEt.h"

SelectedJetsMatcher::SelectedJetsMatcher(const Conditions& cs,
					 const std::vector<unsigned int>& indices,
					 const std::string& name
					 ):
  Matcher(name), m_conditions(cs), m_indices(indices), m_pass(false){
  
  if (indices.empty()) {
    std::string m = "SelectedJetsMatcher: Attempt to iniatialize with an ";
    m += "empty indices vector";
    throw std::out_of_range(m);
  }


  //sort the indices because the last one will be used to 
  // check there are enough jets to process;
  std::sort(m_indices.begin(), m_indices.end());
}

  
void SelectedJetsMatcher::match(const HypoJetIter& jets_b,
				const HypoJetIter& jets_e){
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();

  // check if there are enough jets find the highest (last, as the vector is
  // ordered) and see if it lies within the jet vector
  auto last_jet_pos =  m_indices.back();
  if(jets_e - jets_b - 1 < last_jet_pos){
    m_failed_jets.assign(jets_b, jets_e);
    m_pass = false;
    return;
  }


  // sort jets by descending Et
  std::partial_sort (jets_b,
		     jets_b + last_jet_pos + 1,
		     jets_e,
		     DescendingEt());

  HypoJetVector selected_jets;
  for (auto i : m_indices){selected_jets.push_back(*(jets_b + i));}
  
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


bool SelectedJetsMatcher::pass() const {return m_pass;}

const HypoJetVector &
SelectedJetsMatcher::passedJets() const  noexcept {return m_passed_jets;}

const HypoJetVector &
SelectedJetsMatcher::failedJets() const noexcept {return m_failed_jets;}

std::string SelectedJetsMatcher::toString() const noexcept{
  std::stringstream ss;
  ss << "SelectedJetsMatcher/" << Matcher::toString() << '\n'
     << "Conditions: \n";
  for(auto c : m_conditions){ ss <<  c.toString() << '\n';}
  ss << "indices[" << m_indices.size() << "] ";
  for(auto i : m_indices){ss << i << " ";}
  ss << '\n';

  ss << "Passed jets [" << m_passed_jets.size() << "]\n";
  for (auto j : m_passed_jets) {ss << j->toString() << '\n';}

  ss << "Failed jets [" << m_failed_jets.size() << "]\n";
  for (auto j : m_failed_jets) {ss << j->toString() << '\n';}

  return ss.str();
}


const Conditions& SelectedJetsMatcher::getConditions() const noexcept {
  return m_conditions;
}
