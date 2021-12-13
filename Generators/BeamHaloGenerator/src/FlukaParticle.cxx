/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/FlukaParticle.h"
#include <iostream>
#include <sstream>

//---------------------------------------------------------------------------

FlukaParticle::FlukaParticle(int type): m_type(type),
                                        m_eventId(0),
				        m_flukaId(0),
				        m_partGenNum(0),
				        m_kineticEnergy(0.),
				        m_positionAtScoringPlane(),
				        m_directionalCosines(),
                                        m_weight(1.),
                                        m_totalEnergy(0.),
                                        m_positionAtPrimary() {
}

//---------------------------------------------------------------------------

void FlukaParticle::clear() {
  m_eventId = 0; 
  m_flukaId = 0; 
  m_partGenNum = 0;
  m_kineticEnergy = 0.;
  m_positionAtScoringPlane.setX(0.);
  m_positionAtScoringPlane.setY(0.);
  m_positionAtScoringPlane.setZ(0.);
  m_positionAtScoringPlane.setT(0.);
  m_directionalCosines.setX(0.);
  m_directionalCosines.setY(0.);
  m_directionalCosines.setZ(0.);
  m_weight = 1.;
  m_totalEnergy = 0.;
  m_positionAtPrimary.setX(0.);
  m_positionAtPrimary.setY(0.);
  m_positionAtPrimary.setZ(0.);
  m_positionAtPrimary.setT(0.);
}

//---------------------------------------------------------------------------

int FlukaParticle::read(std::vector<std::string> *eventAsStringVector) {
  clear();

  if(!eventAsStringVector) {
    std::cerr << "ERROR: string vector passed to FlukaParticle::read is null." << std::endl;
    return 1;
  }

  if(m_type == BRUCE) {
    if(eventAsStringVector->size() != 15) {
      std::cerr << "ERROR: there are " << eventAsStringVector->size() << " data words, which is out of range.  This particle will be null." << std::endl;
      return 2;
    }
  }
  else if(m_type == TALANOV) {
    if(eventAsStringVector->size() < 10 || eventAsStringVector->size() > 11 ) {
      std::cerr << "ERROR: there are " << eventAsStringVector->size() << " data words, which is out of range.  This particle will be null." << std::endl;
      return 3;
    }
  }
  else {
    std::cerr << "ERROR: unknown input Fluka record type.  This particle will be null." << std::endl;
    return 4;
  }

  double doubleValue;
  std::vector<std::string>::iterator itr = eventAsStringVector->begin();
  std::vector<std::string>::iterator itr_end = eventAsStringVector->end();
  int wordNumber = 0;
  if(m_type == BRUCE) {
    for(;itr!=itr_end;++itr,++wordNumber) {
      if((*itr).length() == 0) {
        std::cerr << "ERROR: string word has zero length." << std::endl;
        return 5;
      }
      std::istringstream inStr((*itr));
      switch (wordNumber) {
      case 0 : inStr >> m_eventId; break; // FLUKA run number (between 1 and 3000)
      case 1 : inStr >> m_partGenNum; break; // ID of primary particle (between 1 and 100 in each run)
      case 2 : inStr >> m_flukaId; break; // Standard FLUKA particle id
      case 3 : inStr >> m_kineticEnergy; break; // GeV
      case 4 : inStr >> m_weight; break;
      case 5 : inStr >> doubleValue; m_positionAtScoringPlane.setX(doubleValue); break; // (cm)
      case 6 : inStr >> doubleValue; m_positionAtScoringPlane.setY(doubleValue); break; // (cm)
      case 7 : inStr >> doubleValue; m_directionalCosines.setX(doubleValue); break; // w.r.t x-axis
      case 8 : inStr >> doubleValue; m_directionalCosines.setY(doubleValue); break; // w.r.t y-axis
      //case 9 : inStr >> doubleValue; m_positionAtScoringPlane.setT(doubleValue); break; // time (s) since start of primary particle (use t=0 for ATLAS Geant)
      case 10 : inStr >> m_totalEnergy; break; // total energy (GeV)
      case 11 : inStr >> doubleValue; m_positionAtPrimary.setX(doubleValue); break; // (cm)
      case 12 : inStr >> doubleValue; m_positionAtPrimary.setY(doubleValue); break; // (cm)
      case 13 : inStr >> doubleValue; m_positionAtPrimary.setZ(doubleValue); break; // (cm)
      case 14 : inStr >> doubleValue; m_positionAtPrimary.setT(doubleValue); break; // (cm)
      default : break;
      }
    }
  }
  else if(m_type == TALANOV) {
    for(;itr!=itr_end;++itr,++wordNumber) {
      std::istringstream inStr((*itr)); 
      switch (wordNumber) {
      case 0 : inStr >> m_eventId; break;
      case 1 : inStr >> m_flukaId; break;
      case 2 : inStr >> m_partGenNum; break;
      case 3 : inStr >> m_kineticEnergy; break;
      case 4 : inStr >> doubleValue; m_positionAtScoringPlane.setX(doubleValue); break;
      case 5 : inStr >> doubleValue; m_positionAtScoringPlane.setY(doubleValue); break;
      case 6 : inStr >> doubleValue; m_positionAtScoringPlane.setZ(doubleValue); break;
      case 7 : inStr >> doubleValue; m_directionalCosines.setX(doubleValue); break;
      case 8 : inStr >> doubleValue; m_directionalCosines.setY(doubleValue); break;
      case 9 : inStr >> doubleValue; m_directionalCosines.setZ(doubleValue); break;
      case 10 : inStr >> doubleValue; m_positionAtScoringPlane.setT(doubleValue); break;
      default : break;
      }
    }
  }
  
  return 0;
}

//---------------------------------------------------------------------------

int FlukaParticle::pdgId() {
  int pdgID = 0;
  
  // There does not appear to be a clever way to do this.  The case
  // statement mapping is taken from the FLUKA manual:
  // http://www.fluka.org/fluka.php?id=man_onl&sub=7
  
  switch (m_flukaId) {
  case -6 : pdgID = 1000020040; break;
  case -5 : pdgID = 1000020030; break;
  case -4 : pdgID = 1000010030; break;
  case -3 : pdgID = 1000010020; break;
  case 1 : pdgID = 2212; break;
  case 2 : pdgID = -2212; break;
  case 3 : pdgID = 11; break;
  case 4 : pdgID = -11; break;
  case 5 : pdgID = 12; break;
  case 6 : pdgID = -12; break;
  case 7 : pdgID = 22; break;
  case 8 : pdgID = 2112; break;
  case 9 : pdgID = -2112; break;
  case 10 : pdgID = -13; break;
  case 11 : pdgID = 13; break;
  case 12 : pdgID = 130; break;
  case 13 : pdgID = 211; break;
  case 14 : pdgID = -211; break;
  case 15 : pdgID = 321; break;
  case 16 : pdgID = -321; break;
  case 17 : pdgID = 3122; break;
  case 18 : pdgID = -3122; break;
  case 19 : pdgID = 310; break;
  case 20 : pdgID = 3112; break;
  case 21 : pdgID = 3222; break;
  case 22 : pdgID = 3212; break;
  case 23 : pdgID = 111; break;
  case 24 : pdgID = 311; break;
  case 25 : pdgID = -311; break;
  case 27 : pdgID = 14; break;
  case 28 : pdgID = -14; break;
  case 31 : pdgID = -3222; break;
  case 32 : pdgID = -3212; break;
  case 33 : pdgID = -3112; break;
  case 34 : pdgID = 3322; break;
  case 35 : pdgID = -3322; break;
  case 36 : pdgID = 3312; break;
  case 37 : pdgID = -3312; break;
  case 38 : pdgID = 3334; break;
  case 39 : pdgID = -3334; break;
  case 41 : pdgID = -15; break;
  case 42 : pdgID = 15; break;
  case 43 : pdgID = 16; break;
  case 44 : pdgID = -16; break;
  case 45 : pdgID = 411; break;
  case 46 : pdgID = -411; break;
  case 47 : pdgID = 421; break;
  case 48 : pdgID = -421; break;
  case 49 : pdgID = 431; break;
  case 50 : pdgID = -431; break;
  case 51 : pdgID = 4122; break;
  case 52 : pdgID = 4232; break;
  case 53 : pdgID = 4112; break;
  case 54 : pdgID = 4322; break;
  case 55 : pdgID = 4312; break;
  case 56 : pdgID = 4332; break;
  case 57 : pdgID = -4122; break;
  case 58 : pdgID = -4232; break;
  case 59 : pdgID = -4132; break;
  case 60 : pdgID = -4322; break;
  case 61 : pdgID = -4312; break;
  case 62 : pdgID = -4332; break;
  default : pdgID = 0;
  }
  
  return pdgID;
}

//---------------------------------------------------------------------------

void FlukaParticle::print() {
  if(m_type == BRUCE) {
    std::cout.fill(' ');
    std::cout.width(6); std::cout << m_eventId << " ";
    std::cout.width(6); std::cout << m_partGenNum << " ";
    std::cout.width(6); std::cout << m_flukaId << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_kineticEnergy << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_weight << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.x() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.y() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_directionalCosines.x() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_directionalCosines.y() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.t() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_totalEnergy << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.x() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.y() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.z() << " ";
    std::cout.width(13); std::cout.precision(6); std::cout << std::scientific << m_positionAtPrimary.t() << " ";
    std::cout.precision(6);
    std::cout << std::endl;
  }
  else if(m_type == TALANOV) {
    std::cout.fill(' ');
    std::cout.width(7); std::cout << m_eventId << " ";
    std::cout.width(2); std::cout << m_flukaId << " ";
    std::cout.width(4); std::cout << m_partGenNum << " ";
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_kineticEnergy << " "; 
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_positionAtScoringPlane.x() << " ";
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_positionAtScoringPlane.y() << " ";
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_positionAtScoringPlane.z() << " ";
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_directionalCosines.x() << " ";
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_directionalCosines.y() << " ";
    std::cout.width(19); std::cout.precision(12); std::cout << std::scientific << m_directionalCosines.z() << " ";
    std::cout.width(19); std::cout.precision(6); std::cout << std::scientific << m_positionAtScoringPlane.t(); 
    std::cout.precision(6);
    std::cout << std::endl;
  }
}

//---------------------------------------------------------------------------
