/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"


SingleJetGrouper::SingleJetGrouper(){}

SingleJetGrouper::SingleJetGrouper(const HypoJetVector& v):
  m_jets(v), m_size{v.size()}{
}

SingleJetGrouper::SingleJetGrouper(const HypoJetCIter& b,
				   const HypoJetCIter& e):
  m_jets(b, e), m_size{m_jets.size()}{
}


HypoJetVector SingleJetGrouper::next() {
  if (m_index == m_size){
    return HypoJetVector{};
  }
  
  return HypoJetVector{m_jets[m_index++]};
}

std::string SingleJetGrouper::getName() const {
  return "SingleJetGrouper";
}

std::string SingleJetGrouper::toString() const {
  return "SingleJetGrouper - repack input jets into vectors of length 1";
}


