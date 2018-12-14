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

IndexedJetsGrouper::IndexedJetsGrouper(const std::vector<unsigned int>& indices):
  m_indices(indices){
  if (indices.empty()) {
    std::string m = "IndexedJetsGrouper: Attempt to iniatialize with an ";
    m += "empty indices vector";
    throw std::out_of_range(m); 
  }
    
  //sort the indices because the last one will be used to 
  // check there are enough jets to process;
  std::sort(m_indices.begin(), m_indices.end());
}


HypoJetGroupVector IndexedJetsGrouper::group(HypoJetIter& begin,
                                             HypoJetIter& end) const{
  
  // check if there are enough jets find the highest (last, as the vector is
  // ordered) and see if it lies within the jet vector
  
  auto result = HypoJetGroupVector();
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
  result.push_back(inner);
  return result;
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

  
