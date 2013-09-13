/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_ISOMUONFEATURE_TLP1_H
#define TRIGMUONEVENTTPCNV_ISOMUONFEATURE_TLP1_H

#include <stdint.h>
#include <string>
#include "TrigMuonEventTPCnv/IsoMuonFeature_p1.h"


class IsoMuonFeature_tlp1 {

 public:
  IsoMuonFeature_tlp1() {}
 

  std::vector<IsoMuonFeature_p1>      m_isoMu;

};

#endif
