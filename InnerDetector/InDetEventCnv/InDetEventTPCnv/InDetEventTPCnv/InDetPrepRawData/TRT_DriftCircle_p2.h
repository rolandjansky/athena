/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_P2_TRK_H
#define TRT_DRIFT_CIRCLE_P2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TRT_DriftCircle_p2.h
//
//-----------------------------------------------------------------------------

// #include "TrkEventTPCnv/TrkPrepRawData/PrepRawData_p2.h"

namespace InDet
{
   class TRT_DriftCircle_p2
   {
  public:
     TRT_DriftCircle_p2() {}

     // base PrepRawData_p1
     // Trk::PrepRawData_p2 m_rawData;
      
//     unsigned int m_id;
     float m_locPos;
     float m_errMat;
     unsigned int m_TRTdata; // the raw data word from the TRT
     
   };
}

#endif // TRT_DRIFT_CIRCLE_P2_TRK_H
