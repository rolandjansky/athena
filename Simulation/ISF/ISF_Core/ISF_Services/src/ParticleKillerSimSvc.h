/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleKillerSimSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARTICLEKILLERSIMSVC_H
#define ISF_PARTICLEKILLERSIMSVC_H 1

// STL includes
#include <string>

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

namespace ISF {

  /** @class ParticleKillerSimSvc
      
      This ISF SimulationService does literally nothing with the particles
      handed over in the simulate(...) call. Thus those particles will be
      removed from further simulation in ISF.
  
      @author Elmar.Ritsch -at- cern.ch
     */
  class ParticleKillerSimSvc : public ISF::BaseSimulationSvc { 

    public: 
      //** Constructor with parameters */
      ParticleKillerSimSvc( const std::string& name, ISvcLocator* pSvcLocator );
      
      /** Destructor */
      virtual ~ParticleKillerSimSvc(); 
      
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();

      /** Simulation Call  */
      StatusCode simulate(const ISF::ISFParticle& isp);
                                                                 
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();

      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();

    private:     
  }; 
}


#endif //> !ISF_PARTICLEKILLERSIMSV_H
