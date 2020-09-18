/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// LArG4TBECBeamChSensitiveDetector
#undef DEBUG_SD
#undef DEBUG_HITS

#include "LArG4TBECBeamChSensitiveDetector.h"
#include "LArG4TBECBeamChCalculator.h"
#include "LArG4TBECBeamChHit.h"

#include "G4Step.hh"
#include "G4ios.hh"

#include <map>
#include <vector>

LArG4TBECBeamChSensitiveDetector::LArG4TBECBeamChSensitiveDetector(G4String a_name)
  : G4VSensitiveDetector(a_name)
  , m_calculator(new LArG4TBECBeamChCalculator())
  , m_detectorName(a_name)
{
  // Construct a unique name for the collection of hits maintained by
  // a sensitive detector.  Hit collection names can't have any
  // slashes ("/"), because that confuses the name with the sensitive
  // detector names.
  G4String HCname = m_detectorName.strip(G4String::both,'/');
  if ( HCname.contains('/') )
    {
      G4int lastSlash = HCname.last('/');
      HCname.remove(0,lastSlash+1);
    }

  // This is the name to search for in the hit-collection table.
  // Given a detector whose name is "/something/blah/duh/yada", the
  // hit collection name will be "yadaHits".
  m_HCname = HCname + "::Hits";

  // "collectionName" is defined in G4VSensitiveDetector.hh.
  collectionName.insert(m_HCname);

#ifdef DEBUG_SD
        G4cout << "LArG4TBECBeamChSensitiveDetector: Sensitive Detector <"
                   << m_detectorName << "> has hit collection <" << m_HCname
                   << ">" << G4endl;
#endif
}


LArG4TBECBeamChSensitiveDetector::~LArG4TBECBeamChSensitiveDetector()
{
}


void LArG4TBECBeamChSensitiveDetector::Initialize(G4HCofThisEvent* /*m_HCE*/)
{
        m_Hits.clear();
#ifdef DEBUG_SD
        G4cout << "LArG4TBECBeamChSensitiveDetector: initialized " << G4endl;
#endif
}


G4bool LArG4TBECBeamChSensitiveDetector::ProcessHits(G4Step* a_step,
                                                     G4TouchableHistory* /*ROhist*/)
{
        if(!m_calculator->Process(a_step)) return false;
        G4int ident = m_calculator->identifier();
        G4double x = m_calculator->x();
        G4double y = m_calculator->y();

#ifdef DEBUG_HITS
        std::cout << "LArG4TBECBeamChSensitiveDetector                    "
            << " ID=" << ident
            << " x=" << x
            << " y=" << y
            << std::endl;
#endif

        LArG4TBECBeamChHit* hit = new LArG4TBECBeamChHit(ident, x, y);

#ifdef DEBUG_HITS
        std::cout << "LArG4TBECBeamChSensitiveDetector hit = " << hit << std::endl;
#endif

        G4bool found = false;
        for (const LArG4TBECBeamChHit* hit : m_Hits) {
                if(hit->getIdentifier() == ident){
                        found = true;
                        break;
                }
        }
        if(!found){
                m_Hits.push_back(hit);
                return true;
        } else {
#ifdef DEBUG_HITS
        std::cout << "LArG4TBECBeamChSensitiveDetector duplicate hit in"
                  << ident << std::endl;
#endif
                return false;
        }
}

void LArG4TBECBeamChSensitiveDetector::EndOfEvent(G4HCofThisEvent* /*m_HCE*/)
{
#ifdef DEBUG_SD
        G4cout << "LArG4TBECBeamChSensitiveDetector::EndOfEvent: "
               << "collectionName[0]=" << collectionName[0] << G4endl
                   << "LArG4TBECBeamChSensitiveDetector::EndOfEvent: "
                   << " m_Hits.size()=" << m_Hits.size() << G4endl;
#endif

#ifdef DEBUG_HITS
        m_Hit_pointer i;
        for(i = m_Hits.begin(); i != m_Hits.end(); i ++){
                LArG4TBECBeamChHit* hit = *i;
                std::cout << "LArG4TBECBeamChSensitiveDetector::EndOfEvent"
                          << " ID= " << hit->getIdentifier()
                          << " x=" << hit->X()
                                  << " y=" << hit->Y()
                                  << std::endl;
        }
#endif
}
