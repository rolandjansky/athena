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
	       const std::string& label="");
  
  std::vector<const xAOD::Jet*> xAODJets() const;
    
  std::vector<const xAOD::Jet*> xAODJets(const std::string& label) const;
  
  HypoJetVector hypoJets() const;
  HypoJetVector hypoJets(const std::string& label) const;

  void addOneJet(const pHypoJet jet, const std::string& label="");

  std::size_t size() const;
  bool empty() const;


  std::vector<std::string> legLabels() const;
  
 private:

  std::map<std::string, HypoJetVector> m_jets;

    
  std::vector<const xAOD::Jet*>

  xAODJets_(const HypoJetVector::const_iterator begin,
	    const HypoJetVector::const_iterator end
	    )  const;
    
};

#endif
