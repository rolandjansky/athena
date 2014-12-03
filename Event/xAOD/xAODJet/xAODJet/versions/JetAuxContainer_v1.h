// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetAuxContainer_v1.h 631914 2014-11-28 16:08:50Z krasznaa $
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
  /// $Revision: 631914 $
  /// $Date: 2014-11-28 17:08:50 +0100 (Fri, 28 Nov 2014) $
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

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::JetAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODJET_VERSIONS_JETAUXCONTAINER_V1_H
