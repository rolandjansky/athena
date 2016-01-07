/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     SelectedJetsMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
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

#include "TrigJetHypo/TrigHLTJetHypoUtils/SelectedJetsMatcher.h"
#include "xAODJet/Jet.h"
#include "./DescendingEt.h"

SelectedJetsMatcher::SelectedJetsMatcher(const Conditions& cs,
					 const std::vector<unsigned int>& indices,
					 const std::string& name
					 ):
  Matcher(name), m_conditions(cs), m_indices(indices), m_pass(false){
  
  //sort the indices because the last one will be used to 
  // check there are enough jets to process;
  std::sort(m_indices.begin(), m_indices.end());
}

  
void SelectedJetsMatcher::match(JetIter b, JetIter e){
  
  m_passed_jets.clear(); // allow monitoring of the jets that pass conditions.
  m_failed_jets.clear();
  if (b == e){
    m_pass = false;
    return;
  }

  // check if there are enough jets find the highest (last, as the vector is
  // ordered) and see if it lies within the jet vector
  if((e - b -1) < m_indices.back()){
    unsigned int i = -1;
    while (b + (++i) < e) {m_failed_jets.push_back(*(b+i));}
    m_pass = false;
    return;
  }


  // sort jets by descending eta
  std::partial_sort (b, b + m_indices.size(),  e, DescendingEt());

  std::vector<const xAOD::IParticle*> selected_jets;
  for (auto i : m_indices){selected_jets.push_back(*(b+i));}
  
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
    m_passed_jets.insert(m_passed_jets.end(), b, e);
  } else {
    m_failed_jets.insert(m_failed_jets.end(), b, e);
  }
}


bool SelectedJetsMatcher::pass() const {return m_pass;}

std::pair<JetCIter, JetCIter> SelectedJetsMatcher::passed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_passed_jets.begin(),
                                         m_passed_jets.end());}

std::pair<JetCIter, JetCIter> SelectedJetsMatcher::failed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_failed_jets.begin(),
                                         m_failed_jets.end());}

std::string SelectedJetsMatcher::toString() const noexcept{
  std::stringstream ss;
  ss <<  Matcher::toString();
  for(auto c : m_conditions){ ss << "\n" <<  c.toString();}
  ss << "\n indices[" << m_indices.size() <<"]: ";
  for(auto i : m_indices){ss << i << " ";}
  return ss.str();
}


const Conditions& SelectedJetsMatcher::getConditions() const noexcept {
  return m_conditions;
}
