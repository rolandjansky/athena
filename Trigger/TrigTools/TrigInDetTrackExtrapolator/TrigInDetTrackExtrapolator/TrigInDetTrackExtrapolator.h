/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGINDETTRACKEXTRAPOLATOR_H__
#define __TRIGINDETTRACKEXTRAPOLATOR_H__

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigInDetToolInterfaces/ITrigInDetTrackExtrapolator.h"

class TrigTimer;

namespace Trk {	
  class TrkTrackState;      
  class TrkPlanarSurface;
}

namespace MagField {
	class IMagFieldSvc;
}


class TrigInDetTrackExtrapolator: public AthAlgTool, virtual public ITrigInDetTrackExtrapolator
{
 public:
  TrigInDetTrackExtrapolator( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigInDetTrackExtrapolator();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  //  TrigInDetTrackFitPar* extrapolate(const TrigInDetTrack*, 
  //				    const TrigRoiDescriptor*);

  TrigInDetTrackFitPar* extrapolateToBarrel(const TrigInDetTrack*, double);
  TrigInDetTrackFitPar* extrapolateToEndcap(const TrigInDetTrack*, double);
  StatusCode extrapolateToCalo(const TrigInDetTrack*,double,double,double&,double&);

private:

  void m_getMagneticField(double[3],double*);
  Trk::TrkPlanarSurface* m_createEndSurfaceForBarrel(double[6],double);

  Trk::TrkTrackState* m_integrate(Trk::TrkTrackState*, 
				  Trk::TrkPlanarSurface*,
				  Trk::TrkPlanarSurface*);
  TrigInDetTrackFitPar* extrapolateBlindly(const TrigInDetTrack*, double, double);

  int  m_outputLevel;
  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;
  double m_minPt;

#define TRIGTRTEXT_NTIMERS 8
  TrigTimer* m_timer[TRIGTRTEXT_NTIMERS];
  bool m_timers;

};

#endif

