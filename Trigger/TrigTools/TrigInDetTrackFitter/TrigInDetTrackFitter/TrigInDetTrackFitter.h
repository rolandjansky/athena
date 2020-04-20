/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETTRACKFITTER_TRIGINDETTRACKFITTER_H
#define TRIGINDETTRACKFITTER_TRIGINDETTRACKFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MagFieldInterfaces/IMagFieldSvc.h" 
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"


class TrigTimer;

namespace Trk {	
  class TrkBaseNode;             
  class TrkTrackState;      
  class TrkPlanarSurface;
  class Track;
  class TrackStateOnSurface;
}

namespace MagField {	
	class IMagFieldSvc;
}

class TrigInDetTrackFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter
{
 public:
  TrigInDetTrackFitter( const std::string&, const std::string&, const IInterface* );
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  Trk::Track* fitTrack(const Trk::Track&, const Trk::ParticleHypothesis& matEffects = Trk::pion) const;
  void fit(const TrackCollection&, TrackCollection&, const Trk::ParticleHypothesis& matEffects = Trk::pion) const;
private:

  Trk::TrkTrackState* extrapolate(Trk::TrkTrackState*, 
                                  Trk::TrkPlanarSurface*,
                                  Trk::TrkPlanarSurface*) const;
  void getMagneticField(double[3],double*) const;

  void correctScale(Trk::TrkTrackState*) const;

  Trk::TrackStateOnSurface* createTrackStateOnSurface(Trk::TrkBaseNode* pN) const;

  //counters
  ATLAS_THREAD_SAFE mutable size_t m_nTracksTotal;
  ATLAS_THREAD_SAFE mutable size_t m_fitErrorsUnresolved;
  ATLAS_THREAD_SAFE mutable size_t m_fitErrorsDivergence;
  ATLAS_THREAD_SAFE mutable size_t m_fitErrorsLowPt;

  double m_DChi2;
  bool m_doMultScatt;
  bool m_doBremm;
  bool m_offlineClusters;
  bool m_correctClusterPos;
  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;  
  ToolHandle<ITrigDkfTrackMakerTool>    m_trackMaker;
	ToolHandle<Trk::IRIO_OnTrackCreator>  m_ROTcreator;

};

#endif
