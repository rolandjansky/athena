/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BaseSimulationSelector.h"

#include "ISF_Interfaces/SimulationFlavor.h"
#include "ISF_Event/ISFParticle.h"


/** Constructor with parameters */
ISF::BaseSimulationSelector::BaseSimulationSelector(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_simulator("DefaultSimulator", name),
  m_isDynamic(false),
  m_invertCuts(false),
  m_simflavor(ISF::UndefinedSim)
{
  declareProperty("Simulator",      m_simulator  );
  declareProperty("IsDynamic",      m_isDynamic  );
  declareProperty("InvertCuts",     m_invertCuts );
}


/** Gaudi sysInitialize() method */
StatusCode ISF::BaseSimulationSelector::sysInitialize()
{
  if ( AthAlgTool::sysInitialize().isFailure() ) {
      ATH_MSG_FATAL( " Cannot initialize AthService! Abort.");
      return StatusCode::FAILURE;
  }

  if ( m_simulator.retrieve().isFailure() ) {
    ATH_MSG_FATAL( m_simulator.propertyName() << ": Failed to retrieve service " << m_simulator.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( m_simulator.propertyName()  << ": Retrieved service " << m_simulator.type());
  }

  return StatusCode::SUCCESS;
}


/** virtual destructor */
ISF::BaseSimulationSelector::~BaseSimulationSelector() { }

/** make the routing decision */
bool
ISF::BaseSimulationSelector::selfSelect(const ISFParticle& particle)
{
  bool pass = passSelectorCuts(particle);
  pass = ( m_invertCuts ? (!pass) : pass );

  return pass;
}

/** return a handle on the simulator */
ServiceHandle<ISF::ISimulationSvc>* ISF::BaseSimulationSelector::simulator() {
  return &m_simulator;
}

/** return if is this a static or dynamic SimulationSelector
    (is used by fully dynamic partilce routers) */
bool ISF::BaseSimulationSelector::isDynamic() {
  return m_isDynamic;
}

/** return the simulation service ID */
ISF::SimSvcID ISF::BaseSimulationSelector::simSvcID() {
  return m_simulator->simSvcID();
}

/** return the simulation service ID */
ISF::SimulationFlavor ISF::BaseSimulationSelector::simFlavor() {
  return m_simflavor;
}

/** initialize Selector */
void ISF::BaseSimulationSelector::initializeSelector() { }

/** called at the beginning of each athena event
    (can be used for eg. resetting dynamic selectors) */
void ISF::BaseSimulationSelector::beginEvent() { }

/** called at the end of each athena event
    (can be used for eg. resetting dynamic selectors) */
void ISF::BaseSimulationSelector::endEvent() { }

/** update internal event representation */
void ISF::BaseSimulationSelector::update(const ISFParticle& ) { }
