/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleFinalStateFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENPARTICLESIMWHITELIST_H
#define ISF_HEPMC_GENPARTICLESIMWHITELIST_H 1

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// STL includes
#include <string>
#include <vector>

namespace ISF {

    class ISFParticle;

      /** @class GenParticleSimWhiteList
 
       Stable/Interacting particle filter for HepMC particles to be used in the
       stack filling process.  Checks this particle and all daughters.
 
       @author ZLMarshall -at- lbl.gov
      */
  class GenParticleSimWhiteList : public extends<AthAlgTool, IGenParticleFilter> {

    public:
      //** Constructor with parameters */
      GenParticleSimWhiteList( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~GenParticleSimWhiteList(){}

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** passes through to the private version */
      bool pass(const HepMC::GenParticle& particle ) const;

     private:
      /** returns true if the the particle and all daughters are on the white list */
      bool pass(const HepMC::GenParticle& particle , std::vector<int> & used_vertices ) const;
      std::vector<std::string>          m_whiteLists;            //!< The location of the white lists
      std::vector<long int>             m_pdgId;                //!< Allowed PDG IDs
      bool                              m_qs;                   //!< Switch for quasi-stable particle simulation
      double m_minDecayRadiusQS;                         //!< Decay radius below which QS particles should be ignored
  };

}


#endif //> !ISF_HEPMC_GENPARTICLESIMWHITELIST_H
