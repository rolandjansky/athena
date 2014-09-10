/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDET_COMBINED_TRACKFITTER_H__
#define __TRIGINDET_COMBINED_TRACKFITTER_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"
#include "TrigInDetTrackFitter/ITrigL2TrackFittingTool.h"

class TrigInDetCombinedTrackFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter
{
 public:
  TrigInDetCombinedTrackFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigInDetCombinedTrackFitter(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  void fit(TrigInDetTrackCollection*);
  TrackCollection* fit(const TrackCollection&, const Trk::ParticleHypothesis&) {return nullptr;}

private:
  bool m_doMultScatt,m_doBremm;
  double m_momentumThreshold,m_DChi2;
  ToolHandle<ITrigL2TrackFittingTool> m_highPtFitter;
  ToolHandle<ITrigL2TrackFittingTool> m_lowPtFitter;
  //ToolHandle<ITrigL2TrackFittingTool> m_robustFitter;

  ToolHandle<ITrigDkfTrackMakerTool> m_trackMaker;
};

#endif
