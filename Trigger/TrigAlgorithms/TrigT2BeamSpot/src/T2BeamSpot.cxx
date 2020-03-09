/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// $Id: T2BeamSpot.cxx 744847 2016-05-04 15:12:00Z smh $
//

#include "T2BeamSpot.h"
#include "BeamSpotConditionsData/BeamSpotData.h"

namespace PESA
{

  std::ostream& operator<<( std::ostream& os, const T2BeamSpot& beamSpot )
  {
    os
      << " status = "   << beamSpot.status()
      << ", x0 = "      << beamSpot.posX() 
      << ", y0 = "      << beamSpot.posY() 
      << ", z0 = "      << beamSpot.posZ() 
      << ", sigmaX = "  << beamSpot.sigmaX()
      << ", sigmaY = "  << beamSpot.sigmaY() 
      << ", sigmaZ = "  << beamSpot.sigmaZ()
      << ", tiltX = "   << beamSpot.tiltX() 
      << ", tiltY = "   << beamSpot.tiltY()
      << ", sigmaXY = " << beamSpot.sigmaXY()
      ;
    return os;
  }

  T2BeamSpot::T2BeamSpot( const InDet::BeamSpotData* beamspotdata)
    : m_status ( 0  )
      , m_posX   ( 0. )
      , m_posY   ( 0. )
      , m_posZ   ( 0. )
      , m_sigmaX ( 0. )
      , m_sigmaY ( 0. )
      , m_sigmaZ ( 0. )
      , m_tiltX  ( 0. )
      , m_tiltY  ( 0. )
      , m_sigmaXY( 0. ){
     if(beamspotdata == nullptr) return;
     m_status   = beamspotdata->beamStatus();
     m_posX     = beamspotdata->beamPos().x();
     m_posY     = beamspotdata->beamPos().y();
     m_posZ     = beamspotdata->beamPos().z();
     m_sigmaX   = beamspotdata->beamSigma(0);
     m_sigmaY   = beamspotdata->beamSigma(1);
     m_sigmaZ   = beamspotdata->beamSigma(2);
     m_tiltX    = beamspotdata->beamTilt(0);
     m_tiltY    = beamspotdata->beamTilt(1);
     m_sigmaXY  = 0.; // FIXME: fill this in
  }

}
