/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_PRIM_VERTEX_FITTER__
#define __ITRIG_PRIM_VERTEX_FITTER__

#include<vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

class TrigVertex;
class TrigInDetTrackCollection;

static const InterfaceID IID_ITrigPrimaryVertexFitter("ITrigPrimaryVertexFitter",1,0); 

class ITrigPrimaryVertexFitter: virtual public IAlgTool 
{

 public:
  static const InterfaceID& interfaceID() {
    return IID_ITrigPrimaryVertexFitter;
  }
  virtual TrigVertex* fit(const TrigInDetTrackCollection*, double z=0.0) = 0;
  virtual TrigVertex* fit(const TrigInDetTrackCollection*, std::vector<double>&, double z=0.0) = 0;
  virtual TrigVertex* fit(const TrackCollection*, TrackCollection&, double z=0.0) = 0;
  virtual TrigVertex* fit(const TrackCollection*, std::vector<double>&, double z=0.0) = 0;
};

#endif
