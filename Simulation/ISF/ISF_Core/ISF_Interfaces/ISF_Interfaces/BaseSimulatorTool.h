/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_INTERFACES_BASESIMULATORTOOL_H
#define ISF_INTERFACES_BASESIMULATORTOOL_H 1

// STL includes
#include <string>

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IChronoStatSvc.h"

// Athena
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgTool.h"

// DetectorDescription
#include "AtlasDetDescr/AtlasRegion.h"
#include "AtlasDetDescr/AtlasRegionHelper.h"

// ISF includes
#include "ISF_Interfaces/ISimulatorTool.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_Event/SimSvcID.h"

namespace ISF {

  /** @class BaseSimulatorTool

      Concrete base class for all simulator Tools

  */
  class BaseSimulatorTool : public extends<AthAlgTool, ISimulatorTool> {
  public:

    //** Constructor with parameters */
    BaseSimulatorTool(const std::string& type, const std::string& name,  const IInterface* parent)
      : base_class(type, name, parent)
    {
      // Service handling
      declareProperty("ChronoStatService",    m_chronoSvcName     );
    }

    /** Destructor */
    virtual ~BaseSimulatorTool() {}

    /** Gaudi sysInitialize() methods */
    virtual StatusCode sysInitialize() override
    {
      ATH_CHECK( AthAlgTool::sysInitialize() );
      ATH_CHECK( serviceLocator()->service(m_chronoSvcName, m_chrono ) );
      return StatusCode::SUCCESS;
    }

    /** */
    virtual StatusCode initialize() override
    {
      return StatusCode::SUCCESS;
    }

    /** Setup Event chain - in case of a begin-of event action is needed */
    virtual StatusCode setupEvent(const EventContext&) override
    { return StatusCode::FAILURE; }

    /** Setup Event chain - in case of a begin-of event action is needed (called by ISimulationSvc) */
    virtual StatusCode setupEventST() override
    { return setupEvent(Gaudi::Hive::currentContext()); }

    /** */
    virtual StatusCode simulate( const ISFParticle& , ISFParticleContainer&, McEventCollection*) const override
    { return StatusCode::FAILURE; }

    /** Simulation call for vectors of particles */
    virtual StatusCode simulateVector(const ConstISFParticleVector& particles, ISFParticleContainer& secondaries, McEventCollection* mcEventCollection) const override
    {
      // this implementation is a wrapper in case the simulator does
      // implement particle-vector input

      // simulate each particle individually
      for (auto* cisp : particles) {
        ATH_MSG_VERBOSE( "Starting simulation of particle: " << (*cisp) );
        ATH_CHECK(this->simulate(*cisp, secondaries, mcEventCollection));
      }
      return StatusCode::SUCCESS;

    }

    /** Release Event chain - in case of an end-of event action is needed */
    virtual StatusCode releaseEvent(const EventContext&) override
    { return StatusCode::FAILURE; }

    /** Release Event chain - in case of an end-of event action is needed (called by ISimulationSvc) */
    virtual StatusCode releaseEventST() override
    { return releaseEvent(Gaudi::Hive::currentContext()); }

    /** wrapper call to start chrono with given tag */
    const ChronoEntity* chronoStart(const IChronoSvc::ChronoTag& tag ) {
      if (m_chrono) return m_chrono->chronoStart( tag);
      return nullptr;
    }

    /** wrapper call to stop chrono with given tag */
    const ChronoEntity* chronoStop(const IChronoSvc::ChronoTag& tag ) {
      if (m_chrono) return m_chrono->chronoStop( tag);
      return nullptr;
    }

  private:
    /** Default constructor */
    BaseSimulatorTool();

  protected:
    /** Name of the timing service - can be set by declareProperty()*/
    std::string       m_chronoSvcName{"ChronoStatSvc"};

    /** The timing service for general usage */
    IChronoStatSvc*   m_chrono{};

  };

}


#endif //> !ISF_INTERFACES_BASESIMULATORTOOL_H
