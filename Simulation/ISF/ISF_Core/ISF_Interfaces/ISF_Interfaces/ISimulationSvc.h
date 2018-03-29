/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISimulationSvc.h, (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////// 
#ifndef ISF_INTERFACES_ISIMULATIONSVC_H
#define ISF_INTERFACES_ISIMULATIONSVC_H 1

// stl includes
#include <string>

// Framework includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IOVSvcDefs.h"

// ISF includes
#include "ISF_Event/ISFParticleVector.h"
#include "ISF_Event/SimSvcID.h"

namespace ISF {

  class ISFParticle;
  class IParticleBroker;

  /** @ class ISimulationSvc
  
      Main interface of either geometrical of flavor simulation service.
      The SimulationSvc does not take ownership of the StackParticle. 
      The IStackSvc and ITruthSvc handles are provided by the kernel to force one unique configuration.
      
      @ author Andreas.Salzburger -at- cern.ch, Michael.Duehrssen -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class ISimulationSvc : virtual public IInterface { 

      /////////////////////////////////////////////////////////////////// 
      // Public methods: 
      /////////////////////////////////////////////////////////////////// 
    public: 
      ISimulationSvc() : m_ssvcID( ISF::fUndefinedSimID) { };

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(ISimulationSvc, 1, 0);
      
      /** Inform the SimulationSvc about the ParticleBroker */
      virtual StatusCode setParticleBroker( IParticleBroker *broker) = 0;

      /** Simulation call for vectors of particles*/
      virtual StatusCode simulateVector(const ConstISFParticleVector &particles) = 0;

      /** Simulation call for individual particles*/
      virtual StatusCode simulate(const ISFParticle& isp) = 0;

      /** Return the simulation service descriptor */
      virtual std::string& simSvcDescriptor() = 0;

      /** Setup Event chain - in case of a begin-of event action is needed, 
          to be called by simulation kernel */
      virtual StatusCode setupEvent() = 0;                             
                                 
      /** Release Event chain - in case of an end-of event action is needed,
          to be called by simulation kernel  */
      virtual StatusCode releaseEvent() = 0;                             
    
      /** callback after geometry initialization,
          needs to be registered in ::initialzie() */
      virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS) = 0;
                                 
      /** Assign a simulation service ID */
      inline void assignSimSvcID(SimSvcID id);

      /** Return the simulation service ID */
      inline SimSvcID simSvcID();

    private:
      SimSvcID        m_ssvcID;
  }; 

  /** inline methods */
  inline void ISimulationSvc::assignSimSvcID(SimSvcID id) { m_ssvcID = id; }
  inline SimSvcID ISimulationSvc::simSvcID() { return m_ssvcID; }

}

#endif //> !ISF_INTERFACES_ISIMULATIONSVC_H

