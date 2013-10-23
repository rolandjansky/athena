/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/MarsParticle.h"
#include <iostream>
#include <sstream>
  
MarsShowerPrimary::MarsShowerPrimary(): particleId(0),
					processId(0),
					weight(0),
					position(),
					kineticEnergy(0.) {
}

MarsBeamGasInteraction::MarsBeamGasInteraction(): nucleusType(0),
						  nevve(0),
						  timeOfFlight(0.) { 
}

//-------------------------------------------------------------------------------------------------

MarsParticle::MarsParticle(): m_eventNumber(0),
			      m_particleId(0),
			      m_kineticEnergy(0.),
			      m_weight(0.),
			      m_positionAtScoringPlane(),
			      m_directionalCosines(),
			      m_timeOfFlight(0.),
			      m_primaryProtonZ(0.),
			      m_showerPrimary(),
			      m_beamGasInteraction() {
}

//-------------------------------------------------------------------------------------------------

int MarsParticle::read(std::vector<std::string> *eventAsStringVector) {
  if(eventAsStringVector->size() != 17 &&
     eventAsStringVector->size() != 20) {
    std::cerr << "There are only " << eventAsStringVector->size() << " data words.  This event will be null." << std::endl;
    return 1;
  }
  
  double doubleValue;
  std::vector<std::string>::iterator itr = eventAsStringVector->begin();
  std::vector<std::string>::iterator itr_end = eventAsStringVector->end();
  int wordNumber = 0;
  for(;itr!=itr_end;++itr,++wordNumber) {
    std::istringstream inStr((*itr));
    switch (wordNumber) {
    case 0 : inStr >> m_eventNumber; break;
    case 1 : inStr >> m_particleId; break;
    case 2 : inStr >> m_kineticEnergy; break;
    case 3 : inStr >> m_weight; break;
    case 4 : inStr >> doubleValue; m_positionAtScoringPlane.setX(doubleValue); break;
    case 5 : inStr >> doubleValue; m_positionAtScoringPlane.setY(doubleValue); break;
    case 6 : inStr >> doubleValue; m_directionalCosines.setX(doubleValue); break;
    case 7 : inStr >> doubleValue; m_directionalCosines.setY(doubleValue); break;
    case 8 : inStr >> m_timeOfFlight; break;
    case 9 : inStr >> m_primaryProtonZ; break;
    case 10 : inStr >> doubleValue; m_showerPrimary.position.setZ(doubleValue); break;
    case 11 : inStr >> doubleValue; m_showerPrimary.position.setX(doubleValue); break;
    case 12 : inStr >> doubleValue; m_showerPrimary.position.setY(doubleValue); break;
    case 13 : inStr >> m_showerPrimary.kineticEnergy; break;
    case 14 : inStr >> m_showerPrimary.weight; break;
    case 15 : inStr >> m_showerPrimary.particleId; break;
    case 16 : inStr >> m_showerPrimary.processId; break;
    case 17 : inStr >> m_beamGasInteraction.nevve; break;
    case 18 : inStr >> m_beamGasInteraction.nucleusType; break;
    case 19 : inStr >> m_beamGasInteraction.timeOfFlight; break;
    default : break;
    }
  }

  return 0;
}

//-------------------------------------------------------------------------------------------------

void MarsParticle::print(bool beamGas) {
  std::cout.fill(' ');
  std::cout.width(11); std::cout << m_eventNumber << " ";
  std::cout.width(6); std::cout << m_particleId << " ";
  std::cout.width(12); std::cout.precision(5); std::cout << std::scientific << m_kineticEnergy << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_weight << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_positionAtScoringPlane.x() << " ";
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_positionAtScoringPlane.y() << " ";
  std::cout.width(14); std::cout.precision(7); std::cout << std::scientific << m_directionalCosines.x() << " ";
  std::cout.width(14); std::cout.precision(7); std::cout << std::scientific << m_directionalCosines.y() << " ";
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_timeOfFlight << " "; 
  std::cout.width(12); std::cout.precision(5); std::cout << std::scientific << m_primaryProtonZ << " ";
  std::cout.width(12); std::cout.precision(5); std::cout << std::scientific << m_showerPrimary.position.z() << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_showerPrimary.position.x() << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_showerPrimary.position.y() << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_showerPrimary.kineticEnergy << " "; 
  std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_showerPrimary.weight << " ";
  std::cout.fill(' ');
  std::cout.width(3); std::cout << m_showerPrimary.particleId << " ";
  std::cout.width(3); std::cout << m_showerPrimary.processId;
  
  if(beamGas) {
    std::cout << " ";
    std::cout.width(3); std::cout << m_beamGasInteraction.nevve << " ";
    std::cout.width(3); std::cout << m_beamGasInteraction.nucleusType << " ";
    std::cout.width(11); std::cout.precision(4); std::cout << std::scientific << m_beamGasInteraction.timeOfFlight;
  }

  std::cout << std::endl;

  std::cout.precision(6);
}

//-------------------------------------------------------------------------------------------------

int MarsParticle::pdgId() {
  int pdgId = 0;

  switch (m_particleId) {
  case 1 : pdgId = 2212; break;
  case 2 : pdgId = 2112; break;
  case 3 : pdgId = 211; break;
  case 4 : pdgId = -211; break;
  case 5 : pdgId = 321; break;
  case 6 : pdgId = -321; break;
  case 7 : pdgId = -13; break;
  case 8 : pdgId = 13; break;
  case 9 : pdgId = 22; break;
  case 10 : pdgId = 11; break;
  case 11 : pdgId = -11; break;
  case 12 : pdgId = -2212; break;
  case 13 : pdgId = 111; break;
  case 14 : pdgId = 1000010020; break; // Deuterium
  case 15 : pdgId = 1000010030; break; // Tritium
  case 16 : pdgId = 1000020030; break; // He3
  case 17 : pdgId = 1000020040; break; // Alpha-(He4)
  case 18 : pdgId = 14; break;
  case 19 : pdgId = -14; break;
  case 20 : pdgId = 12; break;
  case 21 : pdgId = -12; break;
  case 22 : pdgId = 130; break;
  case 23 : pdgId = 310; break;
  case 24 : pdgId = 311; break;
  case 25 : pdgId = -311; break;
  case 26 : pdgId = 3122; break;
  case 27 : pdgId = -3122; break;
  case 28 : pdgId = 3222; break;
  case 29 : pdgId = 3212; break;
  case 30 : pdgId = 3112; break;
  case 31 : pdgId = -2112; break;
  case 32 : pdgId = 3322; break;
  case 33 : pdgId = 3312; break;
  case 34 : pdgId = 3334; break;
  case 35 : pdgId = -3112; break;
  case 36 : pdgId = -3212; break;
  case 37 : pdgId = -3222; break;
  case 38 : pdgId = -3322; break;
  case 39 : pdgId = -3312; break;
  case 40 : pdgId = -3334; break;
  default : pdgId = 0; break;
  }

  return pdgId;
}

