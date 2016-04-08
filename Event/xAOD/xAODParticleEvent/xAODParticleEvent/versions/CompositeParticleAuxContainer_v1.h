// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticleAuxContainer_v1.h 650425 2015-02-27 16:33:21Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLEAUXCONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLEAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODParticleEvent/versions/CompositeParticle_v1.h"
#include "xAODMissingET/MissingETContainer.h"


namespace xAOD {

  /// Auxiliary container for composite particle containers
  ///
  /// This is defining every possible property that
  /// composite particles can have.
  ///
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revsision$
  /// $Date: 2015-02-27 17:33:21 +0100 (Fri, 27 Feb 2015) $
  ///
  class CompositeParticleAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    CompositeParticleAuxContainer_v1();
    /// Destructor
    ~CompositeParticleAuxContainer_v1() {}



  private:
    /// @name iParticle values
    /// @{
    std::vector< float >              px;
    std::vector< float >              py;
    std::vector< float >              pz;
    std::vector< float >              e;
    /// @}

    /// @name particle property values
    /// @{
    std::vector< float >              charge;
    std::vector< int >                pdgId;
    /// @}

    /// @name Links
    /// @{
    std::vector< xAOD::IParticleLinkContainer >            partLinks;
    std::vector< ElementLink< xAOD::MissingETContainer > > missingETLink;
    /// @}

  }; // class CompositeParticleAuxContainer_v1

} // namespace xAOD


// Set up StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::CompositeParticleAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLEAUXCONTAINER_V1_H
