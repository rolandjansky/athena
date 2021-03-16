/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"

AllJetsGrouper:: AllJetsGrouper(){}

AllJetsGrouper:: AllJetsGrouper(const HypoJetCIter& b,
				const HypoJetCIter& e): m_jets(b, e){
}

AllJetsGrouper:: AllJetsGrouper(const HypoJetVector& jets): m_jets{jets}{
}



HypoJetVector AllJetsGrouper::next(){
  if (m_done) {return HypoJetVector{};}
  
  m_done = true;
  return HypoJetVector{m_jets};
}

std::string AllJetsGrouper::getName() const {
  return "AllJetsGrouper";
}

std::string AllJetsGrouper::toString() const {
  return "AllJetsGrouper - repack all input jets into vector of vectors, sole element is vector of all jets";
}


