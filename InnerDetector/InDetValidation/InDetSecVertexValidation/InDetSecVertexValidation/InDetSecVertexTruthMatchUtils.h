/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetSecVertexTruthMatchUtils_h
#define InDetSecVertexTruthMatchUtils_h

#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace InDetSecVertexTruthMatchUtils {

  //Namespace for useful analysis things on the truth matching decorations applied to the VertexContainer
  //Can be called by algorithms inside or outside Athena to do their analysis

  //How the matching info is stored; link to the truth vertex, a float with its contribution to the relative track weight, and a float with its contribution to the track sumpt2 of the truth vertex
  typedef std::tuple<ElementLink<xAOD::TruthVertexContainer>, float, float> VertexTruthMatchInfo;

  //type codes for vertex matching on all vertices
  enum VertexMatchType {
    MATCHED, // > threshold (default 70%) from one truth interaction
    MERGED,  // not matched
    SPLIT,   // highest weight truth interaction contributes to >1 vtx (vtx with highest fraction of sumpT2 remains matched/merged)
    FAKE,    // highest contribution is fake (if pile-up MC info not present those tracks end up as "fakes")
    NTYPES   // access to number of types
  };

  //type codes for truth vertices
  //NOTE: types are subsets of subsequent types
  enum TruthVertexMatchType {
    INCLUSIVE, // all vertices
    RECONSTRUCTABLE, // fiducial cuts, >= 2 charged daughters
    ACCEPTED, // >= 2 reco tracks
    SEEDED, // tracks pass cuts
    RECONSTRUCTED, // matched to reco vtx
  };

}


#endif
