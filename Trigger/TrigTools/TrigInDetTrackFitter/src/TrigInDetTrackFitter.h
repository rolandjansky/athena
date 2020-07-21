/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGINDETTRACKFITTER_TRIGINDETTRACKFITTER_H
#define TRIGINDETTRACKFITTER_TRIGINDETTRACKFITTER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"


namespace Trk {	
  class TrkBaseNode;             
  class TrkTrackState;      
  class TrkPlanarSurface;
  class Track;
  class TrackStateOnSurface;
}

class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TrigL2HitResidual;

class TrigInDetTrackFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter
{
 public:
  TrigInDetTrackFitter( const std::string&, const std::string&, const IInterface* );
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  Trk::Track* fitTrack(const Trk::Track&, MagField::AtlasFieldCache&, const Trk::ParticleHypothesis& matEffects = Trk::pion) const;
  void fit(const TrackCollection&, TrackCollection&, const EventContext&, const Trk::ParticleHypothesis& matEffects = Trk::pion) const;
  StatusCode getUnbiasedResiduals(const Trk::Track&, std::vector<TrigL2HitResidual>&, const EventContext&) const;

private:

  Trk::TrkTrackState* extrapolate(Trk::TrkTrackState*, 
                                  Trk::TrkPlanarSurface*,
                                  Trk::TrkPlanarSurface*,
                                  MagField::AtlasFieldCache&) const;
  void getMagneticField(double[3],double*, MagField::AtlasFieldCache&) const;

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
  bool m_correctClusterPos;
  ToolHandle<ITrigDkfTrackMakerTool>    m_trackMaker;
	ToolHandle<Trk::IRIO_OnTrackCreator>  m_ROTcreator;
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  const AtlasDetectorID* m_idHelper;


};

#endif
