/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_GEANT4SIMSVC_H
#define ISF_GEANT4SIMSVC_H 1

// STL includes
#include <string>

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// ISF includes
#include "ISF_Interfaces/BaseSimulationSvc.h"
#include "ISF_Interfaces/ISimulatorTool.h"
#include "ISF_Event/ISFParticleContainer.h"

namespace iGeant4 {

  /** @class Geant4SimSvc

  */
  class Geant4SimSvc : public ISF::BaseSimulationSvc {

  public:
    //** Constructor with parameters */
    Geant4SimSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~Geant4SimSvc();

    /** Athena algorithm's interface methods */
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    /** Simulation Call  */
    virtual StatusCode simulate(const ISF::ISFParticle& isp, McEventCollection* mcEventCollection) override;

    /** Simulation Call for vector of ISF particles */
    virtual StatusCode simulateVector(const ISF::ConstISFParticleVector& particles, McEventCollection* mcEventCollection) override;

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent() override;

    /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent() override;

  private:
    /** Default constructor */
    Geant4SimSvc();

    PublicToolHandle<ISF::ISimulatorTool> m_simulatorTool{this, "SimulatorTool", "", ""};

  };
}


#endif //> !ISF_Geant4SimSvc_H
