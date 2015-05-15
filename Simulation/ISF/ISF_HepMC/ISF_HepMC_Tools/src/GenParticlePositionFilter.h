/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenParticlePositionFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_GENPARTICLEPOSITIONFILTER_H
#define ISF_HEPMC_GENPARTICLEPOSITIONFILTER_H 1

// STL includes
#include <string>
#include <vector>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

// ISF includes
#include "ISF_HepMC_Interfaces/IGenParticleFilter.h"

// ISF includes
#include "ISF_Interfaces/IGeoIDSvc.h"

namespace ISF {

  /** @class GenParticlePositionFilter
  
      Particle filter by position, to be used for initial GenEvent read-in.
  
      @author Andreas.Salzburger -at- cern.ch
     */
  class GenParticlePositionFilter : public AthAlgTool,
                                    virtual public IGenParticleFilter { 
      
    public: 
      //** Constructor with parameters */
      GenParticlePositionFilter( const std::string& t, const std::string& n, const IInterface* p );
      
      /** Destructor */
      ~GenParticlePositionFilter(){}

      /** Athena algtool's Hooks */
      StatusCode  initialize();
      StatusCode  finalize();

      /** does the given particle pass the filter? */
      bool pass(const HepMC::GenParticle& particle) const;
	  
	private:
      ServiceHandle<IGeoIDSvc>          m_geoIDSvc;
      std::vector<int>                  m_checkRegion;
  }; 
  
}


#endif //> !ISF_HEPMC_GENPARTICLEPOSITIONFILTER_H
