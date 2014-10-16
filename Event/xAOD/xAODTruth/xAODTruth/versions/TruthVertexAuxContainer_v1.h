// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexAuxContainer_v1.h 622081 2014-10-16 00:55:18Z abuckley $
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
  /// $Revision: 622081 $
  /// $Date: 2014-10-16 02:55:18 +0200 (Thu, 16 Oct 2014) $
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


// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthVertexAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODTRUTH_VERSIONS_TRUTHVERTEXAUXCONTAINER_V1_H
