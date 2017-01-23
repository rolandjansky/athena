/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetTruthVertexValidation/InDetVertexTruthMatchUtils.h"
#include "xAODTracking/VertexContainer.h"

namespace InDetVertexTruthMatchUtils {

  namespace {

    bool isHardScatterEvent( const ElementLink<xAOD::TruthEventBaseContainer> evlink ) {
      //good link of type "TruthEvent" (not truthpileupevent) and the first one in the collection
      return ( evlink.isValid() && (*evlink)->type() == xAOD::Type::TruthEvent && evlink.index() == 0 );
    }

  }
  
  //find highest weight match to the hard scatter interaction
  //in the case of splitting user can take care to check this is really "best"
  // --> example:
  // vertex 0 is 90% hard scatter with many tracks, and 10% another vertex
  // vertex 1 is 100% hard scatter with few tracks.  
  // this algorithm will pick vertex 1, but user may want to use vertex 0 (accessible from splitting link decoration)
  const xAOD::Vertex * bestHardScatterMatch( const xAOD::VertexContainer & vxContainer ) {
    //accessors for decorations
    xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > matchInfoDecor("TruthEventMatchingInfos");

    std::vector<std::pair<const xAOD::Vertex*, size_t> > allMatches = hardScatterMatches( vxContainer );

    //look for the highest weight
    float bestw = 0.;
    const xAOD::Vertex * best = 0;
    for( auto it : allMatches ) {
      std::vector<VertexTruthMatchInfo> & info = matchInfoDecor( *(it.first) );
      //go to the index of the HS match in the info vector, and check the weight
      if( info[ it.second ].second > bestw ) {
        bestw = info[ it.second ].second;
        best = it.first;
      }
    }
   
    //if didn't find any matches will return 0
    return best;
  }

  //Find all hard scatter matches
  std::vector<std::pair<const xAOD::Vertex*, size_t> > hardScatterMatches( const xAOD::VertexContainer & vxContainer ) {
    //accessors for decorations
    xAOD::Vertex::Decorator<std::vector<VertexTruthMatchInfo> > matchInfoDecor("TruthEventMatchingInfos");

    //return vector
    std::vector<std::pair<const xAOD::Vertex*, size_t> > result;

    //loop and look
    for( auto vxit : vxContainer ) {
      const std::vector<VertexTruthMatchInfo> & info =  matchInfoDecor( *vxit );
      for( size_t i=0; i< info.size(); ++i ) {
        if( isHardScatterEvent( info[i].first ) ) {
          result.push_back( std::make_pair(vxit, i) );
          break;
        }
      }
    
    }
    return result;
  }

  HardScatterType classifyHardScatter( const xAOD::VertexContainer & vxContainer ) {

    // return a vector of reco vertices with contributions from the hard-scatter
    // if the second element in the pair is zero, it means tracks from the hard-scatter
    // contribute more weight to that reco vertex than any other interaction
    std::vector<std::pair<const xAOD::Vertex*, size_t> > matches = hardScatterMatches( vxContainer );

    // information indicating whether a given reco vertex is:
    // MATCHED, MERGED, SPLIT or FAKE
    xAOD::Vertex::Decorator<VertexMatchType> matchTypeDecor("VertexMatchType");

    if(  matches.size() == 0 ) {
      // No reco vertices with tracks from hard-scatter
      return NONE;
    } else if( matches.size() == 1 ) {
      // A single reco vertex has tracks from hard-scatter
      const VertexMatchType & type = matchTypeDecor( *(matches[0].first) );
      //check if the index in the matching info for HS is first, then can assign clean/lowpu based on match/merge
      if( matches[0].second == 0 && type == MATCHED ) {
	// HS made largest contribution and vertex matched
        return CLEAN;
      } else if ( matches[0].second == 0 && type == MERGED ) {
	// HS made largest contribution and vertex merged
        return LOWPU;
      } else {
	// HS did not make largest contribution or vertex is fake or vertex is part of non-HS split pair
        return HIGHPU;
      }

    } else {
      // multiple reco vertices have tracks from hard-scatter
      // count how many have hard-scatter tracks as largest contribution
      int num_main = 0;
      const xAOD::Vertex* mainVtx = 0;
      for( auto it : matches ) {
        if( it.second == 0 ) // is hard-scatter the largest contribution?
	{
          num_main++;
	  mainVtx = it.first;
	}
      }
      if(num_main == 0 ) {
        return HIGHPU;
      } else if (num_main == 1 ) {
	const VertexMatchType& type = matchTypeDecor(*mainVtx);
	if (type == MATCHED)
	  return CLEAN;
	else if (type == MERGED)
	  return LOWPU;
	else
	  return HIGHPU;
      } else {
        return HSSPLIT;
      }
    }

    return NONE;
  }

}
