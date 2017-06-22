/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHitsCluster_v1.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-18
 * 
 * @brief  Implementation file of the AFPSiHitsCluster_v1 class.
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPSiHitsCluster_v1.h"


namespace xAOD
{
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, int, pixelLayerID, setPixelLayerID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, xLocal, setXLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, xLocalErr, setXLocalErr)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, yLocal, setYLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, yLocalErr, setYLocalErr)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, zLocal, setZLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, zLocalErr, setZLocalErr)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, float, depositedCharge, setDepositedCharge)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, int, nHits, setNHits)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHitsCluster_v1, int, clusterAlgID, setClusterAlgID )

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPSiHitsCluster_v1, std::vector<AFPSiHitsCluster_v1::AFPHitLink_t>,  hitsLinks, setHitsLinks)

  static SG::AuxElement::Accessor< std::vector<AFPSiHitsCluster_v1::AFPHitLink_t> > hitsLinksAcc( "hitsLinks" );
  
  void AFPSiHitsCluster_v1::addHitLink( const AFPSiHitsCluster_v1::AFPHitLink_t& link )
  {
    hitsLinksAcc( *this ).push_back( link );
  }

  void AFPSiHitsCluster_v1::toPersistent() {
    // Prepare the hits links for persistification:
    if ( hitsLinksAcc.isAvailableWritable( *this ) )
      for (AFPHitLink_t hitLink : hitsLinksAcc( *this ) )
	hitLink.toPersistent();
  }
  
}
