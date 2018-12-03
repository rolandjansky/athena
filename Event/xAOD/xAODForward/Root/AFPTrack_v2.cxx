/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrack_v2.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Implementation file for class xAOD::AFPTrack_v2
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPSiHitsClusterContainer_v1.h"
#include "xAODForward/versions/AFPTrack_v2.h"

namespace xAOD
{
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, float, xLocal, setXLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, float, yLocal, setYLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, float, zLocal, setZLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, float, xSlope, setXSlope)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, float, ySlope, setYSlope)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, int, nHoles, setNHoles)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, int, nClusters, setNClusters)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, float, chi2, setChi2)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v2, int, algID, setAlgID)

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPTrack_v2, std::vector< AFPTrack_v2::AFPClusterLink_t >, clusters, setClusters)
  static SG::AuxElement::Accessor< std::vector<AFPTrack_v2::AFPClusterLink_t> > clustersAcc( "clusters" );

  void AFPTrack_v2::addCluster( const AFPClusterLink_t& link )
  {
    clustersAcc( *this ).push_back( link );
  }

  void AFPTrack_v2::toPersistent() {
    // Prepare the clusters links for persistification:
    if( clustersAcc.isAvailableWritable( *this ) ) {
      std::vector<AFPTrack_v2::AFPClusterLink_t>::iterator end = clustersAcc( *this ).end();
      for(std::vector<AFPTrack_v2::AFPClusterLink_t>::iterator itr = clustersAcc( *this ).begin(); itr != end; ++itr )
	itr->toPersistent();

    }
  }
  
}
