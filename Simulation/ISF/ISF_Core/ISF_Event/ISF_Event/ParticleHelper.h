/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_EVENT_PARTICLEHELPER_H
#define ISF_EVENT_PARTICLEHELPER_H 1

// forward declarations
#include "AtlasHepMC/GenParticle_fwd.h"

namespace ISF {

  class ISFParticle;

  /**
   @class ParticleHelper

   A simple helper class to perform commonly used, basic functions, like
     * converting ISFParticles into a differnt particle representation format

   @author Elmar.Ritsch -at- cern.ch
   */

  class ParticleHelper {

    public:
        /** empty constructor */
        ParticleHelper() {} ;

        /** empty destructor */
        ~ParticleHelper() {} ;

        /** convert the given particle to HepMC format */
        static HepMC::GenParticlePtr convert( const ISF::ISFParticle &p);
        
    private :
  };

} // end of namespace

/* implementation for inlined methods (none existing at the moment) */
//#include <ISF_Event/ParticleHelper.icc>

#endif // ISF_EVENT_PARTICLEHELPER_H

