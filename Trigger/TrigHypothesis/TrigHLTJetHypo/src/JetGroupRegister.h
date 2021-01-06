/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_JETGROUPREGISTER_H
#define TRIGHLTJETHYPO_JETGROUPREGISTER_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include <map>
#include <algorithm>

// maintain a register of ordered vectors HypoJets 

struct HypoJetLess{
  bool operator() (const HypoJetVector::value_type& l,
		   const HypoJetVector::value_type& r) const {
    return l.get() < r.get();
  }
};

struct HypoJetVectorLess{
  bool operator() (const HypoJetVector& l,
		   const HypoJetVector& r) const {
    
    return std::lexicographical_compare(l.begin(),
					l.end(),
					r.begin(),
					r.end(),
					HypoJetLess());
  }
};


class JetGroupRegister {
 public:

  std::size_t record(HypoJetVector v);  // copy vector

 private:
  std::map<HypoJetVector, std::size_t, HypoJetVectorLess> m_reg;
    
};
#endif
