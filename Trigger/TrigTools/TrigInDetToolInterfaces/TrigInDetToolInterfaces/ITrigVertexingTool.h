/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGVERTEXINGTOOL_H__
#define __ITRIGVERTEXINGTOOL_H__

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk {
  class Track;
}
class TrigL2Vertex;
class TrigVertex;
class TrigInDetTrack;
class TrigVertexFitInputTrack;

static const InterfaceID IID_ITrigVertexingTool("ITrigVertexingTool",1,0); 

class ITrigVertexingTool: virtual public IAlgTool 
{
 public:
  static const InterfaceID& interfaceID() {
    return IID_ITrigVertexingTool;
  }
  virtual double FindClosestApproach(const TrigVertexFitInputTrack*,const TrigVertexFitInputTrack*, 
			     double*) = 0;

  virtual StatusCode addTrack(const TrigInDetTrack*,TrigL2Vertex*, const Trk::ParticleHypothesis) = 0;
  virtual StatusCode addTrack(const Trk::Track*, TrigL2Vertex*, const Trk::ParticleHypothesis) = 0;
  virtual StatusCode addTrack(const TrigInDetTrack*,TrigL2Vertex*, double mass) = 0;
  virtual StatusCode addTrack(const Trk::Track*, TrigL2Vertex*, double mass) = 0;
  virtual StatusCode calculateInvariantMass(TrigL2Vertex*) = 0;
  virtual StatusCode createMotherParticle(TrigL2Vertex*) = 0;

  virtual TrigVertex* createTrigVertex(TrigL2Vertex*) = 0;

  virtual StatusCode setMassConstraint(TrigL2Vertex*, const TrigInDetTrack*, const TrigInDetTrack*, double) = 0;
  virtual StatusCode setMassConstraint(TrigL2Vertex*, const TrigInDetTrack*, const TrigInDetTrack*,
			       const TrigInDetTrack*, double) = 0;

  virtual StatusCode setMassConstraint(TrigL2Vertex*,const Trk::Track*,const Trk::Track*, double) = 0;
  virtual StatusCode setMassConstraint(TrigL2Vertex*,const Trk::Track*, const Trk::Track*, 
			       const Trk::Track*, double) = 0;

};

#endif

