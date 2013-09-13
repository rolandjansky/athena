/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P2_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P2_H

#include "DataModelAthenaPool/ElementLink_p1.h"

class CombinedMuonFeature_p2 {
  friend class CombinedMuonFeatureCnv_p2;
    
 public:
    
  // default constructor
  CombinedMuonFeature_p2() { }
  
  // default destructor
  virtual ~CombinedMuonFeature_p2() { }

  float m_pt;
  float m_sigma_pt;
  ElementLinkInt_p1  m_muFastTrack;
  ElementLinkInt_p1  m_IDTrack;
};

#endif
