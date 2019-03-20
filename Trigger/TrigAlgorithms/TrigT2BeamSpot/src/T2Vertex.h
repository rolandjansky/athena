/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TrigInterfaces/IMonitoredAlgo.h"
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
  double vertexSumPt ( const TrackInVertexList& tracks );
  double vertexSumPt2( const TrackInVertexList& tracks );

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

    T2Vertex( const TrigVertex& vertex, const T2BeamSpot& beamSpot, double seedZ )
      : T2SimpleVertex( vertex )
      , m_SumPt   ( -1.                            ) // lazy evaluation
      , m_SumPt2  ( -1.                            )
      , m_Mass    ( vertex.mass()                  )
      , m_NDF     ( vertex.ndof()                  )
      , m_Qual    ( vertex.chi2() / vertex.ndof()  ) // FIXME: ndof==0 ?
      , m_Chi2Prob( -1.                            ) // lazy evaluation
      , m_XY      ( 0.                             )
      , m_Pull    ( vertex.z() - seedZ             ) // FIXME: that's not a pull
      , m_tracks  ( vertex.tracks()                )
      , m_trkTracks  ( nullptr                     )
      {
        const double beamXatVtx =
          tiltedBeamPositionAtZPoint( Z(), beamSpot.posZ(), beamSpot.posX(), beamSpot.tiltX() );
        const double beamYatVtx =
          tiltedBeamPositionAtZPoint( Z(), beamSpot.posZ(), beamSpot.posY(), beamSpot.tiltY() );

        m_XZoom = X() - beamXatVtx;                                                        
        m_YZoom = Y() - beamYatVtx;                                                        
        m_ZZoom = Z() - beamSpot.posZ();
      }

    T2Vertex( const TrigVertex& vertex, TrackCollection* tracks, const T2BeamSpot& beamSpot, double seedZ )
      : T2SimpleVertex( vertex )
      , m_SumPt   ( -1.                            ) // lazy evaluation
      , m_SumPt2  ( -1.                            )
      , m_Mass    ( vertex.mass()                  )
      , m_NDF     ( vertex.ndof()                  )
      , m_Qual    ( vertex.chi2() / vertex.ndof()  ) // FIXME: ndof==0 ?
      , m_Chi2Prob( -1.                            ) // lazy evaluation
      , m_XY      ( 0.                             )
      , m_Pull    ( vertex.z() - seedZ             ) // FIXME: that's not a pull
      , m_tracks  ( nullptr                )
      , m_trkTracks  (tracks)
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
        if (m_tracks) { 
          return vertexSumPt ( *m_tracks );
        }
        else if (m_trkTracks) {
          return vertexSumPt ( *m_trkTracks );
        }
        else return 0; 
      }
      else {
        return m_SumPt;
      }
    }

    double   SumPt2   () const { 
      if ( m_SumPt2  < 0. ) {
        if (m_tracks) { 
          return vertexSumPt2 ( *m_tracks );
        }
        else if (m_trkTracks) {
          return vertexSumPt2 ( *m_trkTracks );
        }
        else return 0;
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

    unsigned NTrksInVtx() const { 
                                  if (m_tracks) {
                                    return m_tracks->size();
                                  } 
                                  else if (m_trkTracks) {
                                    return m_trkTracks->size();
                                  }
                                  else return 0;
                                }


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

    TrackInVertexList* m_tracks; TrackCollection* m_trkTracks;
  };


  std::ostream& operator<<( std::ostream& os, const T2Vertex& vertex );


  class MonitoredVertex
  {
  public:

    // Registration
    void declare( IMonitoredAlgo* p, std::string prefix = "", std::string suffix = "" )
    {
      const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

      p->declareMonitoredStdContainer( prefix + "NTrks"      + suffix, m_NTrks     , reset );
      p->declareMonitoredStdContainer( prefix + "SumPt"      + suffix, m_SumPt     , reset );
      p->declareMonitoredStdContainer( prefix + "SumPt2"     + suffix, m_SumPt2    , reset );
      p->declareMonitoredStdContainer( prefix + "Mass"       + suffix, m_Mass      , reset );
      p->declareMonitoredStdContainer( prefix + "Qual"       + suffix, m_Qual      , reset );
      p->declareMonitoredStdContainer( prefix + "Chi2Prob"   + suffix, m_Chi2Prob  , reset );
      p->declareMonitoredStdContainer( prefix + "X"          + suffix, m_X         , reset );
      p->declareMonitoredStdContainer( prefix + "Y"          + suffix, m_Y         , reset );
      p->declareMonitoredStdContainer( prefix + "Z"          + suffix, m_Z         , reset );
      p->declareMonitoredStdContainer( prefix + "XZoom"      + suffix, m_XZoom     , reset );
      p->declareMonitoredStdContainer( prefix + "YZoom"      + suffix, m_YZoom     , reset );
      p->declareMonitoredStdContainer( prefix + "ZZoom"      + suffix, m_ZZoom     , reset );
      p->declareMonitoredStdContainer( prefix + "Xerr"       + suffix, m_Xerr      , reset );
      p->declareMonitoredStdContainer( prefix + "Yerr"       + suffix, m_Yerr      , reset );
      p->declareMonitoredStdContainer( prefix + "Zerr"       + suffix, m_Zerr      , reset );
      p->declareMonitoredStdContainer( prefix + "XY"         + suffix, m_XY        , reset );
      p->declareMonitoredStdContainer( prefix + "Pull"       + suffix, m_Pull      , reset );
      p->declareMonitoredStdContainer( prefix + "NTrksInVtx" + suffix, m_NTrksInVtx, reset );
    }

    // Update method
    void push_back( const T2Vertex& vertex )
    {
      m_NTrks     .push_back( vertex.NTrks     () );
      m_SumPt     .push_back( vertex.SumPt     () );
      m_SumPt2    .push_back( vertex.SumPt2    () );
      m_Mass      .push_back( vertex.Mass      () );
      m_Qual      .push_back( vertex.Qual      () );
      m_Chi2Prob  .push_back( vertex.Chi2Prob  () );
      m_X         .push_back( vertex.X         () );
      m_Y         .push_back( vertex.Y         () );
      m_Z         .push_back( vertex.Z         () );
      m_XZoom     .push_back( vertex.XZoom     () );
      m_YZoom     .push_back( vertex.YZoom     () );
      m_ZZoom     .push_back( vertex.ZZoom     () );
      m_Xerr      .push_back( vertex.Xerr      () );
      m_Yerr      .push_back( vertex.Yerr      () );
      m_Zerr      .push_back( vertex.Zerr      () );
      m_XY        .push_back( vertex.XY        () );
      m_Pull      .push_back( vertex.Pull      () );
      m_NTrksInVtx.push_back( vertex.NTrksInVtx() );
    }

  private:
    // Data members
    std::vector<unsigned> m_NTrks     ;
    std::vector<double  > m_SumPt     ;
    std::vector<double  > m_SumPt2    ;
    std::vector<double  > m_Mass      ;
    std::vector<double  > m_Qual      ;
    std::vector<double  > m_Chi2Prob  ;
    std::vector<double  > m_X         ;
    std::vector<double  > m_Y         ;
    std::vector<double  > m_Z         ;
    std::vector<double  > m_XZoom     ;
    std::vector<double  > m_YZoom     ;
    std::vector<double  > m_ZZoom     ;
    std::vector<double  > m_Xerr      ;
    std::vector<double  > m_Yerr      ;
    std::vector<double  > m_Zerr      ;
    std::vector<double  > m_XY        ;
    std::vector<double  > m_Pull      ;   
    std::vector<unsigned> m_NTrksInVtx;   
  };

} // end namespace

#endif
