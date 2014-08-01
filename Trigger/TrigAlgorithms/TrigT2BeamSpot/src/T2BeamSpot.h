/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 * @version: $Id: T2BeamSpot.h 361213 2011-04-23 18:26:44Z bartoldu $
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2BeamSpot
 *
 * @brief Helper class that provides an interface to IBeamCondSvc
 *        with some additional functionality used by the beam spot algorithm
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2BEAMSPOT_H
#define TRIGT2BEAMSPOT_T2BEAMSPOT_H

#include "InDetBeamSpotService/IBeamCondSvc.h"

#include <iostream>


namespace PESA
{

  class T2BeamSpot
  {
  public:
  T2BeamSpot( const IBeamCondSvc* beamCondSvc )
    : m_status ( 0  )
      , m_posX   ( 0. )
      , m_posY   ( 0. )
      , m_posZ   ( 0. )
      , m_sigmaX ( 0. )
      , m_sigmaY ( 0. )
      , m_sigmaZ ( 0. )
      , m_tiltX  ( 0. )
      , m_tiltY  ( 0. )
      , m_sigmaXY( 0. )
      {
        if ( beamCondSvc )
          {
            m_status   = beamCondSvc->beamStatus();
            m_posX     = beamCondSvc->beamPos().x();
            m_posY     = beamCondSvc->beamPos().y();
            m_posZ     = beamCondSvc->beamPos().z();
            m_sigmaX   = beamCondSvc->beamSigma(0);
            m_sigmaY   = beamCondSvc->beamSigma(1);
            m_sigmaZ   = beamCondSvc->beamSigma(2);
            m_tiltX    = beamCondSvc->beamTilt(0);
            m_tiltY    = beamCondSvc->beamTilt(1);
            m_sigmaXY  = 0.; // FIXME: fill this in
          }
      }

    // Accessors
    unsigned status () const { return m_status ; }
    double   posX   () const { return m_posX   ; }
    double   posY   () const { return m_posY   ; }
    double   posZ   () const { return m_posZ   ; } 
    double   sigmaX () const { return m_sigmaX ; }
    double   sigmaY () const { return m_sigmaY ; }
    double   sigmaZ () const { return m_sigmaZ ; }
    double   tiltX  () const { return m_tiltX  ; }
    double   tiltY  () const { return m_tiltY  ; }
    double   sigmaXY() const { return m_sigmaXY; }

  private:
    // Data members
    unsigned m_status ;
    double   m_posX   ;
    double   m_posY   ;
    double   m_posZ   ; 
    double   m_sigmaX ;
    double   m_sigmaY ;
    double   m_sigmaZ ;
    double   m_tiltX  ;
    double   m_tiltY  ;
    double   m_sigmaXY;
  };


  std::ostream& operator<<( std::ostream& os, const T2BeamSpot& beamSpot );


} // end namespace

#endif
