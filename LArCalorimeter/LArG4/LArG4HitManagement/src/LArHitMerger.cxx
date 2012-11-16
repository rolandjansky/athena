/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4HitManagement/LArHitMerger.h"
#include "LArSimEvent/LArHitContainer.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"

LArHitMerger::LArHitMerger(StoreGateSvc* detStore,
			   LArHitsEventAction* action,
			   std::string timeBinType,
			   G4float timeBinWidth):
  LArVHitMerger(detStore),
  m_action(action),
  m_timeBinWidth(timeBinWidth)
{
  // Set time bin type
  if(timeBinType == "Default")
    m_timeBinType = LArHitMerger::HitTimeBinDefault;
  else if(timeBinType == "Uniform")
    m_timeBinType = LArHitMerger::HitTimeBinUniform;
  else // Unexpected value - fail over default
    m_timeBinType = LArHitMerger::HitTimeBinDefault;
}

LArHitMerger::~LArHitMerger()
{
}

void LArHitMerger::BeginOfEvent()
{
}

bool LArHitMerger::process(G4Step* /*step*/,
			   LArG4Identifier ident,
			   G4double time,
			   G4double energy)
{
  // Build the hit from the calculator results.
  Identifier id = ConvertID( ident );
  if (!id.is_valid()) 
    return false;

  G4int timeBin = getTimeBin(time);  
  
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

void LArHitMerger::EndOfEvent()
{
  StoredLArHitContainers* storedContainers = m_action->getStoredContainers();

  LArHitContainer* embHitCollection = storedContainers->embHitCollection; 
  LArHitContainer* emecHitCollection = storedContainers->emecHitCollection;
  LArHitContainer* fcalHitCollection = storedContainers->fcalHitCollection;
  LArHitContainer* hecHitCollection = storedContainers->hecHitCollection;
  LArHitContainer* miniFcalHitCollection = storedContainers->miniFcalHitCollection;

  // For each time bin...
  for(timeBins_const_pointer i = m_timeBins.begin(); i != m_timeBins.end(); i++)
  {
    if(m_log->level()==MSG::DEBUG)
      (*m_log) << MSG::DEBUG << "EndOfEvent: time bin " << (*i).first
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

      if(m_larEmID->is_lar_em(id))
      {
	if(m_larEmID->is_em_barrel(id))
	{
	  embHitCollection->push_back(hit);
	}
	else if(m_larEmID->is_em_endcap(id))
	{
	  emecHitCollection->push_back(hit);
	}
      }
      else if(m_larMiniFcalID->is_initialized() && m_larMiniFcalID->is_lar_minifcal(id))
      {
	miniFcalHitCollection->push_back(hit);
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
  }

  clear();
}

void LArHitMerger::clear()
{
  for(timeBins_pointer i = m_timeBins.begin(); i!= m_timeBins.end(); i++)
    delete (*i).second;
  m_timeBins.clear();
}

G4int LArHitMerger::getTimeBin(G4double time) 
{
  G4int timeBin = INT_MAX;

  if(m_timeBinType==LArHitMerger::HitTimeBinDefault)
  {
    if(time<0.) 
      time=0.;
    if(time<10.) 
      timeBin = int(time/2.5);
    else if(time<50.)
      timeBin = 4 + int((time-10.)/5.);
    else if(time<100.)
      timeBin = 12 + int((time-50.)/25.);
    else
      timeBin = 14;
  }
  else
  {
    // Uniform binning by 2.5 ns
    G4double timeBinf = time/m_timeBinWidth;
    
    if (timeBinf < G4double(INT_MAX))
      timeBin = G4int(timeBinf);
  }

  return timeBin;
} 
