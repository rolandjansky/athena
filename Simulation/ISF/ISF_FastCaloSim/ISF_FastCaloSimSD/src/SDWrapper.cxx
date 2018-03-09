/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SDWrapper.h"

// Geant4 includes
#include "G4SDManager.hh"

// Framework utilities
#include "CxxUtils/make_unique.h"

// Project includes
#include "ISF_FastCaloSimEvent/FCS_StepInfoCollection.h"

// Local includes
#include "FCS_StepInfoSD.h"

namespace FCS_Param
{

  namespace detail
  {

    //-------------------------------------------------------------------------
    // Construct with a hit collection name
    //-------------------------------------------------------------------------
    template<class SDType, class HitContainerType>
    SDWrapper<SDType, HitContainerType>::
    SDWrapper(const std::string& name, const std::string& hitCollectionName)
      : G4VSensitiveDetector(name),
        m_hitCollName(hitCollectionName),
        m_hitColl(hitCollectionName)
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
        m_hitColl = CxxUtils::make_unique<HitContainerType>();
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
    }

    //-------------------------------------------------------------------------
    // Explit template instantiations
    //-------------------------------------------------------------------------
    template class SDWrapper<FCS_StepInfoSD, ISF_FCS_Parametrization::FCS_StepInfoCollection>;

  } // namespace detail

} // namespace FCS_Param
