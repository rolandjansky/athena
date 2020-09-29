/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleFinalStateFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENPARTICLEFINALSTATEFILER_H
#define ISF_HEPMC_GENPARTICLEFINALSTATEFILER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

namespace ISF {

    class ISFParticle;

  /** @class GenParticleFinalStateFilter

      Stable/Interacting particle filter for HepMC particles to be used in the
      stack filling process.

      @author Andreas.Salzburger -at- cern.ch
     */
  class GenParticleFinalStateFilter : public extends<AthAlgTool, IGenParticleFilter> {

    public:
      //** Constructor with parameters */
      GenParticleFinalStateFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~GenParticleFinalStateFilter(){}

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

#ifdef HEPMC3
      /** Returns the Particle Stack, should register truth */
      bool pass(HepMC::ConstGenParticlePtr particle) const;
        private:
      /** checks if the particle is in its final state (no end vertex) */
      bool isFinalState( HepMC::ConstGenParticlePtr p) const;
#else

      /** Returns the Particle Stack, should register truth */
      bool pass(const HepMC::GenParticle& particle) const;

        private:
      /** checks if the particle is in its final state (no end vertex) */
      bool isFinalState( const HepMC::GenParticle& p) const;
#endif

      bool                              m_checkGenSimStable;    //!< boolean switch to check on sim stable
      bool                              m_checkGenInteracting;  //!< boolean switch to check on gen interacting

  };

}


#endif //> !ISF_HEPMC_GENPARTICLEFINALSTATEFILER_H
