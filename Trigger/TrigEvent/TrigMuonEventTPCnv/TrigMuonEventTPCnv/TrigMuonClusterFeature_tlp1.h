/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURE_TLP1_H
#define TRIGMUONEVENTTPCNV_TRIGMUONCLUSTERFEATURE_TLP1_H

#include <stdint.h>
#include <string>
#include "TrigMuonEventTPCnv/TrigMuonClusterFeature_p1.h"


class TrigMuonClusterFeature_tlp1 {

 public:
  TrigMuonClusterFeature_tlp1() {}
 

  std::vector<TrigMuonClusterFeature_p1>      m_trigMuCl;

};

#endif
