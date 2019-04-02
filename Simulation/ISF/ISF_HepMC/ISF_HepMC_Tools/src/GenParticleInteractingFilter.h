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
  class GenParticleInteractingFilter final : public extends<AthAlgTool, IGenParticleFilter> {

    public:
      //** Constructor with parameters */
      GenParticleInteractingFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~GenParticleInteractingFilter(){}

      /** Framework methods */
      virtual StatusCode initialize() override;

      /** passes through to the private version */
      virtual bool pass(const HepMC::GenParticle& particle ) const override;

      /** Additional PDG codes to classify as interacting */
      std::vector<int> m_additionalInteractingParticleTypes;

      /** Additional PDG codes to classify as non-interacting */
      std::vector<int> m_additionalNonInteractingParticleTypes;
  };

}

#endif //> !ISF_HEPMC_GENPARTICLESIMWHITELIST_H
