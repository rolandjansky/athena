/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURE_P1_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURE_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

class TileTrackMuFeature_p1 {
  friend class TileTrackMuFeatureCnv_p1;
    
 public:
    
  // default constructor
  TileTrackMuFeature_p1() { }
  
  // default destructor
  virtual ~TileTrackMuFeature_p1() { }

  float m_PtTR_Trk;
  float m_EtaTR_Trk;
  float m_PhiTR_Trk;
  int   m_Typ_IDTrk;

  TPObjRef m_TileMu; 
  TPObjRef m_Track;

};

#endif
