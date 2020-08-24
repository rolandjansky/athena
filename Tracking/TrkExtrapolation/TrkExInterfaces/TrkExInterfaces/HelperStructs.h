/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRKEXINTERFACES_HELPERSTRUCTS_H
#define TRKEXINTERFACES_HELPERSTRUCTS_H

#include "TrkParameters/TrackParameters.h"

namespace Trk {
struct HitInfo 
{
  const Trk::TrackParameters*  trackParms;    // intersection
  float time;       // timing  
  int   detID;      // sensitive detector id
  float deposit;    // interaction energy deposit (secondaries subtracted)   

  /**
   * Constructor
   */
  HitInfo( const Trk::TrackParameters* parms, float time, int id, float deposit ) :
    trackParms(parms), time(time), detID(id), deposit(deposit) 
  {} 
};

struct PathLimit 
{
  float x0Max;       // path limit after which sampled process occurs
  float x0Collected; // x0 traversed so far
  float l0Collected; // l0 traversed so far
  float weightedZ;   // type of material traversed (divide by collected x0 to get average Z)
  int   process;     // type of pre-sampled material process

  /**
   * Constructor
   */
  PathLimit( float pathLimit, int proc) :
    x0Max(pathLimit), x0Collected(0.0), l0Collected(0.0),weightedZ(0.0), process(proc)
  {} 

  /**
   * collected material update
   */
  void updateMat( float dX0, float Z, float dL0) { 
    x0Collected += dX0; weightedZ += dX0*Z; 
    l0Collected += dL0>0. ? dL0 : dX0/0.37/Z;     // use approximation if l0 not available
  }
};

struct TimeLimit 
{
  float tMax;       // pre-sampled life-time
  float time;       // timing  
  int   process;    // type of pre-sampled decay process

  /**
   * Constructor
   */
  TimeLimit( float timeMax, float time, int proc) :
    tMax(timeMax), time(time), process(proc)
  {} 

  /**
   * timing update
   */
  void updateTime( float dt) { time += dt; }
};
}

#endif
