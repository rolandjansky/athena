/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./AcceptAllConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

AcceptAllConditionMT::AcceptAllConditionMT(std::size_t capacity) :

m_capacity(capacity){
}


bool 
AcceptAllConditionMT::isSatisfied(const HypoJetVector&,
                              const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {
  return true;
}



std::string AcceptAllConditionMT::toString() const noexcept {
  std::stringstream ss;
  
  ss << "AcceptAllConditionMT (" << this << ") capacity " <<  m_capacity <<'\n';
  return ss.str();
}
