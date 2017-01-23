/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef InDetVertexTruthMatchUtils_h
#define InDetVertexTruthMatchUtils_h

#include "xAODTracking/VertexContainerFwd.h"
#include "xAODTruth/TruthEventBaseContainer.h"

namespace InDetVertexTruthMatchUtils {

  //Namespace for useful analysis things on the truth matching decorations applied to the VertexContainer
  //Can be called by algorithms inside or outside Athena to do their analysis

  //How the matching info is stored; link to the truth vertex and a float with its contribution to the relative track weight
  typedef std::pair<ElementLink<xAOD::TruthEventBaseContainer>, float> VertexTruthMatchInfo;

  //type codes for vertex matching on all vertices
  enum VertexMatchType {
    MATCHED, // > threshold (default 70%) from one truth interaction
    MERGED,  // not matched
    SPLIT,   // matched or merged, but highest weight interaction is split
    FAKE,    // highest contribution is fake (if pile-up MC info not present those tracks end up as "fakes")
    DUMMY,   // is the dummy vertex
    NTYPES   // access to number of types
};

  //Classification for true hard-scatter interaction
  enum HardScatterType {
    CLEAN,   // matched
    LOWPU,   // merged, but dominant contributor
    HIGHPU,  // merged and dominated by pile-up or fakes
    HSSPLIT, // split
    NONE,    // not found
    NHSTYPES
  };

  //Find the best reco vertex matching the hard scatter event
  //returns 0 in case can't find one
  const xAOD::Vertex * bestHardScatterMatch( const xAOD::VertexContainer & vxContainer );

  //pointers to all reco vertices for which the hard scatter has some contribution
  //includes the index in the matching info where to find it
  std::vector<std::pair<const xAOD::Vertex*, size_t> > hardScatterMatches( const xAOD::VertexContainer & vxContainer );

  //find the hard scatter type
  HardScatterType classifyHardScatter( const xAOD::VertexContainer & vxContainer );

}


#endif
