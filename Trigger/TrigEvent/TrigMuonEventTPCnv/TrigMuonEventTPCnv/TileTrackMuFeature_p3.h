/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_TILETRACKMUFEATURE_P3_H
#define TRIGMUONEVENTTPCNV_TILETRACKMUFEATURE_P3_H

#include "DataModelAthenaPool/ElementLink_p3.h"

class TileTrackMuFeature_p3 {
  friend class TileTrackMuFeatureCnv_p3;
    
 public:
    
  TileTrackMuFeature_p3() : m_Typ_IDTrk(0) { }
  ~TileTrackMuFeature_p3() { }

 private:

  float m_allTheFloats[3];
  //float m_PtTR_Trk;
  //float m_EtaTR_Trk;
  //float m_PhiTR_Trk;
  int   m_Typ_IDTrk;

  ElementLinkInt_p3 m_TileMu;
  ElementLinkInt_p3 m_Track;
};

#endif
