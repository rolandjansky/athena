/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGLHLTJETHYPO_JETCOLLECTOR_H
#define TRIGLHLTJETHYPO_JETCOLLECTOR_H

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

    HypoJetVector jets(begin, end);

    // determine whether each jet has an associated xAOD::Jet
    // there are implementations of HypoJet where this is not the case.
    
    auto new_end =
      std::partition(jets.begin(),
		     jets.end(),
		     [](const HypoJet::IJet* j){return (j->xAODJet()).has_value();});
    // add xAOD::Jet* to m_jets
    m_jets.reserve(m_jets.size() + (new_end - jets.begin()));
    std::transform(jets.begin(),
		   new_end,
		   std::back_inserter(m_jets),
		   [](const pHypoJet j){return *(j->xAODJet());});
      
  }

  std::vector<const xAOD::Jet*> xAODJets() const {return m_jets;}

  void addOneJet(const pHypoJet jet){
    auto opt_xAODJet = jet -> xAODJet();
    if (opt_xAODJet.has_value()){m_jets.push_back(*opt_xAODJet);}
  }

  std::size_t size() const {return m_jets.size();}
  bool empty() const {return m_jets.empty();}
 
 private:
  std::vector<const xAOD::Jet*> m_jets;
};

#endif
