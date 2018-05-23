/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArG4Code/SDWrapper.h"

// Geant4 includes
#include "G4SDManager.hh"

// Framework utilities
#include "CxxUtils/make_unique.h"

// Project includes
#include "LArSimEvent/LArHitContainer.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

// Local includes
#include "LArG4Code/LArG4SimpleSD.h"
#include "LArG4Code/LArG4CalibSD.h"

namespace LArG4
{

  namespace detail
  {

    //-------------------------------------------------------------------------
    // Construct with a hit collection name
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    SDWrapper<SDType, HitContainerType>::
    SDWrapper(const std::string& name, const std::string& hitCollectionName, std::string deadHitCollectionName)
      : G4VSensitiveDetector(name),
        m_hitCollName(hitCollectionName),
        m_hitColl(hitCollectionName),
        m_deadHitCollName(deadHitCollectionName),
        m_deadHitColl(deadHitCollectionName)
    {}

    //-------------------------------------------------------------------------
    // Add a unique SD to the list
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    void SDWrapper<SDType, HitContainerType>::
    addSD(std::unique_ptr<SDType> sd)
    {
      m_sdList.push_back( std::move(sd) );
    }

    //-------------------------------------------------------------------------
    // Add a fast-sim SD name
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    void SDWrapper<SDType, HitContainerType>::
    addFastSimSD(const std::string& sdName)
    {
      // For now, assume only one fastsim SD
      if(!m_fastSimSDName.empty()) {
        G4cerr << GetName() << " \tERROR\t" << "Trying to add fastsim SD "
               << sdName << " after " << m_fastSimSDName << G4endl;
        throw std::runtime_error("More than one fastsim SD");
      }
      m_fastSimSDName = sdName;
    }

    //-------------------------------------------------------------------------
    // Initialize the hit collection at the beginning of the G4 event
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    void SDWrapper<SDType, HitContainerType>::
    Initialize(G4HCofThisEvent*)
    {
      if(!m_hitColl.isValid()) {
        if(verboseLevel >= 5) {
          G4cout << GetName() << " \tDEBUG\t" << "Initializing hit container: "
                 << m_hitCollName << G4endl;
        }
        m_hitColl = CxxUtils::make_unique<HitContainerType>(m_hitCollName);
      }
      if(!m_deadHitCollName.empty() && !m_deadHitColl.isValid()) {
        if(verboseLevel >= 5) {
          G4cout << GetName() << " \tDEBUG\t" << "Initializing hit container: "
                 << m_deadHitCollName << G4endl;
        }
        m_deadHitColl = CxxUtils::make_unique<HitContainerType>(m_deadHitCollName);
      }
    }

    //-------------------------------------------------------------------------
    // This should not be called
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    bool SDWrapper<SDType, HitContainerType>::
    ProcessHits(G4Step*, G4TouchableHistory*)
    {
      G4ExceptionDescription description;
      description << "ProcessHits: this SD shouldn't be assigned to volumes!";
      G4Exception(GetName(), "SDError", FatalException, description);
      return false;
    }

    //-------------------------------------------------------------------------
    // Gather the hits into the WriteHandle from all the SDs
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    void SDWrapper<SDType, HitContainerType>::
    EndOfAthenaEvent()
    {
      if(!m_hitColl.isValid()) {
        G4cerr << GetName() << " \tERROR\t" << "Hit collection WriteHandle is "
               << "invalid!" << G4endl;
        throw std::runtime_error("Invalid hit container WriteHandle: " +
                                 m_hitColl.name());
      }
      // Loop over each SD and fill the container
      for(auto& sd : m_sdList) {
        sd->EndOfAthenaEvent( &*m_hitColl );
      }
      // Gather frozen shower hits
      if(!m_fastSimSDName.empty()) {
        auto fastSD = dynamic_cast<SDType*>
          ( G4SDManager::GetSDMpointer()->FindSensitiveDetector(m_fastSimSDName) );
        if(fastSD) {
          fastSD->EndOfAthenaEvent( &*m_hitColl );
        }
        else {
          G4cerr << GetName() << " \tERROR\t" << "Failed to retrieve/cast "
                 << m_fastSimSDName << G4endl;
          throw std::runtime_error("Failed to retrieve/cast " + m_fastSimSDName);
        }
      }
    }

    template<>
    void SDWrapper<LArG4CalibSD, CaloCalibrationHitContainer>::
    EndOfAthenaEvent()
    {
      if(!m_hitColl.isValid()) {
        G4cerr << GetName() << " \tERROR\t" << "Hit collection WriteHandle is "
               << "invalid!" << G4endl;
        throw std::runtime_error("Invalid hit container WriteHandle: " +
                                 m_hitColl.name());
      }
      CaloCalibrationHitContainer* deadHitCollPtr(nullptr);
      if(!m_deadHitCollName.empty()) {
        if(!m_deadHitColl.isValid()) {
          G4cerr << GetName() << " \tERROR\t" << "Dead Hit collection WriteHandle is "
               << "invalid!" << G4endl;
          throw std::runtime_error("Invalid hit container WriteHandle: " +
                                   m_deadHitColl.name());
        }
        deadHitCollPtr = &*m_deadHitColl;
      }
      // Loop over each SD and fill the container
      for(auto& sd : m_sdList) {
        sd->EndOfAthenaEvent( &*m_hitColl, deadHitCollPtr );
      }
      // Gather frozen shower hits
      if(!m_fastSimSDName.empty()) {
        auto fastSD = dynamic_cast<LArG4CalibSD*>
          ( G4SDManager::GetSDMpointer()->FindSensitiveDetector(m_fastSimSDName) );
        if(fastSD) {
          fastSD->EndOfAthenaEvent( &*m_hitColl, deadHitCollPtr );
        }
        else {
          G4cerr << GetName() << " \tERROR\t" << "Failed to retrieve/cast "
                 << m_fastSimSDName << G4endl;
          throw std::runtime_error("Failed to retrieve/cast " + m_fastSimSDName);
        }
      }
    }

    //-------------------------------------------------------------------------
    // Explit template instantiations
    //-------------------------------------------------------------------------
    template class SDWrapper<LArG4SimpleSD, LArHitContainer>;
    template class SDWrapper<LArG4CalibSD, CaloCalibrationHitContainer>;

  } // namespace detail

} // namespace LArG4
