// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IParticleHelpers.h 618909 2014-09-29 10:16:52Z krasznaa $
#ifndef XAODBASE_IPARTICLEHELPERS_H
#define XAODBASE_IPARTICLEHELPERS_H

// Local include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"

namespace xAOD {

   /// Function setting a link on a deep copy back to its original object
   bool setOriginalObjectLink( const IParticle& original,
                               IParticle& copy );

   /// Function setting links on a deep/shallow copy container to the originals
   bool setOriginalObjectLink( const IParticleContainer& original,
                               IParticleContainer& copy );

   /// Function getting a pointer to the original object from a deep/shallow copy
   const IParticle* getOriginalObject( const IParticle& copy );

} // namespace xAOD

#endif // XAODBASE_IPARTICLEHELPERS_H
