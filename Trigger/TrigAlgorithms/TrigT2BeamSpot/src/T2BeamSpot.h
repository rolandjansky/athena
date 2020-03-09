/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


#include <iostream>

namespace InDet { class BeamSpotData; }

namespace PESA
{

  class T2BeamSpot
  {
  public:


    T2BeamSpot( const InDet::BeamSpotData* );
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
