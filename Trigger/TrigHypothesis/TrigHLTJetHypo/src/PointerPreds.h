/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_POINTERPREDS_H
#define TRIGHLTJETHYPO_POINTERPREDS_H

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

struct PtrLess {
  bool operator()(const pHypoJet& l, const pHypoJet& r){
    return l.get() < r.get();
  }
};


struct PtrEq {
  bool operator() (const pHypoJet& l, const pHypoJet& r){
    return l.get() == r.get();
  }
};

#endif
