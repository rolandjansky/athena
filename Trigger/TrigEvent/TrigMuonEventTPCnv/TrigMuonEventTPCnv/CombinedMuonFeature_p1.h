/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P1_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

class CombinedMuonFeature_p1 {
  friend class CombinedMuonFeatureCnv_p1;
    
 public:
    
  // default constructor
  CombinedMuonFeature_p1() { }
  
  // default destructor
  virtual ~CombinedMuonFeature_p1() { }

  float m_pt;
  float m_sigma_pt;
  TPObjRef  m_muFastTrack;
  TPObjRef  m_IDTrack;
};

#endif
