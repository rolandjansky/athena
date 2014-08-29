/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDT_DRIFT_CIRCLE_ON_TRACK_P1_TRK_H
#define MDT_DRIFT_CIRCLE_ON_TRACK_P1_TRK_H

//-----------------------------------------------------------------------------
//
// file:   MdtDriftCircleOnTrack_p1.h
//
//-----------------------------------------------------------------------------

#include "AthenaPoolUtilities/TPObjRef.h"

#include "Identifier/IdentifierHash.h"
#include "DataModelAthenaPool/ElementLink_p1.h"

namespace Muon
{
    /** Version 1 of the persistent class representing the transient class Muon::MdtDriftCircleOnTrack */
    class MdtDriftCircleOnTrack_p1
    {
        public:
      MdtDriftCircleOnTrack_p1() : 
        m_status(0), m_localAngle(0.), m_positionAlongWire(0.), m_driftTime(0.), m_rotCreationParameters(0) 
      {}

      //IdentifierHash::value_type    m_idDE;

      /** Status of Mdt - in transient world this is the Trk::DriftCircleStatus enum*/
      unsigned int                  m_status;
      
      /** base RIO_OnTrack_p1*/
      TPObjRef                      m_RIO;
      
      /** Link to StraightLineSurface*/
      TPObjRef                      m_saggedSurface;
      
      /** Link to PRD */
      ElementLinkInt_p1  m_prdLink;
      
      /** This angle is the position of the point of closest approach in cylindrical coordinates, and is needed to construct the global position*/
      float                         m_localAngle;
    
      /** This is the position of the point of closest approach, in the local z coord (i.e. along the wire), and is needed to construct the global position*/
      float                         m_positionAlongWire;

      /** This is the drift time that was used to calibrate the hit */
      float                         m_driftTime;

      /** Records information about the 'strategy' used by Muon::MdtDriftCircleOnTrackCreator to make this object. */
      unsigned int 	            m_rotCreationParameters;
    };
}

#endif // MdtDRIFT_CIRCLE_ON_TRACK_P1_TRK_H
