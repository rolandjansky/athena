// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexAuxContainer_v1.h 613511 2014-08-26 15:37:45Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHVERTEXAUXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHVERTEXAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

// Local include(s):
#include "xAODTruth/TruthParticleContainer.h"

namespace xAOD {

  /// Auxiliary store for the truth vertices
  ///
  /// @author Andy Buckley <Andy.Buckey@cern.ch>
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  ///
  /// $Revision: 613511 $
  /// $Date: 2014-08-26 17:37:45 +0200 (Tue, 26 Aug 2014) $
  ///
  class TruthVertexAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    TruthVertexAuxContainer_v1();

  private:
    std::vector< int > id;
    std::vector< int > barcode;
    std::vector< std::vector< ElementLink< TruthParticleContainer > > >
    incomingParticleLinks;
    std::vector< std::vector< ElementLink< TruthParticleContainer > > >
    outgoingParticleLinks;
    std::vector< float > x;
    std::vector< float > y;
    std::vector< float > z;
    std::vector< float > t;
    std::vector< std::vector<float> > weights; // weights for this event first weight

  }; // class TruthVertexAuxContainer_v1

} // namespace xAOD

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthVertexAuxContainer_v1, 1254939514, 1 )
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthVertexAuxContainer_v1, xAOD::AuxContainerBase ); 

#endif // XAODTRUTH_VERSIONS_TRUTHVERTEXAUXCONTAINER_V1_H
