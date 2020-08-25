/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGVERTEXFITTER_TRIGVERTEXINGTOOL_H
#define TRIGVERTEXFITTER_TRIGVERTEXINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrigInDetToolInterfaces/ITrigVertexingTool.h"

class TrigTimer;
class TrigVertex;
class TrigL2Vertex;
class TrigInDetTrack;
namespace Trk {
  class Track;
}

class TrigVertexingTool: public AthAlgTool, virtual public ITrigVertexingTool
{
 public:
  TrigVertexingTool( const std::string&, const std::string&, const IInterface* );

  static const InterfaceID& interfaceID();

  virtual ~TrigVertexingTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  /*
  double FindClosestApproach(const Trk::Track*,const Trk::Track*, double*);
  double FindClosestApproach(const TrigInDetTrack*,const TrigInDetTrack*, double*);
  */
  double FindClosestApproach(const TrigVertexFitInputTrack*,const TrigVertexFitInputTrack*, 
			     double*);
  /*
  double TrackToTrackDistance(const Trk::Track*,const Trk::Track*);
  double TrackToTrackDistance(const TrigInDetTrack*,const TrigInDetTrack*);
  double TrackToVertexDistance(const Trk::Track*,const TrigL2Vertex*);
  double TrackToVertexDistance(const TrigInDetTrack*,const TrigL2Vertex*);
  double TrackToTrackChi2Distance(const Trk::Track*,const Trk::Track*);
  double TrackToTrackChi2Distance(const TrigInDetTrack*,const TrigInDetTrack*);
  double TrackToVertexChi2Distance(const Trk::Track*,const TrigL2Vertex*);
  double TrackToVertexChi2Distance(const TrigInDetTrack*,const TrigL2Vertex*);
  */
  StatusCode addTrack(const TrigInDetTrack*,TrigL2Vertex*, const Trk::ParticleHypothesis);
  StatusCode addTrack(const Trk::Track*, TrigL2Vertex*, const Trk::ParticleHypothesis);
  StatusCode addTrack(const TrigInDetTrack*,TrigL2Vertex*, double mass);
  StatusCode addTrack(const Trk::Track*, TrigL2Vertex*, double mass);
  StatusCode calculateInvariantMass(TrigL2Vertex*);
  StatusCode createMotherParticle(TrigL2Vertex*);

  TrigVertex* createTrigVertex(TrigL2Vertex*);

  StatusCode setMassConstraint(TrigL2Vertex*, const TrigInDetTrack*, const TrigInDetTrack*, double);
  StatusCode setMassConstraint(TrigL2Vertex*, const TrigInDetTrack*, const TrigInDetTrack*,
			       const TrigInDetTrack*, double);

  StatusCode setMassConstraint(TrigL2Vertex*,const Trk::Track*,const Trk::Track*, double);
  StatusCode setMassConstraint(TrigL2Vertex*,const Trk::Track*, const Trk::Track*, 
			       const Trk::Track*, double);

  /*
   StatusCode addMassToTrack(const Trk::Track*,TrigL2Vertex*, double);
   StatusCode addMassToTrack(const TrigInDetTrack*,TrigL2Vertex*, double);
   StatusCode removeTrack(const TrigInDetTrack*);
   StatusCode removeTrack(const Trk::Track*);
   TrigL2Vertex* createVertex(std::list<const TrigInDetTrack*>&);
   TrigL2Vertex* createVertex(std::list<const Trk::Track*>&);
   TrigL2Vertex* createVertex(std::list<std::pair<const TrigInDetTrack*, 
				     const Trk::ParticleHypothesis> >&);
   TrigL2Vertex* createVertex(std::list<std::pair<const Trk::Track*, 
				     const Trk::ParticleHypothesis> >&);
   TrigL2Vertex* createVertex(std::list<std::pair<const TrigInDetTrack*, 
				     double> >&);
   TrigL2Vertex* createVertex(std::list<std::pair<const Trk::Track*, 
				     double> >&);

  */

 private:
  double twoCircleClosestApproach(const double *, const double *, double*);

#define TRIGVERTEXTOOL_NTIMERS 5
  TrigTimer* m_timer[TRIGVERTEXTOOL_NTIMERS];
  bool m_timers;

};

#endif
