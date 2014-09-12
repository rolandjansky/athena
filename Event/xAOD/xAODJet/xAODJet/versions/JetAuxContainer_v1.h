// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetAuxContainer_v1.h 596642 2014-05-12 16:54:56Z delsart $
#ifndef XAODJET_VERSIONS_JETAUXCONTAINER_V1_H
#define XAODJET_VERSIONS_JETAUXCONTAINER_V1_H

// System include(s):
#include <vector>

// ROOT include(s):
#include <Math/Vector4D.h>

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODJet/JetTypes.h"

namespace xAOD {

  /// Temporary container used until we have I/O for AuxStoreInternal
  ///
  /// This class is meant to serve as a temporary way to provide an auxiliary
  /// store with Athena I/O capabilities for the jet EDM. Will be exchanged for
  /// a generic auxiliary container type (AuxStoreInternal) later on.
  ///
  /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
  ///
  /// $Revision: 596642 $
  /// $Date: 2014-05-12 18:54:56 +0200 (Mon, 12 May 2014) $
  ///
  class JetAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    JetAuxContainer_v1();

  private:
    /// Type used for the constituent links:
    typedef std::vector< std::vector< ElementLink< IParticleContainer > > >
    ConstituentLinks_t;

    //std::vector< JetFourMom_t > momentum;
    std::vector<float> pt;
    std::vector<float> eta;
    std::vector<float> phi;
    std::vector<float> m;

    ConstituentLinks_t constituentLinks;
    std::vector< std::vector< float > > constituentWeights;

  }; // class JetAuxContainer_v1

} // namespace xAOD

// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::JetAuxContainer_v1, 1294631886, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::JetAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODJET_VERSIONS_JETAUXCONTAINER_V1_H
