/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FatrasSimSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASSIMSVC_H
#define ISF_FATRASSIMSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// Tracking includes
#include "TrkTrack/TrackCollection.h"

namespace ISF {
    class IParticleFilter;
    class IParticleProcessor;
    class ISFParticle;
    class ITruthSvc;
}

namespace iFatras {

  /** @class FatrasSimSvc
  
      @author Andreas.Salzburger -at- cern.ch
     */
  class FatrasSimSvc : public ISF::BaseSimulationSvc { 

    public: 
      //** Constructor with parameters */
      FatrasSimSvc( const std::string& name, ISvcLocator* pSvcLocator );
      
      /** Destructor */
      virtual ~FatrasSimSvc(); 
      
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
      /** Default constructor */
      FatrasSimSvc();

      /** Track Creation & transport */
      ToolHandle<ISF::IParticleProcessor>  m_IDsimulationTool;   //!< Pointer to the transport AlgTool
      bool                                 m_useExtrapolator;  //!< Boolean used to run with the old extrapolator setup
      ToolHandle<ISF::IParticleProcessor>  m_simulationTool;   //!< Pointer to the transport AlgTool
      ToolHandle<ISF::IParticleFilter>     m_particleFilter;   //!< the particle filter concerning kinematic cuts, etc.

  }; 
}


#endif //> !ISF_FatrasSimSvc_H
