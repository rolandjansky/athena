/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArG4CalibSD.h"

#include "LArG4Code/VCalibrationCalculator.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "G4RunManager.hh"
#include "MCTruth/EventInformation.h"

#include "G4Step.hh"

LArG4CalibSD::LArG4CalibSD(G4String a_name, LArG4::VCalibrationCalculator* calc, bool doPID)
  : G4VSensitiveDetector(a_name)
  , m_calculator(calc)
  , m_numberInvalidHits(0)
  , m_doPID(doPID)
  , m_larEmID (nullptr)
  , m_larFcalID (nullptr)
  , m_larHecID (nullptr)
  , m_larMiniFcalID (nullptr)
  , m_caloDmID (nullptr)
{}

LArG4CalibSD::~LArG4CalibSD()
{
  // I owned my own calculator
  delete m_calculator;
  if(verboseLevel>5 && m_numberInvalidHits>0) {
    G4cout << "Destructor: Sensitive Detector <" << SensitiveDetectorName << "> had " << m_numberInvalidHits
           << " G4Step energy deposits outside the region determined by its Calculator." << G4endl;
  }
}

G4bool LArG4CalibSD::ProcessHits(G4Step* a_step,G4TouchableHistory*)
{
  // If there's no energy, there's no hit.  (Aside: Isn't this energy
  // the same as the energy from the calculator?  Not necessarily.
  // The calculator may include detector effects such as
  // charge-collection which are not modeled by Geant4.)
  if(a_step->GetTotalEnergyDeposit() == 0.) return false;

  // Convert the G4Step into (eta,phi,sampling).
  // Check that hit was valid.  (It might be invalid if, for example,
  // it occurred outside the sensitive region.  If such a thing
  // happens, it means that the geometry definitions in the
  // detector-construction routine and the calculator do not agree.)
  if(!(m_calculator->Process(a_step))) {
    m_numberInvalidHits++;
    return false;
  }

  return SimpleHit( m_calculator->identifier() , m_calculator->energies() );
}

G4bool LArG4CalibSD::SimpleHit( const LArG4Identifier& a_ident , const std::vector<double>& energies ){

  // retreive particle ID
  unsigned int particleID = 0;
  if( m_doPID ) {
    EventInformation * event_info = dynamic_cast<EventInformation*>(G4RunManager::GetRunManager()->GetCurrentEvent()->GetUserInformation());
    if (event_info) particleID = event_info->GetCurrentPrimary()->barcode();
  }

  // Build the hit from the calculator results.
  Identifier id = ConvertID( a_ident );
  if (!id.is_valid()) return true;

  // Reject cases where invisible energy calculation produced values
  // of order 10e-12 instead of 0 due to rounding errors in that
  // calculation, or general cases where the energy deposits are
  // trivially small.
  if(energies[0]+energies[1]+energies[3] < 0.001*CLHEP::eV && fabs(energies[2]) < 0.001*CLHEP::eV ) {
    return true;
  }
      
  // Build the hit.
  CaloCalibrationHit* hit = new CaloCalibrationHit(id,
                                                   energies[0],
                                                   energies[1],
                                                   energies[2],
                                                   energies[3],
                                                   particleID);

  // If we haven't had a hit in this cell before, create one and add
  // it to the hit collection.
      
  // If we've had a hit in this cell before, then add the energy to
  // the existing hit.
      
  // Look for the key in the hitCollection (this is a binary search).
  auto bookmark = m_calibrationHits.lower_bound(hit);
 
  // The lower_bound method of a map finds the first element
  // whose key is not less than the identifier.  If this element
  // == our hit, we've found a match.
    
  // Reminders:
  // bookmark = iterator (pointer) into the hitCollection set.
  // (*bookmark) = a member of the set, which is a LArG4Hit*.
    
  // Equals() is a function defined in LArG4Hit.h; it has the value of
  // "true" when a LArG4Hit* points to the same identifier.
    
  if (bookmark == m_calibrationHits.end() ||
      !(*bookmark)->Equals(hit)) {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.
    if (m_calibrationHits.empty() ||
        bookmark == m_calibrationHits.begin()) {
      // Insert the hit before the first entry in the map.
      m_calibrationHits.insert(hit);
    } else {
      // We'just done a binary search of hitCollection, so we should use
      // the results of that search to speed up the insertion of a new
      // hit into the map.  The "insert" method is faster if the new
      // entry is right _after_ the bookmark.  If we left bookmark
      // unchanged, the new entry would go right _before_ the
      // bookmark.  We therefore want to decrement the bookmark from
      // the lower_bound search.
            
      m_calibrationHits.insert(--bookmark, hit);
    }
  } else {
    // Update the existing hit.
    (*bookmark)->Add(hit);
        
    // We don't need our previously-created hit anymore.
    delete hit;
  }

  return true;
}


// Something special has happened (probably the detection of escaped
// energy in CaloG4Sim/SimulationEnergies).  We have to bypass the
// regular sensitive-detector processing.  Determine the identifier
// (and only the identifier) using the calculator, then built a hit
// with that identifier and the energies in the vector.

G4bool LArG4CalibSD::SpecialHit(G4Step* a_step,
                                const std::vector<G4double>& a_energies)
{
  // If we can't calculate the identifier, something is wrong.
  if (!(m_calculator->Process( a_step, LArG4::VCalibrationCalculator::kOnlyID))) return false;

  return SimpleHit( m_calculator->identifier() , a_energies );
} 
 

void LArG4CalibSD::EndOfAthenaEvent( CaloCalibrationHitContainer * hitContainer )
{
  // Loop through the hits...
  for(auto hit : m_calibrationHits)
  {

    // Because of the design, we are sure this is going into the right hit container
    // Can we actually do this with move?
    hitContainer->push_back(hit);

  } // End of loop over hits

  // Clean up    
  m_calibrationHits.clear();
}


Identifier LArG4CalibSD::ConvertID(const LArG4Identifier& a_ident) const
{
    Identifier id;
    
    if(a_ident[0]==4)
    {
        // is LAr
        if(a_ident[1]==1)
        {
            //is LAr EM
            try
            {
                id = m_larEmID->channel_id(a_ident[2],  // barrel_ec
                                           a_ident[3],  // sampling
                                           a_ident[4],  // region
                                           a_ident[5],  // eta
                                           a_ident[6]); // phi
            }
            catch (LArID_Exception& e)
            {
                G4cout << "ERROR ConvertID: LArEM_ID error code " << e.code() << " "
                << (std::string) e << G4endl;
            }
        }
        else if(a_ident[1]==2)
        {
            //is EM HEC
            try
            {
                id = m_larHecID->channel_id(a_ident[2],  // zSide
                                            a_ident[3],  // sampling
                                            a_ident[4],  // region
                                            a_ident[5],  // eta
                                            a_ident[6]); // phi
            }
            catch(LArID_Exception& e)
            {
                G4cout << "ERROR ConvertID: BuildHitCollections: LArHEC_ID error code " << e.code() << " "
                << (std::string) e << G4endl;
            }
        }
        else if(a_ident[1]==3)
        {
            // FCAL
            if(a_ident[3]>0)
            {
                //is EM FCAL
                try
                {
                    id = m_larFcalID->channel_id(a_ident[2],  // zSide
                                                 a_ident[3],  // sampling
                                                 a_ident[4],  // eta
                                                 a_ident[5]); // phi
                }
                catch(LArID_Exception& e)
                {
                    G4cout << "ERROR ConvertID:: LArFCAL_ID error code " << e.code() << " "
                    << (std::string) e << G4endl;
                }
            }
            else
            {
                //is Mini FCAL
                try
                {
                    id = m_larMiniFcalID->channel_id(a_ident[2],  // zSide
                                                     a_ident[3],  // module
                                                     a_ident[4],  // depth
                                                     a_ident[5],  // eta
                                                     a_ident[6]); // phi
                }
                catch(LArID_Exception& e)
                {
                    G4cout << "ERROR ConvertID:: LArMiniFCAL_ID error code " << e.code() << " "
                    << (std::string) e << G4endl;
                }
            }
        }
    }
    else if(a_ident[0]==10)
    {
      // This is a dead-material identifier
      try 
      {
        id = m_caloDmID->zone_id(a_ident[1],  // zSide
                     a_ident[2],  // type
                     a_ident[3],  // sampling
                     a_ident[4],  // region
                     a_ident[5],  // eta
                     a_ident[6]); // phi
      }
      catch(CaloID_Exception& e)
      {
        G4cout << "ERROR ConvertID: CaloDM_ID error code " << e.code() << " "
               << (std::string) e << G4endl;
      }
    }

    return id;
}
