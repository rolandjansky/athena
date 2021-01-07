/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     IndexedJetsGrouper.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// Selects jets at given positions from a vector of jets sorted by
// descending Et
// ********************************************************************
//

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <sstream>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IndexedJetsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DescendingEt.h"


IndexedJetsGrouper::IndexedJetsGrouper(const std::vector<unsigned int>& inds):
  m_indices(inds){
}

IndexedJetsGrouper::IndexedJetsGrouper(const std::vector<unsigned int>& indices,
				       const HypoJetVector& jets):
  m_indices(indices), m_jets(jets){
    
  //sort the indices because the last one will be used to 
  // check there are enough jets to process;
  std::sort(m_indices.begin(), m_indices.end());
}

IndexedJetsGrouper::IndexedJetsGrouper(const std::vector<unsigned int>& indices,
				       const HypoJetCIter& b,
				       const HypoJetCIter& e):
  m_indices(indices), m_jets(b, e){
    
  //sort the indices because the last one will be used to 
  // check there are enough jets to process;
  std::sort(m_indices.begin(), m_indices.end());
}


std::vector<HypoJetGroupVector>
IndexedJetsGrouper::group(HypoJetIter& begin, HypoJetIter& end) const{
  

  if (m_indices.empty()) {return std::vector<HypoJetGroupVector>{};}

  // check if there are enough jets find the highest (last, as the vector is
  // ordered) and see if it lies within the jet vector
  
  auto hjgv = HypoJetGroupVector();
  auto result = std::vector<HypoJetGroupVector>();
  auto last_jet_pos =  m_indices.back();
  if(end - begin - 1 < last_jet_pos){return result;}
  
  // sort jets by descending Et
  std::partial_sort (begin,
                     begin + last_jet_pos + 1,
                     end,
                     DescendingEt());
  
  // place the jets at positions in the index vector into the inner vector
  HypoJetVector inner;
  for (auto i : m_indices){inner.push_back(*(begin + i));}
  
  // push the inner vector into the outer vector
  hjgv.push_back(inner);
  result.push_back(hjgv);
  return result;
}


std::optional<HypoJetVector>
IndexedJetsGrouper::next(){

  // exhausts after a single group
  
  // check if there are enough jets find the highest (last, as the vector is
  // ordered) and see if it lies within the jet vector

  if (m_done) { return std::optional<HypoJetVector>(); }
  if (m_indices.empty()) { return std::optional<HypoJetVector>(); }
  
  auto last_jet_pos =  m_indices.back();
  if (m_jets.size() <= last_jet_pos) {
    m_done = true;
    return std::optional<HypoJetVector>();
  }
  
  // sort jets by descending Et
  std::partial_sort (m_jets.begin(),
                     m_jets.begin() + last_jet_pos + 1,
                     m_jets.end(),
                     DescendingEt());
  
  // place the jets at positions in the index vector into the inner vector
  HypoJetVector hjv;
  for (auto i : m_indices){hjv.push_back(*(m_jets.begin() + i));}
  
  // push the inner vector into the outer vector
  m_done = true;
  return std::make_optional<HypoJetVector>(hjv);
}


std::string IndexedJetsGrouper::getName() const {
  return "IndexedJetsGrouper";
}

std::string IndexedJetsGrouper::toString() const {
  std::stringstream ss;
  ss << "IndexedJetsGrouper - select from jets ordered in descending Et "
     << " in positions: ";
  for(auto i : m_indices){ ss << i << " ";}
  ss << '\n';
  return ss.str();
}

  
