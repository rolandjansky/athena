/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HitManagement/LArCalibHitMerger.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloDM_ID.h"

#include <string>

LArCalibHitMerger::LArCalibHitMerger(StoreGateSvc* detStore,
				     LArHitsEventAction* action):
  LArVCalibHitMerger(detStore),
  m_action(action)
{
}

LArCalibHitMerger::~LArCalibHitMerger()
{
}

void LArCalibHitMerger::BeginOfEvent()
{
}

bool LArCalibHitMerger::process(const LArG4Identifier& ident,
				const std::vector<G4double>& energies)
{
  Identifier id = ConvertID(ident);
  if(!id.is_valid())
    return false;

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
						   energies[3]);


  // If we haven't had a hit in this cell before, add the current hit
  // to the hit collection.

  // If we've had a hit in this cell before, then add this to the
  // previously-existing hit.

  // Look for the hit in the m_calibrationHits (this is a binary search).
  m_calibrationHits_ptr_t bookmark = m_calibrationHits.lower_bound(hit);
  
  // The lower_bound method of a set finds the first element not less
  // the hit.  If this element == our hit, we've found a match.
  
  // Reminders:
  // bookmark = iterator (pointer) into the m_calibrationHits set.
  // (*bookmark) = a member of the set, which is a CalibrationHit*.
  
  // Equals() is a function defined in CalibrationHit.h; it has the value of
  // "true" when a CalibrationHit* points to the same identifier.
  
  if(bookmark == m_calibrationHits.end() || !(*bookmark)->Equals(hit))
  {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.

    if(m_calibrationHits.empty() || bookmark == m_calibrationHits.begin())
    {
      // Insert the hit before the first entry in the map.
      m_calibrationHits.insert(hit);
    }
    else
    {
      // We've just done a binary search of m_calibrationHits,
      // so we should use the results of that search to speed up
      // the insertion of a new hit into the map.  The "insert"
      // method is faster if the new entry is right _after_ the
      // bookmark.  If we left bookmark unchanged, the new entry
      // would go right _before_ the bookmark.  We therefore
      // want to decrement the bookmark from the lower_bound
      // search.
      
      m_calibrationHits.insert(--bookmark, hit);
    }
  }
  else
  {
    // Update the existing hit. 
    (*bookmark)->Add(hit);
    
    // We don't need the hit anymore.  Remember that we've adopted
    // the hit, so we're responsible for deleting it if needed.
    delete hit;
  }

  return true;
}


bool LArCalibHitMerger::process(const LArG4Identifier& ident,
                                const std::vector<G4double>& energies, 
				unsigned int particleID)
{
  Identifier id = ConvertID(ident);
  if(!id.is_valid())
    return false;

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

  // If we haven't had a hit in this cell before, add the current hit
  // to the hit collection.

  // If we've had a hit in this cell before, then add this to the
  // previously-existing hit.

  // Look for the hit in the m_calibrationHits (this is a binary search).
  m_calibrationHits_ptr_t bookmark = m_calibrationHits.lower_bound(hit);
  
  // The lower_bound method of a set finds the first element not less
  // the hit.  If this element == our hit, we've found a match.
  
  // Reminders:
  // bookmark = iterator (pointer) into the m_calibrationHits set.
  // (*bookmark) = a member of the set, which is a CalibrationHit*.
  
  // Equals() is a function defined in CalibrationHit.h; it has the value of
  // "true" when a CalibrationHit* points to the same identifier.
  
  if(bookmark == m_calibrationHits.end() || !(*bookmark)->Equals(hit))
  {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.

    if(m_calibrationHits.empty() || bookmark == m_calibrationHits.begin())
    {
      // Insert the hit before the first entry in the map.
      m_calibrationHits.insert(hit);
    }
    else
    {
      // We've just done a binary search of m_calibrationHits,
      // so we should use the results of that search to speed up
      // the insertion of a new hit into the map.  The "insert"
      // method is faster if the new entry is right _after_ the
      // bookmark.  If we left bookmark unchanged, the new entry
      // would go right _before_ the bookmark.  We therefore
      // want to decrement the bookmark from the lower_bound
      // search.
      
      m_calibrationHits.insert(--bookmark, hit);
    }
  }
  else
  {
    // Update the existing hit. 
    (*bookmark)->Add(hit);
    
    // We don't need the hit anymore.  Remember that we've adopted
    // the hit, so we're responsible for deleting it if needed.
    delete hit;
  }

  return true;
}


void LArCalibHitMerger::EndOfEvent(std::string detectorName)
{
  StoredLArCalibHitContainers* storedContainers = m_action->getStoredCalibContainers();

  CaloCalibrationHitContainer* deadHitCollection = storedContainers->deadHitCollection;
  CaloCalibrationHitContainer* inactiveHitCollection = storedContainers->inactiveHitCollection;
  CaloCalibrationHitContainer* activeHitCollection = storedContainers->activeHitCollection;

  m_calibrationHits_ptr_t i;
  for(i = m_calibrationHits.begin(); i != m_calibrationHits.end(); i++)
  {
    CaloCalibrationHit* hit = *i;
    Identifier id = hit->cellID();

    if(m_caloDmID->is_lar_dm(id) || m_caloDmID->is_tile_dm(id))
    {
      deadHitCollection->push_back(hit);
    }
    else if(detectorName.find("::Inactive")!=std::string::npos)
    {
      inactiveHitCollection->push_back(hit);
    }
    else if(detectorName.find("::Dead")!=std::string::npos)
    {
      inactiveHitCollection->push_back(hit);
    }
    else
    {
      // If it's not dead or inactive, then it's active.
      activeHitCollection->push_back(hit);
    }
  }

  clear();
}

void LArCalibHitMerger::clear()
{
  m_calibrationHits.clear();
}

