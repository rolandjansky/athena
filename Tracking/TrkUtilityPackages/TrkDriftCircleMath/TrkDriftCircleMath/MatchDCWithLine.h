/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DCMATH_MATCHDCWITHLINE_H
#define DCMATH_MATCHDCWITHLINE_H

#include "TrkDriftCircleMath/DriftCircle.h"
#include "TrkDriftCircleMath/DCOnTrack.h"
#include "TrkDriftCircleMath/Line.h"
#include "TrkDriftCircleMath/ResidualWithLine.h"
#include "TrkDriftCircleMath/HitSelection.h"

namespace TrkDriftCircleMath {
   
  class MatchDCWithLine {
  public:
    enum MatchStrategy { Road, Pull };
  public:
    MatchDCWithLine():m_resWithLine{},
			m_tubeRadius{},
			m_closeByCut{},
			m_showerCut{},
			m_deltaCut{},
			m_strategy{},
			m_deltas{},
			m_outOfTimes{},
			m_onTracks{},
			m_passedTubes{},
			m_ml1{},
			m_ml2{},
			m_closeHits{},
			m_showerHits{},
			m_matchDifference{},
			m_dcOnTrackVec{}
    {/**nop **/}

    MatchDCWithLine( const Line& l, double deltaCut, MatchStrategy strategy, double tubeRadius ) 
      : m_resWithLine(l), m_deltaCut(deltaCut), m_strategy(strategy),
      m_deltas{},
      m_outOfTimes{},
      m_onTracks{},
      m_passedTubes{},
      m_ml1{},
      m_ml2{},
      m_closeHits{},
      m_showerHits{},
      m_matchDifference{},
      m_dcOnTrackVec{}
  	{ 
      m_dcOnTrackVec.reserve(50);
      setTubeRadius(tubeRadius);    
    }

    
    void set( const Line& l, double deltaCut, MatchStrategy strategy, double tubeRadius ) {
      m_resWithLine.set(l);
      m_deltaCut = deltaCut;
      m_strategy = strategy;
      setTubeRadius(tubeRadius);
    }
    
    void setTubeRadius( double radius ){
      m_tubeRadius = radius;
      m_closeByCut = 2*radius;
      m_showerCut  = 4* radius;
    }

    const DCOnTrackVec& match( const DCVec& dcs );
    const DCOnTrackVec& match( const DCOnTrackVec& dcs, const HitSelection* selection = 0, bool recoverMdtHits = true, bool usePreciseErrors = false );
    
    unsigned int deltas() const { return m_deltas; }
    unsigned int hitsOutOfTime() const { return m_outOfTimes; }
    unsigned int hitsOnTrack() const { return m_onTracks; }
    unsigned int closeHits() const { return m_closeHits; }
    unsigned int showerHits() const { return m_showerHits; }
    unsigned int passedTubes() const { return m_passedTubes; }
    unsigned int hitsMl1() const { return m_ml1; }
    unsigned int hitsMl2() const { return m_ml2; }

    /** returns the number of DCOnTrack that have a different status after the match, returns 0 if used with DCVec */
    unsigned int matchDifference() const { return m_matchDifference; }
  private:
    
    void reset();
    void matchDC( DCOnTrack& dc, double& res, double& dline, bool forceOnTrack = false, bool forceOffTrack = false, bool usePreciseErrors = false );
    bool select( double residual, double error ) const;

    ResidualWithLine m_resWithLine;
    double       m_tubeRadius;
    double       m_closeByCut;
    double       m_showerCut;
    double       m_deltaCut;
    MatchStrategy m_strategy;
    unsigned int m_deltas;
    unsigned int m_outOfTimes;
    unsigned int m_onTracks;
    unsigned int m_passedTubes;
    unsigned int m_ml1;
    unsigned int m_ml2;
    unsigned int m_closeHits;
    unsigned int m_showerHits;

    unsigned int m_matchDifference;

    // cache results to reduce CPU usage
    mutable DCOnTrackVec m_dcOnTrackVec;
  };
  
  inline bool MatchDCWithLine::select( double residual, double error ) const
  {
    if( residual < 0. ) residual *= -1.;
    switch( m_strategy ){
    case Road :
      return residual < m_deltaCut;
    case Pull:
      return residual < m_deltaCut*error;
    default:
      return false;
    }
  }

  inline void MatchDCWithLine::reset() {
    //clear cache
    m_dcOnTrackVec.clear();

    m_outOfTimes = 0;
    m_deltas = 0;
    m_onTracks = 0;
    m_passedTubes = 0;
    m_ml1 = 0;
    m_ml2 = 0;
    m_closeHits = 0;
    m_showerHits = 0;
    m_matchDifference = 0;
  }
}

#endif
