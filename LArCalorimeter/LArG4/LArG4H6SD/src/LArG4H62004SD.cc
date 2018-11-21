/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H62004SD.h"

#include "LArG4Code/ILArCalculatorSvc.h"

#include "G4Step.hh"
#include "G4ios.hh"

#include <vector>

#undef DEBUG_ME

LArG4H62004SD::LArG4H62004SD(G4String a_name, ILArCalculatorSvc* calc, const std::string& type, const float width)
  : LArG4SimpleSD(a_name,calc,type,width)
{

  // Set appropriate calculator
#ifdef DEBUG_ME
  std::cout<<"LArG4H62004SD::LArG4H62004SD name: "<<a_name<<std::endl;
#endif

  // Get pointer to the message service
  if (verboseLevel>5) G4cout << "--- Call LArG4H62004SD for detector " << a_name << std::endl;

}


G4bool LArG4H62004SD::ProcessHits(G4Step* a_step,G4TouchableHistory* /*ROhist*/)
{
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)

  G4double edep = a_step->GetTotalEnergyDeposit() * a_step->GetTrack()->GetWeight();
#ifdef DEBUG_ME
  G4cout << "ProcessHits: my name: "<<this->GetName()<<", calc.: "<<m_calculator<<", energy: "<<edep << G4endl;
#endif
  if (edep == 0.) return false;

  // Convert the G4Step into (eta,phi,sampling).
  std::vector<LArHitData> hdata;
  G4bool valid = m_calculator->Process(a_step, hdata);

  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)

  if ( ! valid )
    {
      m_numberInvalidHits++;
      return false;
    }

  // A calculator can determine that a given energy deposit results
  // in more than one hit in the simulation.  FOr each such hit...
  bool result = true;
  for(auto larhit : hdata)
    {
      LArG4Identifier ident = larhit.id;
      if(ident[0] == 10) { // dead hit, where to put it ?
        // probably skip in the regular container
        continue;
      }
      G4double time = larhit.time;
      G4double energy = larhit.energy;
      // Changing the ident to comply with H6 dictionary
      if(ident[1] == 1) { // EMEC module
        ident[6] += 20; // change phi id
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

      result = result && SimpleHit( ident, time, energy );

    }// for each hit return by the calculator.
  return result;

}
