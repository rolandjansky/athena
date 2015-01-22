// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ParticleAuxContainer_v1.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_PARTICLEAUXCONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_PARTICLEAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"
#include "xAODParticleEvent/versions/Particle_v1.h"


namespace xAOD {

  /// Auxiliary container for particle containers
  ///
  /// This is defining every possible property that
  /// particles can have.
  ///
  /// @author Karsten Koeneke <karsten.koeneke@cern.ch>
  ///
  /// $Revsision$
  /// $Date: 2014-12-19 17:02:19 +0100 (Fri, 19 Dec 2014) $
  ///
  class ParticleAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    ParticleAuxContainer_v1();
    /// Destructor
    ~ParticleAuxContainer_v1() {}



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

  }; // class ParticleAuxContainer_v1

} // namespace xAOD


// Set up StoreGate inheritance for the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::ParticleAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODPARTICLEEVENT_VERSIONS_PARTICLEAUXCONTAINER_V1_H
