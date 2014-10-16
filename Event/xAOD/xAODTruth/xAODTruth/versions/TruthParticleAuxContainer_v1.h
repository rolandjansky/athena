// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPARTICLEAUXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPARTICLEAUXCONTAINER_V1_H

#include <vector>

#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"

#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

namespace xAOD {


  /// Auxiliary store for the truth vertices
  ///
  /// @author Andy Buckley <Andy.Buckey@cern.ch>
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  /// @author Jovan Mitrevski <Jovan.Mitrevski@cern.h>
  ///
  class TruthParticleAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    TruthParticleAuxContainer_v1();

  private:
    std::vector< int > pdgId;
    std::vector< int > barcode;
    std::vector< int > status;
    std::vector< ElementLink< TruthVertexContainer > > prodVtxLink;
    std::vector< ElementLink< TruthVertexContainer > > decayVtxLink;
    std::vector< float > px;
    std::vector< float > py;
    std::vector< float > pz;
    std::vector< float > e;
    std::vector< float > m; // needed since not necessarily on shell

  }; // class TruthParticleAuxContainer_v1

} // namespace xAOD


// StoreGate registration
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::TruthParticleAuxContainer_v1, xAOD::AuxContainerBase );


#endif // XAODTRUTH_VERSIONS_TRUTHPARTICLEAUXCONTAINER_V1_H
