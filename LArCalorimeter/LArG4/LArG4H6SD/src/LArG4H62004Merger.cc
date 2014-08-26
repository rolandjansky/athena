/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4H6SD/LArG4H62004Merger.h"
#include "LArSimEvent/LArHitContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"

LArG4H62004Merger::LArG4H62004Merger(StoreGateSvc* detStore,
			   G4float timeBinWidth):
  LArVHitMerger(detStore),
  m_timeBinWidth(timeBinWidth)
{
}

LArG4H62004Merger::~LArG4H62004Merger()
{
}

void LArG4H62004Merger::BeginOfEvent()
{
}

bool LArG4H62004Merger::process(G4Step* /*step*/,
			   LArG4Identifier ident,
			   G4double time,
			   G4double energy)
{
  // The hits are separated into different time bins, with the
  // bin width determined by user input.
  
  G4double timeBinf = time / m_timeBinWidth ;
  
  // As strange as it sounds, it's theoretically possible for
  // the integer time bin to exceed the size of an integer
  // number on a computer; for example, consider a neutron decay
  // (~20 min) compared to a typical time bin width of 2.5 ns.
  // Therefore, put in a check against numeric overflow.
  // (INT_MAX, the largest possible integer that can be
  // represented in C++, is defined in "#include <climits>".)
  
  G4int timeBin = INT_MAX;
  if (timeBinf < G4double(INT_MAX))
    timeBin = G4int(timeBinf);
  
  // Find the set of hits for this time bin.  If this is the
  // first hit in this bin, create a new set.
  
  hits_t* hitCollection = 0;
  timeBins_pointer setForThisBin = m_timeBins.find( timeBin );

  if (setForThisBin == m_timeBins.end())
  {
    // New time bin
    hitCollection = new hits_t;
    m_timeBins[ timeBin ] = hitCollection;
  }
  else
  {
    // Get the existing set of hits for this time bin.
    // Reminders:
    // setForThisBin = iterator (pointer) into the m_timeBins map
    // (*setForThisBin) = pair< G4int, m_hits_t* >
    // (*setForThisBin).second = m_hits_t*, the pointer to the set of hits
      
    hitCollection = (*setForThisBin).second;
  }
  
  // Build the hit from the calculator results.
  Identifier id = ConvertID( ident );
  if (!id.is_valid())
    return false;
  
  LArHit* hit = new LArHit(id,energy,time);

  // If we haven't had a hit in this cell before, create one and add
  // it to the hit collection.
  
  // If we've had a hit in this cell before, then add the energy to
  // the existing hit.
  
  // Look for the key in the hitCollection (this is a binary search).
  hits_pointer bookmark = hitCollection->lower_bound(hit);
  
  // The lower_bound method of a map finds the first element
  // whose key is not less than the identifier.  If this element
  // == our hit, we've found a match.
  
  // Reminders:
  // bookmark = iterator (pointer) into the hitCollection set.
  // (*bookmark) = a member of the set, which is a LArG4Hit*.
  
  // Equals() is a function defined in LArG4Hit.h; it has the value of
  // "true" when a LArG4Hit* points to the same identifier.
  
  if (bookmark == hitCollection->end() ||
      !(*bookmark)->Equals(hit))
  {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.
    if (hitCollection->empty() || 
	bookmark == hitCollection->begin())
    {
      // Insert the hit before the first entry in the map.
      hitCollection->insert(hit);
    }
    else
    {
      // We'just done a binary search of hitCollection, so we should use
      // the results of that search to speed up the insertion of a new
      // hit into the map.  The "insert" method is faster if the new
      // entry is right _after_ the bookmark.  If we left bookmark
      // unchanged, the new entry would go right _before_ the
      // bookmark.  We therefore want to decrement the bookmark from
      // the lower_bound search.
      
      hitCollection->insert(--bookmark, hit);
    }
  }
  else
  {
    // Update the existing hit.
    (*bookmark)->Add(hit);
    
    // We don't need our previously-created hit anymore.  
    delete hit;	
  }

  return true;
}

void LArG4H62004Merger::EndOfEvent()
{ 

  StatusCode status;
  StoredLArHitContainers* storedContainers = 0;

  status = m_detStore->retrieve(storedContainers);
  if (status.isFailure())
  {
    (*m_log) << MSG::ERROR << "LArG4H62004Merger::EndOfEvent: Unable to retrieve StoredContainers" << endreq;
    return;
  }
  else
    (*m_log) << MSG::DEBUG << "LArG4H62004Merger::EndOfEvent: StoredContainers retrieved" << endreq;
  LArHitContainer* emecHitCollection = storedContainers->emecHitCollection;
  LArHitContainer* fcalHitCollection = storedContainers->fcalHitCollection;
  LArHitContainer* hecHitCollection = storedContainers->hecHitCollection;


  // Insert all the hits we've gathered, for all the time bins, into
  // the G4Event hit collection.  Note that G4Event will "adopt" these
  // objects and is responsible for deleting them.

  // For each time bin...
  for(timeBins_const_pointer i = m_timeBins.begin(); i != m_timeBins.end(); i++)
  {
    (*m_log) << MSG::DEBUG << "LArG4H62004Merger::EndOfEvent: time bin " << (*i).first
	     << " - #hits = " << (*i).second->size() << endreq;

    const hits_t* hitSet = (*i).second;

    // For each hit in the set...
    for(hits_pointer j = hitSet->begin(); j != hitSet->end(); j++)
    {
      // Reminders:
      // j = iterator (pointer) into the hitSet.
      // (*j) = a member of the set, which is a LArG4Hit*.
      LArHit* hit = *j;
      hit->finalize();
      Identifier id  = hit->cellID();

      if(m_larEmID->is_em_endcap(id))
      {
        emecHitCollection->push_back(hit);
      }
      else if(m_larFcalID->is_lar_fcal(id))
      {
        fcalHitCollection->push_back(hit);
      }
      else if(m_larHecID->is_lar_hec(id))
      {
        hecHitCollection->push_back(hit);
      }
    }
      // We don't need the hit collection for this time bin anymore.
  } // for each time bin

  clear();
}

void LArG4H62004Merger::clear()
{
  for(timeBins_pointer i = m_timeBins.begin(); i!= m_timeBins.end(); i++)
    delete (*i).second;
  m_timeBins.clear();
}

