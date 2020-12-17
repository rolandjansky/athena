
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"

AllJetsGrouper:: AllJetsGrouper(){}

AllJetsGrouper:: AllJetsGrouper(const HypoJetIter& b,
				const HypoJetIter& e): m_jets(b, e){
}

AllJetsGrouper:: AllJetsGrouper(const HypoJetVector& jets): m_jets{jets}{
}



std::vector<HypoJetGroupVector> AllJetsGrouper::group(HypoJetIter& begin,
						      HypoJetIter& end
						      ) const {
  HypoJetGroupVector hjgv{HypoJetVector(begin, end)};
  return std::vector<HypoJetGroupVector>{hjgv};
}

std::optional<HypoJetGroupVector> AllJetsGrouper::next(){
  if (m_done){
    return std::optional<HypoJetGroupVector>();
  }
  
  HypoJetGroupVector hjgv{HypoJetVector(m_jets)};
  m_done = true;
  return std::make_optional<HypoJetGroupVector>(hjgv);
}

std::string AllJetsGrouper::getName() const {
  return "AllJetsGrouper";
}

std::string AllJetsGrouper::toString() const {
  return "AllJetsGrouper - repack all input jets into vector of vectors, sole element is vector of all jets";
}


