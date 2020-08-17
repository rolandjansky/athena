/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "BaseSimulationSelector.h"

#include "ISF_Interfaces/SimulationFlavor.h"
#include "ISF_Event/ISFParticle.h"


/** Constructor with parameters */
ISF::BaseSimulationSelector::BaseSimulationSelector(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_simulator("DefaultSimulator", name),
  m_isDynamic(false),
  m_invertCuts(false)
{
  declareProperty("Simulator",      m_simulator  );
  declareProperty("IsDynamic",      m_isDynamic  );
  declareProperty("InvertCuts",     m_invertCuts );
  declareProperty("SimulationFlavor", m_simFlavorProp);
  m_simFlavorProp.verifier().setLower(ISF::UndefinedSim+1);
  m_simFlavorProp.verifier().setUpper(ISF::NFlavors-1);
  m_simFlavorProp.declareUpdateHandler(&ISF::BaseSimulationSelector::SimulationFlavorHandler, this);
}

void ISF::BaseSimulationSelector::SimulationFlavorHandler(Gaudi::Details::PropertyBase&)
{
  // FIXME would probably be better to have this in SimulationFlavor.h
  switch(m_simFlavorProp.value())
    {
    case 1: m_simflavor = ISF::ParticleKiller; break;
    case 2: m_simflavor = ISF::Fatras; break;
    case 3: m_simflavor = ISF::Geant4; break;
    case 4: m_simflavor = ISF::FastCaloSim; break;
    case 5: m_simflavor = ISF::FastCaloSimV2; break;
    case 6: m_simflavor = ISF::Parametric; break;
    case 7: m_simflavor = ISF::FatrasPileup; break;
    case 8: m_simflavor = ISF::FastCaloSimPileup; break;
    }
}

/** Gaudi sysInitialize() method */
StatusCode ISF::BaseSimulationSelector::sysInitialize()
{
  ATH_CHECK( AthAlgTool::sysInitialize() );
  ATH_CHECK( m_simulator.retrieve() );

  return StatusCode::SUCCESS;
}


/** virtual destructor */
ISF::BaseSimulationSelector::~BaseSimulationSelector() { }

/** make the routing decision */
bool
ISF::BaseSimulationSelector::selfSelect(const ISFParticle& particle) const
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
ISF::SimulationFlavor ISF::BaseSimulationSelector::simFlavor() const {
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
