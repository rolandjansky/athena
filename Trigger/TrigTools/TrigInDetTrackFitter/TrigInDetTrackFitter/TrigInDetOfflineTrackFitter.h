/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDET_OFFLINETRACKFITTER_H__
#define __TRIGINDET_OFFLINETRACKFITTER_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetIdentifier/PixelID.h" 
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"

class TrigTimer;


class TrigInDetOfflineTrackFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter
{
 public:
  TrigInDetOfflineTrackFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigInDetOfflineTrackFitter(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  void fit(TrigInDetTrackCollection*);
  TrackCollection* fit(const TrackCollection&, const Trk::ParticleHypothesis&) {return nullptr;}
private:
  const PixelID* m_pixelId;
  const SCT_ID* m_sctId;
  double m_DChi2;
  bool m_doMultScatt,m_doBremm,m_offlineClusters;
  ToolHandle<Trk::ITrackFitter> m_trackFitter;

#define TRIGOFFLINETRACKFIT_NTIMERS 10
  TrigTimer* m_timer[TRIGOFFLINETRACKFIT_NTIMERS];
  bool m_timers;

};

#endif
