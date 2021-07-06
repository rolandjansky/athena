/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2Vertex
 *
 * @brief Helper class that provides an interface to TrigVertex
 *        with some additional functionality used by the beam spot algorithm
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 * @author David W. Miller, SLAC/Stanford University, <David.W.Miller@cern.ch>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2VERTEX_H
#define TRIGT2BEAMSPOT_T2VERTEX_H

/// Local tools
#include "T2BeamSpot.h"

/// External classes
#include "TrigInDetEvent/TrigVertex.h"
#include "TrkTrack/TrackCollection.h"
#include "TMath.h"

#include <string>
#include <vector>
#include <cmath>
#include <iostream>

namespace PESA
{
  class T2Vertex;

  // Helpers
  double vertexChi2Prob( const T2Vertex& vertex );

  double vertexSumPt ( const TrackCollection& tracks );
  double vertexSumPt2( const TrackCollection& tracks );

  double tiltedBeamPositionAtZPoint( double Zref, double nominalZPosition,
                                     double nominalTransversePosition, double tilt );

  class T2SimpleVertex
  {
  public:
    // Constructor
    T2SimpleVertex( const TrigVertex& vertex )
      : m_NTrks  ( ( vertex.ndof() + 3 ) / 2 )
      , m_X      ( vertex.x()                )
      , m_Y      ( vertex.y()                )
      , m_Z      ( vertex.z()                )
      // FIXME: TrigVertex::cov() is not const-correct
      , m_Xerr   ( sqrt( const_cast< TrigVertex& >(vertex).cov()[0] ) )
      , m_Yerr   ( sqrt( const_cast< TrigVertex& >(vertex).cov()[2] ) )
      , m_Zerr   ( sqrt( const_cast< TrigVertex& >(vertex).cov()[5] ) )
      {
      }

    // Accessors
    unsigned NTrks   () const { return m_NTrks; }
    double   X       () const { return m_X    ; }
    double   Y       () const { return m_Y    ; }
    double   Z       () const { return m_Z    ; }
    double   Xerr    () const { return m_Xerr ; }
    double   Yerr    () const { return m_Yerr ; }
    double   Zerr    () const { return m_Zerr ; }

  private:
    // Data members
    unsigned m_NTrks;
    double   m_X    ;
    double   m_Y    ;
    double   m_Z    ;
    double   m_Xerr ;
    double   m_Yerr ;
    double   m_Zerr ;
  };


  class T2Vertex : public T2SimpleVertex
  {
  public:
    // Constructor

    T2Vertex( const TrigVertex& vertex, const TrackCollection& tracks, const T2BeamSpot& beamSpot, double seedZ )
      : T2SimpleVertex( vertex )
      , m_SumPt   ( -1.                            ) // lazy evaluation
      , m_SumPt2  ( -1.                            )
      , m_Mass    ( vertex.mass()                  )
      , m_NDF     ( vertex.ndof()                  )
      , m_Qual    ( vertex.chi2() / vertex.ndof()  ) // FIXME: ndof==0 ?
      , m_Chi2Prob( -1.                            ) // lazy evaluation
      , m_XY      ( 0.                             )
      , m_Pull    ( vertex.z() - seedZ             ) // FIXME: that's not a pull
      , m_tracks  (tracks)
      {
        const double beamXatVtx =
          tiltedBeamPositionAtZPoint( Z(), beamSpot.posZ(), beamSpot.posX(), beamSpot.tiltX() );
        const double beamYatVtx =
          tiltedBeamPositionAtZPoint( Z(), beamSpot.posZ(), beamSpot.posY(), beamSpot.tiltY() );

        m_XZoom = X() - beamXatVtx;                                                        
        m_YZoom = Y() - beamYatVtx;                                                        
        m_ZZoom = Z() - beamSpot.posZ();
      }

    // Accessors
    double   SumPt   () const { 
      if ( m_SumPt  < 0. ) {
        return vertexSumPt ( m_tracks );
      }
      else {
        return m_SumPt;
      }
    }

    double   SumPt2   () const { 
      if ( m_SumPt2  < 0. ) {
        return vertexSumPt2 ( m_tracks );
      }
      else {
        return m_SumPt2;
      }
    }

    double   Mass    () const { return m_Mass    ; }
    double   NDF     () const { return m_NDF     ; }
    double   Qual    () const { return m_Qual    ; }
    double   Chi2Prob() const { if ( m_Chi2Prob < 0. ) m_Chi2Prob = vertexChi2Prob( *this ); return m_Chi2Prob; }
    double   XZoom   () const { return m_XZoom   ; }
    double   YZoom   () const { return m_YZoom   ; }
    double   ZZoom   () const { return m_ZZoom   ; }
    double   XY      () const { return m_XY      ; }
    double   Pull    () const { return m_Pull    ; }

    unsigned NTrksInVtx() const { return m_tracks.size(); }


  private:
    // Data members
    double   m_SumPt   ;
    double   m_SumPt2  ;
    double   m_Mass    ;
    double   m_NDF     ;
    double   m_Qual    ;
    mutable std::atomic<double>   m_Chi2Prob;
    double   m_XZoom   ;
    double   m_YZoom   ;
    double   m_ZZoom   ;
    double   m_XY      ;
    double   m_Pull    ;   

    TrackCollection m_tracks;
  };


  std::ostream& operator<<( std::ostream& os, const T2Vertex& vertex );

} // end namespace

#endif
