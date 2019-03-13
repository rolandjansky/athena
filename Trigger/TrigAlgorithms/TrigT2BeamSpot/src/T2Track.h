/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 *
 *
 * @project: HLT, PESA algorithms
 * @package: TrigT2BeamSpot
 * @class  : T2Track
 *
 * @brief Helper class that provides an interface to TrigInDetTrack
 *        with some additional functionality used by the beam spot algorithm
 *
 * @author Rainer Bartoldus, SLAC, <bartoldu@slac.stanford.edu>
 * @author David W. Miller, SLAC/Stanford University, <David.W.Miller@cern.ch>
 *
 **********************************************************************************/

#ifndef TRIGT2BEAMSPOT_T2TRACK_H
#define TRIGT2BEAMSPOT_T2TRACK_H

/// Externals
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrkTrack/Track.h" 
#include "TrkTrackSummary/TrackSummary.h"
#include "TrigInterfaces/IMonitoredAlgo.h"
#include "GaudiKernel/SystemOfUnits.h"
//using Gaudi::Units::GeV;

// Amg::error
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <string>
#include <vector>
#include <cmath>
#include <iostream>


namespace PESA {

  class T2Track;

  int    trackNDF     ( const TrigInDetTrack& track );
  double trackChi2Prob( const T2Track& track );

  class T2Track
  {
  public:

    // Constructor
    T2Track( const TrigInDetTrack& track )
      : m_Pt      ( std::abs(track.param()->pT())/Gaudi::Units::GeV  )
      , m_Eta     ( track.param()->eta()          )
      , m_Phi     ( track.param()->phi0()         )
      , m_Z0      ( track.param()->z0()           )
      , m_D0      ( track.param()->a0()           )
      , m_Z0err   ( track.param()->ez0()          )
      , m_D0err   ( track.param()->ea0()          )
      , m_NDF     ( trackNDF( track )             )
      , m_Qual    ( track.chi2()                  ) // Note this is the reduced chi2 (chi2/NDF)
      , m_Chi2Prob( -1.                           ) // lazy evaluation
      , m_SiHits  ( track.siSpacePoints()->size() )
      , m_PIXHits ( track.NPixelSpacePoints()     )
      , m_SCTHits ( track.NSCT_SpacePoints()      )
      , m_TRTHits ( track.NStrawHits()            )
      {}

    T2Track( const Trk::Track& track )
      : m_Chi2Prob( -1.                           ) // lazy evaluation
      {
        
        const Trk::TrackParameters* trackPars = track.perigeeParameters();
        if (trackPars) {
          m_D0 = trackPars->parameters()[Trk::d0]; 
          m_Z0 = trackPars->parameters()[Trk::z0]; 
          if (trackPars->covariance()) {
            m_D0err = Amg::error(*(trackPars->covariance()),Trk::d0);
            m_Z0err = Amg::error(*(trackPars->covariance()),Trk::z0);
          }
          m_Phi = trackPars->parameters()[Trk::phi0]; 
          float theta = trackPars->parameters()[Trk::theta]; 
          m_Eta = -log(tan(0.5*theta)); 
          float qOverP = trackPars->parameters()[Trk::qOverP]; 
          m_Pt = std::abs(std::sin(theta)/qOverP)/Gaudi::Units::GeV;

          const Trk::FitQuality* fq = track.fitQuality();
          m_Qual = 1e8;
          //m_NDF = 0;
          if (fq) {
            if(fq->numberDoF()!=0) {
              m_Qual = fq->chiSquared()/fq->numberDoF();
              //m_NDF = fq->numberDoF() - 5;//Remove 5 helix parameters
            }
          }
          int nPix=0;
          int nSct=0;
	  if( track.trackSummary() != nullptr){
	    nPix =    track.trackSummary()->get(Trk::numberOfPixelHits);
	    nSct =  track.trackSummary()->get(Trk::numberOfSCTHits);
	  } else {
	    for(auto tSOS = track.trackStateOnSurfaces()->begin();  
		tSOS!=track.trackStateOnSurfaces()->end(); ++tSOS) { 
	      if ((*tSOS)->type(Trk::TrackStateOnSurface::Perigee) == false) {
		const Trk::FitQualityOnSurface* fq =  (*tSOS)->fitQualityOnSurface(); 
		if(!fq) continue; 
		int nd = fq->numberDoF(); 
		if(nd==2) nPix++;
		if(nd==1) nSct++;
	      }
	    }
	  }
          m_PIXHits = nPix; 
          m_SCTHits = nSct/2; 
          m_SiHits = m_PIXHits + m_SCTHits; 
          m_NDF = (m_PIXHits + m_SCTHits)*2 - 5;
          m_TRTHits = 0; //for now: FTF tracks have no TRT extension in any case
        }
      }

    // Accessors
    double Pt      () const { return m_Pt     ; }
    double Eta     () const { return m_Eta    ; }
    double Phi     () const { return m_Phi    ; }
    double Z0      () const { return m_Z0     ; }
    double D0      () const { return m_D0     ; }
    double Z0err   () const { return m_Z0err  ; }
    double D0err   () const { return m_D0err  ; }
    double NDF     () const { return m_NDF    ; }
    double Qual    () const { return m_Qual   ; }
    double Chi2Prob() const { if ( m_Chi2Prob < 0. ) m_Chi2Prob = trackChi2Prob( *this ); return m_Chi2Prob  ; }
    int    SiHits  () const { return m_SiHits ; }
    int    PIXHits () const { return m_PIXHits; }
    int    SCTHits () const { return m_SCTHits; }
    int    TRTHits () const { return m_TRTHits; }

  private:
    // Data members
    double m_Pt;
    double m_Eta;
    double m_Phi;
    double m_Z0;
    double m_D0;
    double m_Z0err;
    double m_D0err;
    double m_NDF;
    double m_Qual;
    mutable std::atomic<double> m_Chi2Prob;
    int    m_SiHits;
    int    m_PIXHits;
    int    m_SCTHits;
    int    m_TRTHits;
  };


  std::ostream& operator<<( std::ostream& os, const T2Track& track );


  class MonitoredTrack
  {
  public:

    // Registration
    void declare( IMonitoredAlgo* p, std::string prefix = "", std::string suffix = "" )
    {
      const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

      p->declareMonitoredStdContainer( prefix + "Pt"       + suffix, m_Pt      , reset );
      p->declareMonitoredStdContainer( prefix + "Eta"      + suffix, m_Eta     , reset );
      p->declareMonitoredStdContainer( prefix + "Phi"      + suffix, m_Phi     , reset );
      p->declareMonitoredStdContainer( prefix + "Z0"       + suffix, m_Z0      , reset );
      p->declareMonitoredStdContainer( prefix + "D0"       + suffix, m_D0      , reset );
      p->declareMonitoredStdContainer( prefix + "Z0err"    + suffix, m_Z0err   , reset );
      p->declareMonitoredStdContainer( prefix + "D0err"    + suffix, m_D0err   , reset );
      p->declareMonitoredStdContainer( prefix + "NDF"      + suffix, m_NDF     , reset );
      p->declareMonitoredStdContainer( prefix + "Qual"     + suffix, m_Qual    , reset );
      p->declareMonitoredStdContainer( prefix + "Chi2Prob" + suffix, m_Chi2Prob, reset );
      p->declareMonitoredStdContainer( prefix + "SiHits"   + suffix, m_SiHits  , reset );
      p->declareMonitoredStdContainer( prefix + "PIXHits"  + suffix, m_PIXHits , reset );
      p->declareMonitoredStdContainer( prefix + "SCTHits"  + suffix, m_SCTHits , reset );
      p->declareMonitoredStdContainer( prefix + "TRTHits"  + suffix, m_TRTHits , reset );
    }

    // Update method
    void push_back( const TrigInDetTrack& track )
    {
      push_back( T2Track( track ) );
    }

    void push_back( const T2Track& track )
    {
      m_Pt      .push_back( track.Pt      () );
      m_Eta     .push_back( track.Eta     () );
      m_Phi     .push_back( track.Phi     () );
      m_Z0      .push_back( track.Z0      () );
      m_D0      .push_back( track.D0      () );
      m_Z0err   .push_back( track.Z0err   () );
      m_D0err   .push_back( track.D0err   () );
      m_NDF     .push_back( track.NDF     () );    
      m_Qual    .push_back( track.Qual    () );    
      m_Chi2Prob.push_back( track.Chi2Prob() );
      m_SiHits  .push_back( track.SiHits  () );
      m_PIXHits .push_back( track.PIXHits () );
      m_SCTHits .push_back( track.SCTHits () );
      m_TRTHits .push_back( track.TRTHits () );
    }

  private:
    // Data members
    std::vector<double> m_Pt;
    std::vector<double> m_Eta;
    std::vector<double> m_Phi;
    std::vector<double> m_Z0;
    std::vector<double> m_D0;
    std::vector<double> m_Z0err;
    std::vector<double> m_D0err;
    std::vector<double> m_NDF;
    std::vector<double> m_Qual;
    std::vector<double> m_Chi2Prob;
    std::vector<int>    m_SiHits;
    std::vector<int>    m_PIXHits;
    std::vector<int>    m_SCTHits;
    std::vector<int>    m_TRTHits;
  };


  // The following will become possible with an extension to IMonitoredAlgo

#if 0
  class MonitoredTrack2
  {
  public:

    // Registration
    void declare( IMonitoredAlgo* p, std::string prefix = "", std::string suffix = "" )
    {
      const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

      p->declareMonitoredStdContainer( prefix + "Pt"       + suffix, m_track, &T2Track::Pt      , reset );
      p->declareMonitoredStdContainer( prefix + "Eta"      + suffix, m_track, &T2Track::Eta     , reset );
      p->declareMonitoredStdContainer( prefix + "Phi"      + suffix, m_track, &T2Track::Phi     , reset );
      p->declareMonitoredStdContainer( prefix + "Z0"       + suffix, m_track, &T2Track::Z0      , reset );
      p->declareMonitoredStdContainer( prefix + "D0"       + suffix, m_track, &T2Track::D0      , reset );
      p->declareMonitoredStdContainer( prefix + "Z0err"    + suffix, m_track, &T2Track::Z0err   , reset );
      p->declareMonitoredStdContainer( prefix + "D0err"    + suffix, m_track, &T2Track::D0err   , reset );
      p->declareMonitoredStdContainer( prefix + "NDF"      + suffix, m_track, &T2Track::NDF     , reset );
      p->declareMonitoredStdContainer( prefix + "Qual"     + suffix, m_track, &T2Track::Qual    , reset );
      p->declareMonitoredStdContainer( prefix + "Chi2Prob" + suffix, m_track, &T2Track::Chi2Prob, reset );
      p->declareMonitoredStdContainer( prefix + "SiHits"   + suffix, m_track, &T2Track::SiHits  , reset );
      p->declareMonitoredStdContainer( prefix + "PIXHits"  + suffix, m_track, &T2Track::PIXHits , reset );
      p->declareMonitoredStdContainer( prefix + "SCTHits"  + suffix, m_track, &T2Track::SCTHits , reset );
      p->declareMonitoredStdContainer( prefix + "TRTHits"  + suffix, m_track, &T2Track::TRTHits , reset );
    }

    // Update method
    void push_back( const T2Track& track )
    {
      m_track.push_back( track );
    }

  private:
    // Data members
    std::vector< T2Track > m_track;
  };
#endif


} // end namespace

#endif
