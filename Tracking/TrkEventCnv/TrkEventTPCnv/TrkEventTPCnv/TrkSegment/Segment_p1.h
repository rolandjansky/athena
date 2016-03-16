/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Segment_p1.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKSEGMENT_SEGMENT_P1_H
#define TRKSEGMENT_SEGMENT_P1_H

//-----------------------------------------------------------------------------
//
// file:   Segment_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"
#include <inttypes.h>

namespace Trk {

  class Segment_p1 {
    
    public:
      Segment_p1() : m_author(0) {};
      // : public MeasurementBase_p1
      
      TPObjRef          m_localParameters;  // mutable const LocalParameters*

      TPObjRef          m_localErrorMatrix; // mutable const ErrorMatrix*

      TPObjRef          m_fitQuality;       // FitQuality*

      std::vector< TPObjRef > m_containedMeasBases;   // for DataVector<const MeasurementBase>*    m_containedMeasBases;
     
      uint16_t          m_author;
};
}

#endif // TRKSEGMENT_SEGMENT_P1_H


