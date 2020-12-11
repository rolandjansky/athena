/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGLHLTJETHYPO_XAODJETCOLLECTOR_H
#define TRIGLHLTJETHYPO_XAODJETCOLLECTOR_H

#include  "xAODJet/Jet.h"
#include  "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include <string>
#include <vector>
#include <map>

// xAODJetCollector - an object send to a (possibly recursive)
// TrigJetHypoToolHelpers to obtain xAOD jets

using CI = std::map<std::string, HypoJetVector>::const_iterator;

class xAODJetCollector {

public:

  void addJets(const HypoJetCIter& begin,
	       const HypoJetCIter& end,
	       int chainPartInd = -1);
  
  std::vector<const xAOD::Jet*> xAODJets() const;
    
  std::vector<const xAOD::Jet*> xAODJets(int chainPartInd) const;
  
  HypoJetVector hypoJets() const;
  HypoJetVector hypoJets(int chainPartInd) const;

  void addOneJet(const pHypoJet jet, int ind = -1);


  std::size_t size() const;
  bool empty() const;

  std::vector<int> legInds() const;
  
 private:

  // store passing jets by chainPart index. this defaults to -1
  // only leaf nodes have indices >= 0
  std::map<int, HypoJetVector> m_jets;

    
  std::vector<const xAOD::Jet*>

  xAODJets_(const HypoJetVector::const_iterator begin,
	    const HypoJetVector::const_iterator end
	    )  const;
    
};

#endif
