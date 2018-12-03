/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleBroker.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef ISF_INTERFACES_IPARTICLEBROKER_H
#define ISF_INTERFACES_IPARTICLEBROKER_H 1

// Framework includes
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"

// ISF includes
#include "ISF_Event/SimSvcID.h"
#include "ISF_Event/ISFParticleContainer.h"
#include "ISF_Event/EntryLayer.h"
#include "ISF_Interfaces/ISimulationSelector.h"

namespace ISF {

  class ISFParticle;
  class IStackFiller;

  /** @ class IParticleBroker
  
      Interface for the particle broker service
  
      @ author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
     */
  class IParticleBroker : virtual public IInterface {

      /////////////////////////////////////////////////////////////////// 
      // Public methods: 
      /////////////////////////////////////////////////////////////////// 
    public: 

      /// Creates the InterfaceID and interfaceID() method
      DeclareInterfaceID(IParticleBroker, 1, 0);
      
      /** Register an array of SimulationSelectors */
      virtual StatusCode registerSimSelector(SimSelectorToolArray &simSelectorTools, AtlasDetDescr::AtlasRegion geoID) = 0;

      /** Initialize the broker */
      virtual StatusCode initializeEvent(ISFParticleContainer&& simParticles) = 0;

      /** Finalize the event in the broker service */
      virtual StatusCode finalizeEvent() = 0;

      /** hand a new ISF particle to the broker */
      virtual void push( ISFParticle *particle, const ISFParticle *parent) = 0;
           
      /** Get vectors of ISF particles from the broker */
      virtual const ConstISFParticleVector& popVector(size_t maxVectorSize=10240) = 0;

      /** Get the current number of particles stored */
      virtual size_t numParticles() const = 0;

      /* Dump the stack to the screen */
      virtual StatusCode dump() const = 0;
  }; 
}

#endif //> !ISF_INTERFACES_IPARTICLEBROKER_H

