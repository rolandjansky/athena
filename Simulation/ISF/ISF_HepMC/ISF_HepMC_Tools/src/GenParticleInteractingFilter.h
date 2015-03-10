/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleFinalStateFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENPARTICLEINTERACTINGFILTER_H
#define ISF_HEPMC_GENPARTICLEINTERACTINGFILTER_H 1

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// STL includes
#include <string>

namespace ISF {

    class ISFParticle;

      /** @class GenParticleInteractingFilter
 
       Stable/Interacting particle filter for HepMC particles to be used in the
       stack filling process.  Checks this particle for interacting (not nu/G/LSP/whatever).
 
       @author ZLMarshall -at- lbl.gov
      */
      class GenParticleInteractingFilter : public AthAlgTool,
                              virtual public IGenParticleFilter {

    public:
      //** Constructor with parameters */
      GenParticleInteractingFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~GenParticleInteractingFilter(){}

      /** passes through to the private version */
      bool pass(const HepMC::GenParticle& particle ) const;

  };

}

#endif //> !ISF_HEPMC_GENPARTICLESIMWHITELIST_H
