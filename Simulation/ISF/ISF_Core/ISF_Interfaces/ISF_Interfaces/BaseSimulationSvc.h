/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// BaseSimulationSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_BASESIMULATIONSVC_H
#define ISF_BASESIMULATIONSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"


// ISF includes
#include "ISF_Interfaces/ISimulationSvc.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/SimSvcID.h"

namespace ISF {

  class IParticleBroker;
  class ITruthSvc;

  /** @class BaseSimulationSvc
  
      Concrete base class for all simulation services.

      It facilitates the use of event store and detector store, provides record and retrieve 
      methods and initializes the ChronoStatSvc.
      
      It implements a dummy callback from the detector Store after the GeoModelSvc has initialized
      the detector geometry.
  
      @author Michael.Duehrssen -at- cern.ch, Andreas.Salzburger -at- cern.ch, Elmar.Ritsch -at- cern.ch
     */
  class BaseSimulationSvc : public extends<AthService, ISimulationSvc> {
    public: 
      
      //** Constructor with parameters */
      BaseSimulationSvc( const std::string& name, ISvcLocator* pSvcLocator):
        base_class(name,pSvcLocator),
        m_evtStore( "StoreGateSvc/StoreGateSvc",  name ),
        m_detStore( "StoreGateSvc/DetectorStore", name ),
        m_simDescr(),
        m_screenOutputPrefix("isf >> "),
        m_chronoSvcName("ChronoStatSvc"),
        m_chrono(0),
        m_particleBroker(0)
      {
        // to be set to a unique descriptor
        declareProperty( "Identifier",
                         m_simDescr,
                         "A unique string to identify the simulator.");

        declareProperty( "EvtStore",
                         m_evtStore = StoreGateSvc_t ("StoreGateSvc", name),
                         "Handle to a StoreGateSvc instance: it will be used to retrieve data during the course of the job" );
        declareProperty( "DetStore",
                         m_detStore = StoreGateSvc_t ("StoreGateSvc/DetectorStore", name),
                         "Handle to a StoreGateSvc/DetectorStore instance: it will be used to retrieve data during the course of the job" );                        
        // refine the screen output for debugging
        declareProperty("ScreenOutputPrefix",   m_screenOutputPrefix);
        // Service handling
        declareProperty("ChronoStatService",    m_chronoSvcName     );
        
      };
      
      /** Destructor */
      virtual ~BaseSimulationSvc() {}; 
                  
      /** Gaudi sysInitialize() methods */
      StatusCode sysInitialize()
      { 
        if ( AthService::sysInitialize().isFailure() ) {
            ATH_MSG_FATAL( m_screenOutputPrefix << " Cannot initialize AthService! Abort.");
            return StatusCode::FAILURE;
        }
        if ( serviceLocator()->service(m_chronoSvcName, m_chrono ).isFailure()){
          ATH_MSG_FATAL( m_screenOutputPrefix << " Cannot retrieve ChronoStatSvc! Abort.");
          return StatusCode::FAILURE;
        }
        
        return StatusCode::SUCCESS; 
      }      

      /** Callback after geometry has been built - dummy implementation */
      virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS) 
      {
         ATH_MSG_VERBOSE( m_screenOutputPrefix << " Callback recieved after geometry setup.");  
         return StatusCode::SUCCESS; 
      }

      /** Return the simulation service descriptor */
      std::string& simSvcDescriptor() { return m_simDescr; }
                                 
      /** Setup Event chain - in case of a begin-of event action is needed */
      StatusCode setupEvent()
      { return StatusCode::SUCCESS; }

      /** Release Event chain - in case of an end-of event action is needed */
      StatusCode releaseEvent()
      { return StatusCode::SUCCESS; }

      /** Inform the SimulationSvc about the ParticleBroker svc */
      virtual StatusCode setParticleBroker( IParticleBroker *broker) {
        m_particleBroker = broker;
        return StatusCode::SUCCESS;
      }

      /** Simulation call for vectors of particles */
      virtual StatusCode simulateVector(const ConstISFParticleVector& particles) {
        // this implementation is a wrapper in case the simulator does
        // implement particle-vector input
        ConstISFParticleVector::const_iterator partIt    = particles.begin();
        ConstISFParticleVector::const_iterator partItEnd = particles.end();
        bool success = true;
        // simulate each particle individually
        for ( ; partIt != partItEnd; partIt++) {
          ATH_MSG_VERBOSE( m_screenOutputPrefix <<  "Starting simulation of particle: " << (*partIt) );
          if ( this->simulate(**partIt).isFailure()) {
            ATH_MSG_WARNING("Simulation of particle failed!" << endmsg <<
                            "   -> simulator: " << this->simSvcDescriptor() <<
                            "   -> particle : " << (ISFParticle&)(**partIt) );
            success = false;
          }
        }
        return ( success ) ? StatusCode::SUCCESS : StatusCode::FAILURE;
      }

      /** Simulation call for individual particles */
      virtual StatusCode simulate(const ISFParticle& isp);

      /** wrapper call to start chrono with given tag */
      const ChronoEntity* chronoStart(const IChronoSvc::ChronoTag& tag ) {
        if (m_chrono) return m_chrono->chronoStart( tag);
      }

      /** wrapper call to stop chrono with given tag */
      const ChronoEntity* chronoStop(const IChronoSvc::ChronoTag& tag ) {
        if (m_chrono) return m_chrono->chronoStop( tag);
      }

      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
      
      /** @brief The standard @c StoreGateSvc (event store)
       * Returns (kind of) a pointer to the @c StoreGateSvc
       */
      StoreGateSvc_t& evtStore() const {return m_evtStore;};

      /** @brief The standard @c StoreGateSvc/DetectorStore
       * Returns (kind of) a pointer to the @c StoreGateSvc
       */
      StoreGateSvc_t& detStore() const {return m_detStore;};

      /** templated Tool retrieval - gives unique handling & look and feel */
      template <class T> StatusCode retrieveTool(ToolHandle<T>& thandle){
         if (!thandle.empty() && thandle.retrieve().isFailure()){
               ATH_MSG_FATAL( m_screenOutputPrefix << "Cannot retrieve " << thandle << ". Abort.");
               return StatusCode::FAILURE;
           } else
               ATH_MSG_DEBUG(m_screenOutputPrefix << "Successfully retrieved " << thandle);
               return StatusCode::SUCCESS;
      }
      
      /** templated Tool retrieval - gives unique handling & look and feel */
      template <class T> 
      StatusCode retrieveTools(ToolHandleArray<T>& thandleArray){
         if (!thandleArray.empty() && thandleArray.retrieve().isFailure()){
               ATH_MSG_FATAL( m_screenOutputPrefix << "Cannot retrieve " << thandleArray << ". Abort.");
               return StatusCode::FAILURE;
           } else
               ATH_MSG_DEBUG(m_screenOutputPrefix << "Successfully retrieved " << thandleArray);
        return StatusCode::SUCCESS;
      }
      
      /** templated record collection method, will create a new one if not existing */
      template<class T>
      StatusCode recordCollection( T*& coll, const std::string& collName) const{
         // create if necessary
         coll = coll ? coll : new T;
         // record
         if (evtStore()->record( coll, collName).isFailure()){
             ATH_MSG_FATAL( m_screenOutputPrefix << "Cannot record collection " <<  collName << ". Abort." );
             return StatusCode::FAILURE;
         } else 
             ATH_MSG_DEBUG(m_screenOutputPrefix << "Successfully recorded collection " << collName);
        return StatusCode::SUCCESS;
      }

      /** templated retrieve collection method, boolean steers that force break */
      template<class T>
      StatusCode retrieveCollection(T*& coll, const std::string& collName, bool forceBreak=true) const {
          // check for existence in the soft case
          if (!forceBreak && !evtStore()->contains<T>(collName)) { 
              coll = 0; 
              ATH_MSG_DEBUG(m_screenOutputPrefix << "Collection does not exists (not required). Ignore.");
              return StatusCode::SUCCESS;
          } 
          if ( evtStore()->retrieve(coll, collName).isFailure()){
              ATH_MSG_FATAL( m_screenOutputPrefix << "Cannot retireve collection " <<  collName << ". Abort." );
              return StatusCode::FAILURE;
         } else  
              ATH_MSG_DEBUG(m_screenOutputPrefix << "Successfully retrieved collection " << collName);
          return StatusCode::SUCCESS;     
       }

    private:     
      /** Default constructor */
      BaseSimulationSvc();
      
      /// Pointer to StoreGate (event store by default)
      mutable StoreGateSvc_t m_evtStore;

      /// Pointer to StoreGate (detector store by default)
      mutable StoreGateSvc_t m_detStore;
      

    protected:  
      /** The simulator service descriptor */
      std::string       m_simDescr;
      
      /** Screen output refinement - can be changed by declareProperty() */
      std::string       m_screenOutputPrefix;

      /** Name of the timing service - can be set by declareProperty()*/
      std::string       m_chronoSvcName;

      /** The timing service for general usage */
      IChronoStatSvc*   m_chrono; 

      /** The particle service used to push particles into the simulation */
      IParticleBroker*  m_particleBroker;
      
  }; 
  
  
   /** Simulation Call --- hand over to the particleProcessor if it exists */
   inline StatusCode BaseSimulationSvc::simulate(const ISFParticle& /*isp*/)
   { 
       return StatusCode::SUCCESS;
   }
}


#endif //> !ISF_BASESIMULATIONSVC_H
