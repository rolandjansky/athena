/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P4_H
#define TRIGMUONEVENTTPCNV_COMBINEDMUONFEATURE_P4_H

#include "DataModelAthenaPool/ElementLink_p3.h"

class CombinedMuonFeature_p4 {
  friend class CombinedMuonFeatureCnv_p4;
    
 public:
    
  // default constructor
  CombinedMuonFeature_p4() { }
  
  // default destructor
  virtual ~CombinedMuonFeature_p4() { }

  float m_pt;
  float m_sigma_pt;
  float m_charge;
  int   m_flag;
  ElementLinkInt_p3  m_muFastTrack;
  ElementLinkInt_p3  m_IDTrack;
};

#endif
