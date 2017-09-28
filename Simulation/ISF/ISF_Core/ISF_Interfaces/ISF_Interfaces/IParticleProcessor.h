/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleProcessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IPARTICLEPROCESSOR_H
#define ISF_FATRASINTERFACES_IPARTICLEPROCESSOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace ISF {
  
  class ISFParticle;    
    
  /** 
   @class IParticleProcessor

   universal processor tool, e.g. particle transport tool

   - return of new ISFParticle on the next detector boundary
   - return 0 indicates that the particle did not reach the associated detector boundary
       
   @author Andreas.Salzburger -at- cern.ch
   
   */
      
  class IParticleProcessor : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IParticleProcessor(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IParticleProcessor, 1, 0);

       /** Creates a new ISFParticle from a given ParticleState, 
          universal transport tool */
       virtual ISF::ISFParticle* process(const ISF::ISFParticle& isp) = 0;
  };

} // end of namespace

#endif 

