/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include  "xAODJetCollector.h"

#include <algorithm>
#include <cassert>

void xAODJetCollector::addJets(const HypoJetCIter& begin,
			       const HypoJetCIter& end,
			       int chainPartInd){
  auto& jets = m_jets[chainPartInd];
  jets.insert(jets.end(), begin, end);
}
  

std::vector<const xAOD::Jet*> xAODJetCollector::xAODJets() const {
  
  HypoJetVector all;
  for (const auto& p : m_jets){
    std::copy(p.second.cbegin(),
	      p.second.cend(),
	      std::back_inserter(all)
	      );
  }
  return xAODJets_(all.cbegin(), all.cend());
}


std::vector<const xAOD::Jet*>
xAODJetCollector::xAODJets(int chainPartInd) const {
  
  if (m_jets.count(chainPartInd) == 0){
    std::vector<const xAOD::Jet*> empty;
    return empty;
  }
  
  const auto& jets = m_jets.at(chainPartInd);
  return xAODJets_(jets.cbegin(), jets.cend());
}

  
HypoJetVector xAODJetCollector::hypoJets() const {
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

  

HypoJetVector xAODJetCollector::hypoJets(int chainPartInd) const {
  auto begin = m_jets.at(chainPartInd).cbegin();
  auto end = m_jets.at(chainPartInd).cend();
  HypoJetSet js(begin, end);
  return HypoJetVector(js.begin(), js.end());
}


void xAODJetCollector::addOneJet(const pHypoJet jet,

				 int chainPartInd){
  m_jets[chainPartInd].push_back(jet);
}


std::size_t xAODJetCollector::size() const {return hypoJets().size();}

bool xAODJetCollector::empty() const {return hypoJets().empty();}

std::vector<const xAOD::Jet*>
xAODJetCollector::xAODJets_(const HypoJetVector::const_iterator begin,
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


std::vector<int>  xAODJetCollector::legInds() const {
  std::vector<int> legInds;

  for(auto it = m_jets.begin(); it != m_jets.end(); ++it){
    legInds.push_back(it->first);
  }

  return legInds;
}
