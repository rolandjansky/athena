/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_VERTEX_FITTER__
#define __ITRIG_VERTEX_FITTER__

#include<list>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetToolInterfaces/TrigParticleTable.h"

static const InterfaceID IID_ITrigVertexFitter("ITrigVertexFitter",1,0); 

class ITrigVertexFitter: virtual public IAlgTool 
{

 public:
  static const InterfaceID& interfaceID() {
    return IID_ITrigVertexFitter;
  }
  virtual TrigVertex* fit(TrigVertex*) = 0;
  virtual TrigVertex* fit(TrackInVertexList*) = 0;
  virtual TrigVertex* fit(TrackInVertexList*,std::vector<TrigVtx::TrigParticleName>) = 0;
};




#endif
