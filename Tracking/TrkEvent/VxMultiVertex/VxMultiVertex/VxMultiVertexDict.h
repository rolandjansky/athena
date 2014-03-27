/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXMULTIVERTEX_VXMULTIVERTEXDICT_H
#define VXMULTIVERTEX_VXMULTIVERTEXDICT_H

#include "VxVertex/Vertex.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/LinearizedTrack.h"

#include "VxMultiVertex/MVFVxCandidate.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "VxMultiVertex/MVFVxContainer.h"
#include "VxMultiVertex/MvfFitInfo.h"

#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBaseCollection.h"
#include "DataModel/ElementLink.h"

namespace VxMultiVertexDict
{
  class temp
  {
  public:
    ElementLink< MVFVxContainer > m_link;
  };
}


#endif



