/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/LArG4SimpleSD.h"

#include "LArG4Code/ILArCalculatorSvc.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID_Exception.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "StoreGate/StoreGateSvc.h"
#include "LArSimEvent/LArHitContainer.h"

#include "G4RunManager.hh"
#include "MCTruth/AtlasG4EventUserInfo.h"

#include "G4Step.hh"
#ifdef DEBUG_IDENTIFIERS
#include "G4ThreeVector.hh"
#endif

LArG4SimpleSD::LArG4SimpleSD(G4String a_name, ILArCalculatorSvc* calc, const std::string& type, const float width)
  : G4VSensitiveDetector(a_name)
  , m_calculator(calc)
  , m_numberInvalidHits(0)
  , m_timeBinType(LArG4SimpleSD::HitTimeBinDefault)
  , m_timeBinWidth(width)
  , m_larEmID(nullptr)
  , m_larFcalID(nullptr)
  , m_larHecID(nullptr)
  , m_larMiniFcalID(nullptr)
{
  // Only one string causes a change in action
  if(type == "Uniform")
    m_timeBinType = LArG4SimpleSD::HitTimeBinUniform;
}

LArG4SimpleSD::LArG4SimpleSD(G4String a_name, StoreGateSvc* detStore)
  : G4VSensitiveDetector(a_name)
  , m_calculator(nullptr)
  , m_numberInvalidHits(0)
  , m_timeBinType(LArG4SimpleSD::HitTimeBinDefault)
  , m_timeBinWidth(2.5*CLHEP::ns)
  , m_larEmID (nullptr)
  , m_larFcalID (nullptr)
  , m_larHecID (nullptr)
  , m_larMiniFcalID (nullptr)
{
  // This should only be used when it's safe to do this retrieval
  const CaloIdManager* caloIdManager=nullptr;
  if ( detStore->retrieve(caloIdManager).isFailure() ){
    throw std::runtime_error("Could not retrieve Calo ID manager");
  }

  const LArEM_ID* larEmID = caloIdManager->getEM_ID();
  if(larEmID==0)
    throw std::runtime_error("IFastSimDedicatedSD: Invalid LAr EM ID helper");

  const LArFCAL_ID* larFcalID = caloIdManager->getFCAL_ID();
  if(larFcalID==0)
    throw std::runtime_error("IFastSimDedicatedSD: Invalid FCAL ID helper");

  const LArHEC_ID* larHecID = caloIdManager->getHEC_ID();
  if(larHecID==0)
    throw std::runtime_error("IFastSimDedicatedSD: Invalid HEC ID helper");

  const LArMiniFCAL_ID* larMiniFcalID = caloIdManager->getMiniFCAL_ID();
  if(larMiniFcalID==0)
    throw std::runtime_error("IFastSimDedicatedSD: Invalid Mini FCAL ID helper");

  setupHelpers( larEmID, larFcalID, larHecID, larMiniFcalID );
}

LArG4SimpleSD::~LArG4SimpleSD()
{
  // I owned my own calculator
  delete m_calculator;
  if(verboseLevel>5 && m_numberInvalidHits>0) {
    G4cout << "Destructor: Sensitive Detector <" << SensitiveDetectorName << "> had " << m_numberInvalidHits
           << " G4Step energy deposits outside the region determined by its Calculator." << G4endl;
  }
}

G4bool LArG4SimpleSD::ProcessHits(G4Step* a_step,G4TouchableHistory*)
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
  std::vector<LArHitData> hits;

  if(!(m_calculator->Process(a_step, hits))) {
    m_numberInvalidHits++;
    return false;
  }

  // A calculator can determine that a given energy deposit results
  // in more than one hit in the simulation.  For each such hit...
  G4bool result = true;
#ifdef DEBUG_IDENTIFIERS
  const G4StepPoint* pre_step_point = a_step->GetPreStepPoint();
  const G4ThreeVector startPoint = pre_step_point->GetPosition();
  const G4StepPoint* post_step_point = a_step->GetPostStepPoint();
  const G4ThreeVector endPoint = post_step_point->GetPosition();
  const G4ThreeVector p = startPoint;
  const int side = (p.z()<0) ? -1 : 1;
#endif
  for(const auto &ihit : hits) {
#ifdef DEBUG_IDENTIFIERS
    if (ihit.id[2]*side<0 && std::abs(ihit.id[1])<4) {
      G4cout << SensitiveDetectorName << "::ProcessHits ERROR G4Step position "<< "(" << p.x() << ", " << p.y() << ", " << p.z() << ") is inconsistent with LArG4Identifier assigned:" << G4endl;
      G4cout << "     LArG4Identifier from " << m_calculator->name() << " tech : " << ihit.id[1] << ", barrel_ec : " << ihit.id[2]
             << ", sampling : " << ihit.id[3]
             << ",   region : " << ihit.id[4]
             << ",      eta : " << ihit.id[5]
             << ",      phi : " << ihit.id[6] << G4endl;
      G4cout << "     startPoint: (" << startPoint.x() << ", " << startPoint.y() << ", " << startPoint.z() << ")" << G4endl;
      G4cout << "       endPoint: (" << endPoint.x() << ", " << endPoint.y() << ", " << endPoint.z() << ")" << G4endl;
    }
#endif
    // Ported in from the old LArG4HitMerger code
    result = result && SimpleHit( ihit.id ,
                                  ihit.time,
                                  ihit.energy );
  }
  return result;
}

G4bool LArG4SimpleSD::SimpleHit( LArG4Identifier lar_id , G4double time , G4double energy )
{
  // Build the hit from the calculator results.
  Identifier id = ConvertID( lar_id );
  if (!id.is_valid()) return false;

  G4int timeBin = getTimeBin( time );
    
  // Find the set of hits for this time bin.  If this is the
  // first hit in this bin, create a new set.
    
  hits_t* hitCollection = 0;
  auto setForThisBin = m_timeBins.find( timeBin );
    
  if (setForThisBin == m_timeBins.end()) {
    // New time bin
    hitCollection = new hits_t;
    m_timeBins[ timeBin ] = hitCollection;
  } else {
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
  auto bookmark = hitCollection->lower_bound(hit);
    
  // The lower_bound method of a map finds the first element
  // whose key is not less than the identifier.  If this element
  // == our hit, we've found a match.
    
  // Reminders:
  // bookmark = iterator (pointer) into the hitCollection set.
  // (*bookmark) = a member of the set, which is a LArG4Hit*.
    
  // Equals() is a function defined in LArG4Hit.h; it has the value of
  // "true" when a LArG4Hit* points to the same identifier.
    
  if (bookmark == hitCollection->end() ||
      !(*bookmark)->Equals(hit)) {
    // We haven't had a hit in this readout cell before.  Add it
    // to our set.
    if (hitCollection->empty() ||
        bookmark == hitCollection->begin()) {
      // Insert the hit before the first entry in the map.
      hitCollection->insert(hit);
    } else {
      // We'just done a binary search of hitCollection, so we should use
      // the results of that search to speed up the insertion of a new
      // hit into the map.  The "insert" method is faster if the new
      // entry is right _after_ the bookmark.  If we left bookmark
      // unchanged, the new entry would go right _before_ the
      // bookmark.  We therefore want to decrement the bookmark from
      // the lower_bound search.
            
      hitCollection->insert(--bookmark, hit);
    }
  } else {
    // Update the existing hit.
    (*bookmark)->Add(hit);
        
    // We don't need our previously-created hit anymore.
    delete hit;
  }
  
  return true;
}


G4int LArG4SimpleSD::getTimeBin(G4double time) const
{
    G4int timeBin = INT_MAX;
    
    if(m_timeBinType==LArG4SimpleSD::HitTimeBinDefault)
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


void LArG4SimpleSD::EndOfAthenaEvent( LArHitContainer * hitContainer )
{
  // For each time bin...
  for(const auto i : m_timeBins) {
    if (verboseLevel>5)
      G4cout << "EndOfEvent: time bin " << i.first << " - #hits = " << i.second->size() << G4endl;
      
    const hits_t* hitSet = i.second;
      
    // For each hit in the set...
    for(auto hit : *hitSet){
      // Because of the design, we are sure this is going into the right hit container
      hit->finalize();
      hitContainer->push_back(hit);
    } // End of loop over hits in the set

  } // End of loop over time bins
    
  for(auto i : m_timeBins) delete i.second;
  m_timeBins.clear();
}

Identifier LArG4SimpleSD::ConvertID(const LArG4Identifier& a_ident) const
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
                    << (std::string) e
                    << G4endl;
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
                    << (std::string) e
                    << G4endl;
                }
            }
        }
    }
    return id;
}
