/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKV0VERTEX_V0VERTEXDICT_H
#define TRKV0VERTEX_V0VERTEXDICT_H

#include "VxVertex/Vertex.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/VxContainer.h"
#include "VxVertex/VxCandidate.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "VxVertex/ExtendedVxCandidate.h"
#include "VxVertex/LinearizedTrack.h"
#include "DataModel/ElementLink.h"
#include "TrkV0Vertex/V0Hypothesis.h"
#include "TrkV0Vertex/V0Candidate.h"
#include "TrkV0Vertex/V0Container.h"

namespace Trk
{
  class temp
  {
    public:
      ElementLink< V0Container > m_link;
      std::vector<Trk::V0Hypothesis*> m_dum1;
  };
}

#endif



