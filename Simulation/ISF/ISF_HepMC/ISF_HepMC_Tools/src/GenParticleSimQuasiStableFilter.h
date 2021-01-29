/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticleFinalStateFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GenParticleSimQuasiStableFilter_H
#define ISF_HEPMC_GenParticleSimQuasiStableFilter_H 1

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// STL includes
#include <string>
#include <vector>

// Units
#include "GaudiKernel/SystemOfUnits.h"

namespace ISF {

    class ISFParticle;

      /** @class GenParticleSimQuasiStableFilter
 
       Quasi stable particle and their daughter filter for HepMC particles to be used in the
       stack filling process.  Checks this particle and all daughters.
      */
  class GenParticleSimQuasiStableFilter : public extends<AthAlgTool, IGenParticleFilter> {

    public:
      //** Constructor with parameters */
      GenParticleSimQuasiStableFilter( const std::string& t, const std::string& n, const IInterface* p );

      /** Destructor */
      ~GenParticleSimQuasiStableFilter(){}

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** passes through to the private version */
#ifdef HEPMC3
      bool pass(HepMC::ConstGenParticlePtr particle ) const;
#else
      bool pass(const HepMC::GenParticle& particle ) const;
#endif

     private:
      /** returns true if the the particle and all daughters are on the white list */
      std::vector< int >    m_statuscode = {1,2};                 //!< status codes to accept
      std::vector< double > m_minProdRadius = {22*Gaudi::Units::mm,0*Gaudi::Units::mm};           //!< minimum production radius below which particles should be ignored
      std::vector< double > m_minDecayRadius = {22*Gaudi::Units::mm,22*Gaudi::Units::mm};         //!< minimum decay radius below which particles should be ignored
  };

}


#endif //> !ISF_HEPMC_GenParticleSimQuasiStableFilter_H
