/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXVERTEX_VXVERTEXDICT_H
#define VXVERTEX_VXVERTEXDICT_H

#include "VxVertex/Vertex.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/LinearizedTrack.h"
#include "DataModel/ElementLink.h"

namespace VxVertexDict
{
  class temp
  {
    public:
      ElementLink< VxContainer > m_link;
      std::vector<Trk::VxTrackAtVertex> m_vvtav;
  };
}

#endif



