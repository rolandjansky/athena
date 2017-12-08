/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_Interfaces/ISimulationSelector.h"
#include "ISF_Interfaces/SimulationFlavor.h"
#include "ISF_Event/ISFParticle.h"

/** Constructor with parameters */
ISF::ISimulationSelector::ISimulationSelector( const std::string& t, const std::string& n, const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_simulator("DefaultSimulator", n),
  m_isDynamic(false),
  m_invertCuts(false),
  m_simflavor(ISF::UndefinedSim)
{
  declareProperty("Simulator",      m_simulator  );
  declareProperty("IsDynamic",      m_isDynamic  );
  declareProperty("InvertCuts",     m_invertCuts );
}

/** Gaudi sysInitialize() method */
StatusCode ISF::ISimulationSelector::sysInitialize()
{ 
  if ( AthAlgTool::sysInitialize().isFailure() ) {
      ATH_MSG_FATAL( " Cannot initialize AthService! Abort.");
      return StatusCode::FAILURE;
  }

  if ( m_simulator.retrieve().isFailure() ) {
    ATH_MSG_FATAL( m_simulator.propertyName() << ": Failed to retrieve tool " << m_simulator.type());
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( m_simulator.propertyName()  << ": Retrieved tool " << m_simulator.type());
  }

  //// determine simulation flavor, to be stored in particle's barcode
  //std::string simulatorType = m_simulator.type() ;
  //std::transform(simulatorType.begin(), simulatorType.end(), simulatorType.begin(), ::tolower);

  //if      (simulatorType.find("fatras")!=std::string::npos)   { m_simflavor = ISF::FatrasSim; }
  //else if (simulatorType.find("g4")!=std::string::npos)       { m_simflavor = ISF::Geant4Sim; }
  //else if (simulatorType.find("geant")!=std::string::npos)    { m_simflavor = ISF::Geant4Sim; }
  //else if (simulatorType.find("full")!=std::string::npos)     { m_simflavor = ISF::Geant4Sim; }
  //else if (simulatorType.find("fastcalo")!=std::string::npos) { m_simflavor = ISF::FastCaloSim; }
  //else    { m_simflavor = ISF::UndefinedSim; }    

  return StatusCode::SUCCESS;
}

/** virtual destructor */
ISF::ISimulationSelector::~ISimulationSelector() { }

/** make the routing decision */
bool 
ISF::ISimulationSelector::selfSelect(const ISFParticle& particle) 
{
  bool pass = passSelectorCuts(particle);
  pass = ( m_invertCuts ? (!pass) : pass );

  //// store the simulator flavor in the particle's extra bc 
  //if ( pass ) {
  //  if ( m_barcodeSvc->hasBitCalculator() ) {
  //    Barcode::ParticleBarcode extrabc = particle.getExtraBC();
  //    m_barcodeSvc->getBitCalculator()->SetSimulator(extrabc,m_simflavor);
  //    particle.setExtraBC( extrabc );
  //  }
  //}

  return pass;
}


