/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// EscapedEnergyRegistry
// 15-Jul-2004 William Seligman
// 12-Apr-2019 Marilena Bandieramonte
//
#include "CaloG4Sim/EscapedEnergyRegistry.h"
#include "CaloG4Sim/VEscapedEnergyProcessing.h"

#include "G4ios.hh"
#include "globals.hh"

#include <map>

namespace CaloG4
{
#ifdef G4MULTITHREADED
EscapedEnergyRegistry::EERThreadMap_t EscapedEnergyRegistry::m_EERThreadMap;
#endif

EscapedEnergyRegistry* EscapedEnergyRegistry::GetInstance()
  {
#ifdef G4MULTITHREADED
    auto eer = getEER();
    if (!eer) { //nullpointer if it is not found
      return setEER();
    }
    else {
      return eer;
    }
#else
    //Standard implementation of a Singleton Pattern
    static EscapedEnergyRegistry instance;
    return &instance;
#endif
  }

#ifdef G4MULTITHREADED
  EscapedEnergyRegistry* EscapedEnergyRegistry::getEER()
  {
   // Get current thread-ID
   const auto tid = std::this_thread::get_id();
   auto eerPair = m_EERThreadMap.find(tid);
   if(eerPair == m_EERThreadMap.end()) {
     return nullptr; //if not found return null pointer
   }
   else {
     return eerPair->second;
   }
  }

  EscapedEnergyRegistry* EscapedEnergyRegistry::setEER()
  {
    EscapedEnergyRegistry* instance = new EscapedEnergyRegistry;
    const auto tid = std::this_thread::get_id();
    auto inserted = m_EERThreadMap.insert( std::make_pair(tid, instance)).first;
    return (EscapedEnergyRegistry*) inserted->second;
  }
#endif

  void EscapedEnergyRegistry::AddAndAdoptProcessing( const G4String& name,
                                                     std::unique_ptr<VEscapedEnergyProcessing> process )
  {
    // Don't bother adding a null pointer.
    if ( process == nullptr ) return;

    // Check that we're not adding any duplicates.
    for (const auto& [key, storedProcess] : m_processingMap) {
      if ( name == key ) {
        G4cout << "CaloG4Sim::EscapedEnergyRegistry::AddAndAdoptProcessing -"
               << G4endl;
        G4cout << "   Trying to add a second VEscapedEnergyProcessing with the name '"
               << name
               << "'" << G4endl;
        G4cout << "   Entry is rejected!" << G4endl;
        return;
      }
      if ( process == storedProcess ) {
        G4cout << "CaloG4Sim::EscapedEnergyRegistry::AddAndAdoptProcessing -"
               << G4endl;
        G4cout << "   The key '"
               << name
               << "' has the same VEscapedEnergyProcessing object as the key '"
               << key
               << "'" << G4endl;
        G4cout << "   Entry is rejected!" << G4endl;
        return;
      }
    }

    // There are no duplicates, so add the entry.
    m_processingMap.emplace(name,std::move(process));
  }

  VEscapedEnergyProcessing*
  EscapedEnergyRegistry::GetProcessing( const G4String& volumeName ) const
  {
    // Search through the map.  If we find an entry whose text string
    // is a substring of the volume name (e.g., "LAr::" is a substring
    // of "LAr::BarrelCryostat::MotherVolume"), then return that
    // VEscapedEnergyProcessing object.

    // Reminder:
    // m_processingMap = consists of pair< G4String, VEscapedEnergyProcessing* >

    for (auto& [key, storedProcess] : m_processingMap) {
      if ( volumeName.contains(key) )
        return storedProcess.get();
    }

    // If we get here, then there was no entry in the map that
    // matched any portion of the volume name.

    return nullptr;
  }

} // namespace CaloG4
