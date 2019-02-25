/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BTagging_v1$

// EDM include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"
// Local include(s):
#include "xAODBTagging/versions/BTagVertex_v1.h"
//#include "BTagVertexAccessors_v1.h"


namespace xAOD {

  BTagVertex_v1::BTagVertex_v1(): SG::AuxElement() {
  }

  // Implementation of "basic" accessors 

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagVertex_v1, float, chi2, setChi2 )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagVertex_v1, float, NDF, setNDF )
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BTagVertex_v1, float, pv_compatibility, setPV_compatibility )
  AUXSTORE_OBJECT_SETTER_AND_GETTER( BTagVertex_v1, std::vector<float>, compToOtherSV, setCompToOtherSV)

  
  // Implementation of accessors for the constituent tracks

  AUXSTORE_OBJECT_SETTER_AND_GETTER( BTagVertex_v1, std::vector<float>, track_chi2, setTrack_chi2)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( BTagVertex_v1, std::vector<float>, track_NDF, setTrack_NDF)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( BTagVertex_v1, std::vector<float>, track_refPx, setTrack_refPx)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( BTagVertex_v1, std::vector<float>, track_refPy, setTrack_refPy)
  AUXSTORE_OBJECT_SETTER_AND_GETTER( BTagVertex_v1, std::vector<float>, track_refPz, setTrack_refPz)

  static const SG::AuxElement::Accessor<std::vector< ElementLink< xAOD::TrackParticleContainer > > > track_linkAcc( "track_links" );

  const std::vector<ElementLink<xAOD::TrackParticleContainer> >& BTagVertex_v1::track_links() const {
    return track_linkAcc( *this );
  }

  void BTagVertex_v1::setTrack_links(const std::vector<ElementLink<xAOD::TrackParticleContainer> >& value) {
    track_linkAcc( *this ) = value;
    return;
  }



  void BTagVertex_v1::toPersistent() {
   
   // if (reco_vertexLinkAcc.isAvailableWritable(*this)) {
   //   std::vector<ElementLink<xAOD::BTagVertexContainer> >::iterator itEnd = reco_vertexLinkAcc(*this).end();
   //   for (std::vector<ElementLink<xAOD::BTagVertexContainer> >::iterator it = reco_vertexLinkAcc(*this).begin(); it != itEnd; ++it) {
   //     it->toPersistent();
   //   }  
   // } 

    if (track_linkAcc.isAvailableWritable(*this)) {
      std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator itEnd = track_linkAcc(*this).end();
      for (std::vector<ElementLink<xAOD::TrackParticleContainer> >::iterator it = track_linkAcc(*this).begin(); it != itEnd; ++it) {
	    it->toPersistent();
      }
    }
  }

}


