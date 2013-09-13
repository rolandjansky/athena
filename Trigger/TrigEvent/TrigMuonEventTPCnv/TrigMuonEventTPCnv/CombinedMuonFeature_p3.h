/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P3_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P3_H

#include "DataModelAthenaPool/ElementLink_p3.h"

class CombinedMuonFeature_p3 {
  friend class CombinedMuonFeatureCnv_p3;
    
 public:
    
  // default constructor
  CombinedMuonFeature_p3() { }
  
  // default destructor
  virtual ~CombinedMuonFeature_p3() { }

  float m_pt;
  float m_sigma_pt;
  ElementLinkInt_p3  m_muFastTrack;
  ElementLinkInt_p3  m_IDTrack;
};

#endif
