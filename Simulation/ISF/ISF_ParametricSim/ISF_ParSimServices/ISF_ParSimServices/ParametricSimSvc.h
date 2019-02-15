/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParametricSimSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_PARSIMSERVICES_PARAMETRICSIMSVC_H
#define ISF_PARSIMSERVICES_PARAMETRICSIMSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthService.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"
#include "ISF_Interfaces/IParticleProcessor.h"

// Trk
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

// Random Number Generation
#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "CLHEP/Random/RandomEngine.h"

namespace ISF {
    class ISFParticle;
}

namespace iParSim {

  /** @class ParametricSimSvc
 
      The ParSimService
 
      @author Andreas.Salzburger -at- cern.ch
     */
    
  class ParametricSimSvc : public ISF::BaseSimulationSvc { 

    public: 
      //** Constructor with parameters */
      ParametricSimSvc( const std::string& name, ISvcLocator* pSvcLocator );
     
      /** Destructor */
      virtual ~ParametricSimSvc(); 
     
      /** Athena algorithm's interface methods */
      StatusCode  initialize();
      StatusCode  finalize();

      /** Simulation Call  */
      StatusCode simulate(const ISF::ISFParticle& isp, McEventCollection* mcEventCollection);
                                                                 
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent();

      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent();

    private:     
      /** Default constructor */
      ParametricSimSvc();

      /** xAOD::Particle creation - given by jobOptions  */
      ToolHandleArray<ISF::IParticleProcessor>     m_particleSmearers;   //!< the multiple particle smearers

      /** Trk Tracking Geometry Service   */
      ServiceHandle<Trk::ITrackingGeometrySvc>  m_trackingGeometrySvc;  //!< ServiceHandle to the TrackingGeometrySvc
      
      /** Random Service and Engine */
      ServiceHandle<IAthRNGSvc>  m_randomSvc{this, "RndmSvc", "AthRNGSvc", ""};  //!< Random number service
      ATHRNG::RNGWrapper*        m_randomEngineWrapper;                          //!< Random number engine wrapper
      std::string                m_randomEngineName;                             //!< Name of the random number stream


  }; 
}


#endif //> !ISF_PARSIMSERVICES_PARAMETRICSIMSVC_H
