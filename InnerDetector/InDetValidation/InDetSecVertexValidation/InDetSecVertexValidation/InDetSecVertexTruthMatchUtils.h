/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetSecVertexTruthMatchUtils_h
#define InDetSecVertexTruthMatchUtils_h

#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthEventBaseContainer.h"

namespace InDetSecVertexTruthMatchUtils {

  //Namespace for useful analysis things on the truth matching decorations applied to the VertexContainer
  //Can be called by algorithms inside or outside Athena to do their analysis

  //How the matching info is stored; link to the truth vertex, a float with its contribution to the relative track weight, and a float with its contribution to the track sumpt2 of the truth vertex
  typedef std::tuple<ElementLink<xAOD::TruthEventBaseContainer>, float, float> VertexTruthMatchInfo;

  //type codes for vertex matching on all vertices
  enum VertexMatchType {
    NOTYPE,  // vertex is not secondary
    MATCHED, // > threshold (default 70%) from one truth interaction
    MERGED,  // not matched
    SPLIT,   // highest weight truth interaction contributes to >1 vtx (vtx with highest fraction of sumpT2 remains matched/merged)
    FAKE,    // highest contribution is fake (if pile-up MC info not present those tracks end up as "fakes")
    DUMMY,   // is the dummy vertex
    NTYPES   // access to number of types
  };

}


#endif
