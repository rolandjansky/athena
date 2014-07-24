// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticleAuxContainer_v1.h 606326 2014-07-14 14:12:20Z kkoeneke $
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
  /// $Date: 2014-07-14 16:12:20 +0200 (Mon, 14 Jul 2014) $
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
    std::vector< xAOD::IParticleLinkContainer >            constituentLinks;
    std::vector< ElementLink< xAOD::MissingETContainer > > missingETLink;
    /// @}

  }; // class CompositeParticleAuxContainer_v1

} // namespace xAOD


// Set up a CLID and StoreGate inheritance for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::CompositeParticleAuxContainer_v1, 1205246726, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE( xAOD::CompositeParticleAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif // XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLEAUXCONTAINER_V1_H
