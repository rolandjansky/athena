/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDETTRACKFITTER_H__
#define __TRIGINDETTRACKFITTER_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MagFieldInterfaces/IMagFieldSvc.h" 
#include "TrigInDetToolInterfaces/ITrigInDetTrackFitter.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"

class TrigTimer;

namespace Trk {	
  class TrkBaseNode;             
  class TrkTrackState;      
  class TrkPlanarSurface;
  class IRIO_OnTrackCreator;
  class Track;
  class TrackStateOnSurface;
}

namespace MagField {	
	class IMagFieldSvc;
}

struct FitStatStruct {
  FitStatStruct(int id) : m_algorithmId(id)  {
    m_nTracksTotal = 0;
    for(int i=0;i<5;i++) m_fitErrors[i]=0;
  }
  int m_algorithmId;
  long int m_nTracksTotal;
  long int m_fitErrors[5];
};

class TrigInDetTrackFitter: public AthAlgTool, virtual public ITrigInDetTrackFitter
{
 public:
  TrigInDetTrackFitter( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigInDetTrackFitter(){};
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  void fitTrack(TrigInDetTrack&);
  void fit(TrigInDetTrackCollection*);
  Trk::Track* fitTrack(const Trk::Track&, const Trk::ParticleHypothesis& matEffects = Trk::pion);
  TrackCollection* fit(const TrackCollection&, const Trk::ParticleHypothesis& matEffects = Trk::pion);
private:

  Trk::TrkTrackState* m_extrapolate(Trk::TrkTrackState*, 
				    Trk::TrkPlanarSurface*,
				    Trk::TrkPlanarSurface*);
  void m_matrixInversion5x5(double a[5][5]);
  void m_getMagneticField(double[3],double*);

  void correctScale(Trk::TrkTrackState*);

  Trk::TrackStateOnSurface* createTrackStateOnSurface(Trk::TrkBaseNode* pN) const;

  double m_DChi2;
  bool m_doMultScatt;
  bool m_doBremm;
  bool m_offlineClusters;
  bool m_correctClusterPos;
  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;  
  ToolHandle<ITrigDkfTrackMakerTool>    m_trackMaker;
	ToolHandle<Trk::IRIO_OnTrackCreator>  m_ROTcreator;

  std::vector<FitStatStruct> m_fitStats;
  int m_algorithmId;

#define TRIGTRACKFIT_NTIMERS 10
  TrigTimer* m_timer[TRIGTRACKFIT_NTIMERS];
  bool m_timers;

};

#endif
