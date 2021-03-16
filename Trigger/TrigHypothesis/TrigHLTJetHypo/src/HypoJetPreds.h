/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_HYPOJETPREDS_H
#define TRIGHLTJETHYPO_HYPOJETPREDS_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"


struct HypoJetPtLess{
  bool operator() (const HypoJetVector::value_type& l,
                   const HypoJetVector::value_type& r) const {
    return l->pt() < r->pt();
  }
};


struct HypoJetPtGreater{
  bool operator() (const HypoJetVector::value_type& l,
                   const HypoJetVector::value_type& r) const {
    return l->pt() > r->pt();
  }
};


#endif
