/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IGenParticleFilter.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_HEPMC_INTERFACES_IGENPARTICLEFILTER_H
#define ISF_HEPMC_INTERFACES_IGENPARTICLEFILTER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// Simulation includes
#include "ISF_Event/SimSvcID.h" 

namespace HepMC {
    class GenParticle;
} 
 
namespace ISF {
 
  static const InterfaceID IID_IGenParticleFilter("IGenParticleFilter", 1, 0);
   
  /**
   @class IGenParticleFilter
      
   Interface definition for HepMC::GenParticle filter,
   these filters should be primarily be used in the stack filling process      
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IGenParticleFilter : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IGenParticleFilter(){}

       /** AlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IGenParticleFilter; }
       
       /** Returns a boolean if the particle has passed or not */
       virtual bool pass(const HepMC::GenParticle& particle) const = 0;
       
  };

} // end of namespace

#endif // ISF_HEPMC_INTERFACES_IGENPARTICLEFILTER_H
