/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGLHLTJETHYPO_XAODJETCOLLECTOR_H
#define TRIGLHLTJETHYPO_XAODJETCOLLECTOR_H

#include  "xAODJet/Jet.h"
#include  "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include  "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include <vector>
#include <algorithm>
#include <cassert>

// xAODJetCollector - an object send to a (possibly recursive)
// TrigJetHypoToolHelpers to obtain xAOD jets

namespace HypoJet{
  class IJet;
}

class xAODJetCollector {

public:

  void addJets(const HypoJetCIter& begin, const HypoJetCIter& end){
    m_jets.insert(m_jets.end(), begin, end);
  }
  
  std::vector<const xAOD::Jet*> xAODJets() const {
    
    HypoJetVector hypoJets(m_jets.begin(), m_jets.end());
    
    auto new_end =
      std::partition(hypoJets.begin(),
		     hypoJets.end(),
		     [](const HypoJet::IJet* j){
		       return (j->xAODJet()).has_value();});
    // add xAOD::Jet* to m_jets
    std::vector<const xAOD::Jet*> xJets;
    xJets.reserve(new_end - hypoJets.begin());
    std::transform(hypoJets.begin(),
		   new_end,
		   back_inserter(xJets),
		   [](const pHypoJet j){return *(j->xAODJet());});

    std::set<const xAOD::Jet*> js(xJets.begin(), xJets.end());
    return std::vector<const xAOD::Jet*> (js.begin(), js.end());
  }

  
  HypoJetVector hypoJets() const {
    HypoJetSet js(m_jets.begin(), m_jets.end());
    return HypoJetVector(js.begin(), js.end());
  }
  
  void addOneJet(const pHypoJet jet){
    m_jets.push_back(jet);
  }

  std::size_t size() const {return m_jets.size();}
  bool empty() const {return m_jets.empty();}
 
 private:
  HypoJetVector m_jets;

};

#endif
