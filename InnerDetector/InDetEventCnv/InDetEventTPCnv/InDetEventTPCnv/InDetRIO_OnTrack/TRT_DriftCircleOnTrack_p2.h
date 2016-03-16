/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_DRIFT_CIRCLE_ON_TRACK_p2_TRK_H
#define TRT_DRIFT_CIRCLE_ON_TRACK_p2_TRK_H

#include "AthenaPoolUtilities/TPObjRef.h"

#include "Identifier/IdentifierHash.h"

#include "DataModelAthenaPool/ElementLink_p1.h"
//#include "DataModelAthenaPool/ElementLinkContNames_p2.h"

namespace InDet
{
    class TRT_DriftCircleOnTrack_p2
    {
        public:
      TRT_DriftCircleOnTrack_p2() {}
      
      unsigned int                  m_id;
      TPObjRef                      m_localParams;
      TPObjRef                      m_localErrMat;
            
      IdentifierHash::value_type    m_idDE;
      unsigned int                  m_status;
      bool                          m_highLevel;
      float                         m_timeOverThreshold;
      
      float                         m_localAngle;
      float                         m_positionAlongWire;
      
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
    };
}

#endif 
