/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMSD_SDWRAPPER_H
#define ISF_FASTCALOSIMSD_SDWRAPPER_H

// System includes
#include <string>
#include <vector>
#include <memory>

// External includes
#include "G4VSensitiveDetector.hh"

// Framework includes
#include "StoreGate/WriteHandle.h"


// Forward declarations
class FCS_StepInfoSD;
namespace ISF_FCS_Parametrization {
  class FCS_StepInfoCollection;
}
namespace FCS_Param
{

  namespace detail
  {

    /// @class SDWrapper
    /// @brief A template class which wraps multiple sensitive detectors.
    ///
    /// Allows for SD tools to manage several SDs which collaborate to fill one
    /// hit container in a multi-threading-friendly way. The wrapper owns the
    /// WriteHandle for the hit container and gathers hits from each SD at the
    /// end of an event.
    ///
    /// The inheritance from G4VSensitiveDetector is merely a trick so the SD
    /// tool can save this object in the SensitiveDetectorBase thread-local
    /// container. It also allows to create the hit container at the right time
    /// via the SD Initialize method invoked by Geant4.
    ///
    /// Clients shouldn't use this generic template directly, but should use
    /// the explicitly allowed specializations given below.
    ///
    /// @author Steve Farrell <Steven.Farrell@cern.ch>
    ///
    template<class SDType, class HitContainerType>
    class SDWrapper : public G4VSensitiveDetector
    {

      public:

        /// Alias to the SD list type
        using SDList_t = std::vector< std::unique_ptr<SDType> >;

        /// Construct the wrapper from the output collection name
        SDWrapper(const std::string& name, const std::string& hitCollectionName);

        /// Add an SD to this wrapper
        void addSD(std::unique_ptr<SDType> sd);

        /// Beginning of G4 event; initialize the hit collection.
        virtual void Initialize(G4HCofThisEvent*) override final;

        /// This method should not be called. It will throw.
        virtual bool ProcessHits(G4Step*, G4TouchableHistory*) override final;

        /// Gather the hits into the WriteHandle from all the SDs
        void EndOfAthenaEvent();

      private:

        /// The hit container name
        std::string m_hitCollName;

        /// The hit container handle
        SG::WriteHandle<HitContainerType> m_hitColl;

        /// The list of sensitive detectors that I own and manage
        SDList_t m_sdList;

    }; // class SDWrapper

  } // namespace detail


  /// Template instantiation for LArG4SimpleSD
  using FCS_StepInfoSDWrapper = detail::SDWrapper<FCS_StepInfoSD, ISF_FCS_Parametrization::FCS_StepInfoCollection>;

} // namespace FCS_Param

#endif
