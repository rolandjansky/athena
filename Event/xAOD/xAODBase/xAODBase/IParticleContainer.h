// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleContainer.h 567380 2013-10-28 10:48:26Z krasznaa $
#ifndef XAODBASE_IPARTICLECONTAINER_H
#define XAODBASE_IPARTICLECONTAINER_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODBase/IParticle.h"

namespace xAOD {

   /// Simple convenience declaration of IParticleContainer
   ///
   /// Note that this structure should be used with care. It should
   /// mainly be used in tool interfaces, and nowhere else. For instance
   /// it is possible to put view containers of this type into StoreGate,
   /// but it is not possible to write out an object of this type into
   /// an output file.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 567380 $
   /// $Date: 2013-10-28 11:48:26 +0100 (Mon, 28 Oct 2013) $
   ///
   typedef DataVector< IParticle > IParticleContainer;

} // namespace xAOD

// To make it possible to put IParticleContainers into StoreGate:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::IParticleContainer, 1241842700, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODBASE_IPARTICLECONTAINER_H
