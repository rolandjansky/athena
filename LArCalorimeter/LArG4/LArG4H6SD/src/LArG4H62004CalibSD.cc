/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004CalibSD.h"
#include "LArG4Code/ILArCalibCalculatorSvc.h"

#undef DEBUG_SD

LArG4H62004CalibSD::LArG4H62004CalibSD(G4String a_name, ILArCalibCalculatorSvc* calc,bool doPID):
  LArG4CalibSD(a_name,calc,doPID)
{
#ifdef DEBUG_SD
  std::cout << " in LArG4H62004CalibSD:::LArG4H62004CalibSD a_name " << a_name << std::endl;
#endif
}

G4bool LArG4H62004CalibSD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
#ifdef DEBUG_SD
  std::cout << "my name: "<<this->GetName()<<", calc: "<<LArG4::m_calculator<<std::endl;
#endif
  LArG4Identifier ident;
  std::vector<G4double> energies;
  // Convert the G4Step into identifier and energy.
  G4bool valid = m_calculator->Process(a_step, ident, energies, LArG4::kEnergyAndID);

  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)

  if ( ! valid )
    {
      ++m_numberInvalidHits;
      std::cout<< " Invalid step, not counted: "<<GetName()<<", calc: "<<m_calculator<<" !!!"<<std::endl;
      return false;
    }

  // Changing the ident to comply with H6 dictionary
  if(ident[1] == 1) { // EMEC module
    ident[6] += 8; // change phi id
    if(ident[6] < 20) ident[6] = 20;
    if(ident[6] > 27) ident[6] = 27;
  }else if(ident[1] == 2) { // HEC module
    if(ident[4] == 0) {
      if(ident[3]==0 && ident[5] < 6) ident[5]=6;
      if(ident[3]==2 && ident[5] < 8) ident[5]=8;
    } else {
      if(ident[3]==1 && ident[5] > 2) ident[5]=2;
    }
  }

  bool processed = SimpleHit( ident, energies, m_calibrationHits );
  if(!processed) G4cout<<"LArG4H62004CalibSD name: "<<GetName()<<" failure !!"<<G4endl;
  return true;
}

G4bool LArG4H62004CalibSD::SpecialHit(G4Step* a_step,
                                      const std::vector<G4double>& a_energies)
{
#ifdef DEBUG_SD
  std::cout<<"LArG4H62004CalibSD::SpecialHit called, calling calculator..."<<std::endl;
#endif
  // Convert the G4Step into an identifier.
  LArG4Identifier ident;
  std::vector<double> energies;
  G4bool valid = m_calculator->Process( a_step, ident, energies, LArG4::kOnlyID );

  // If we can't calculate the identifier, something is wrong.
  if ( ! valid ) return false;

  // Changing the ident to comply with H6 dictionary
  if(ident[1] == 1) { // EMEC module
    ident[6] += 8; // change phi id
    if(ident[6] < 20) ident[6] = 20;
    if(ident[6] > 27) ident[6] = 27;
  }else if(ident[1] == 2) { // HEC module
    if(ident[4] == 0) {
      if(ident[3]==0 && ident[5] < 6) ident[5]=6;
      if(ident[3]==2 && ident[5] < 8) ident[5]=8;
    } else {
      if(ident[3]==1 && ident[5] > 2) ident[5]=2;
    }
  }

  bool processed = SimpleHit( ident, a_energies, m_calibrationHits );
  return processed;
}
