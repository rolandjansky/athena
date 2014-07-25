/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIG_L2_VERTEX_FITTER__
#define __ITRIG_L2_VERTEX_FITTER__

#include<list>
#include "GaudiKernel/IAlgTool.h"

class TrigL2Vertex;

static const InterfaceID IID_ITrigL2VertexFitter("ITrigVertexL2Fitter",1,0); 

class ITrigL2VertexFitter: virtual public IAlgTool 
{
 public:
  static const InterfaceID& interfaceID() {
    return IID_ITrigL2VertexFitter;
  }
  virtual StatusCode fit(TrigL2Vertex*) = 0;
  virtual StatusCode fitWithConstraints(TrigL2Vertex*) = 0;
};

#endif
