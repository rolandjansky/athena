/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPVertex_v1.cxx
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-12-08
 * 
 * @brief  Implementation file for class xAOD::AFPVertex_v1
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPProtonContainer_v1.h"
#include "xAODForward/versions/AFPToFTrackContainer_v1.h"
#include "xAODForward/versions/AFPVertex_v1.h"

namespace xAOD
{
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPVertex_v1, float, position, setPosition)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPVertex_v1, float, distA, setDistA)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPVertex_v1, float, distC, setDistC)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPVertex_v1, int, algID, setAlgID)

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPVertex_v1, std::vector< AFPVertex_v1::AFPProtonLink_t >, protons, setProtons)
  static const SG::AuxElement::Accessor< std::vector<AFPVertex_v1::AFPProtonLink_t> > protonsAcc( "protons" );

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPVertex_v1, std::vector< AFPVertex_v1::AFPToFTrackLink_t >, tofTracks, setToFTracks)
  static const SG::AuxElement::Accessor< std::vector<AFPVertex_v1::AFPToFTrackLink_t> > tofTracksAcc( "tofTracks" );


  void AFPVertex_v1::addProton( const AFPProtonLink_t& link )
  {

    if (not this->isAvailable< std::vector<AFPVertex_v1::AFPProtonLink_t> >("protons") )
        protonsAcc( *this ) = std::vector<AFPVertex_v1::AFPProtonLink_t>();
    protonsAcc( *this ).push_back( link );
    return;
  }

  void AFPVertex_v1::addToFTrack( const AFPToFTrackLink_t& link )
  {
    if (not this->isAvailable< std::vector<AFPVertex_v1::AFPToFTrackLink_t> >("tofTracks") )
        tofTracksAcc( *this ) = std::vector<AFPVertex_v1::AFPToFTrackLink_t>();
    tofTracksAcc( *this ).push_back( link );
    return;
  }

  void AFPVertex_v1::toPersistent() {
    // Prepare the protons links for persistification:
    if( protonsAcc.isAvailableWritable( *this ) ) {
      for (AFPProtonLink_t link : protonsAcc( *this )) link.toPersistent();
    }
    // Prepare the tofTrackss links for persistification:
    if( tofTracksAcc.isAvailableWritable( *this ) ) {
       for (AFPToFTrackLink_t link : tofTracksAcc( *this )) link.toPersistent(); 
    }
      return;
  }

  
}
