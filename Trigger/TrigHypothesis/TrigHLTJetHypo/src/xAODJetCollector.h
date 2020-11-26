/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGLHLTJETHYPO_XAODJETCOLLECTOR_H
#define TRIGLHLTJETHYPO_XAODJETCOLLECTOR_H

#include  "xAODJet/Jet.h"
#include  "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

// xAODJetCollector - an object send to a (possibly recursive)
// TrigJetHypoToolHelpers to obtain xAOD jets


  
class xAODJetCollector {

public:

  void addJets(const HypoJetCIter& begin,
	       const HypoJetCIter& end,
	       const std::string& label=""){
    auto& jets = m_jets.at(label);
    jets.insert(jets.end(), begin, end);
  }
  
  std::vector<const xAOD::Jet*> xAODJets() const {
    
    HypoJetVector all;
    for (const auto& p : m_jets){
      std::copy(p.second.cbegin(),
		p.second.cend(),
		std::back_inserter(all)
		);
    }
    return xAODJets_(all.cbegin(), all.cend());
  }

  std::vector<const xAOD::Jet*> xAODJets(const std::string& label) const {
    
    const auto& jets = m_jets.at(label);
    return xAODJets_(jets.cbegin(), jets.cend());
  }
  

  
  HypoJetVector hypoJets() const {
    HypoJetVector all;
    for (const auto& p : m_jets){
      std::copy(p.second.cbegin(),
		p.second.cend(),
		std::back_inserter(all)
		);
    }
    HypoJetSet js(all.begin(), all.end());
    return HypoJetVector(js.begin(), js.end());
  }
  
  void addOneJet(const pHypoJet jet, const std::string& label=""){
    m_jets[label].push_back(jet);
  }

  std::size_t size() const {return m_jets.size();}
  bool empty() const {return m_jets.empty();}
 
 private:

  std::map<std::string, HypoJetVector> m_jets;

    
  std::vector<const xAOD::Jet*>
  xAODJets_(const HypoJetVector::const_iterator begin,
	    const HypoJetVector::const_iterator end
	    )  const {
    
    
    HypoJetVector hypoJets(begin, end);
    
    auto new_end =
      std::partition(hypoJets.begin(),
		     hypoJets.end(),
		     [](const pHypoJet& j){
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
  
  
};

#endif
