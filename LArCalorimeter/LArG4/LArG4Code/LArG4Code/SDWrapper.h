/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4CODE_SDWRAPPER_H
#define LARG4CODE_SDWRAPPER_H

// System includes
#include <string>
#include <vector>
#include <memory>

// External includes
#include "G4VSensitiveDetector.hh"

// Framework includes
#include "StoreGate/WriteHandle.h"


// Forward declarations
class LArG4SimpleSD;
class LArHitContainer;
class LArG4CalibSD;
class CaloCalibrationHitContainer;


namespace LArG4
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
        SDWrapper(const std::string& name, const std::string& hitCollectionName, std::string deadHitCollectionName="");

        /// Add an SD to this wrapper
        void addSD(std::unique_ptr<SDType> sd);

        /// Add a (non-owned) fast-sim SD by name
        void addFastSimSD(const std::string& fastSimSDName);

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

        /// The hit container name
        std::string m_deadHitCollName;

        /// The hit container handle
        SG::WriteHandle<HitContainerType> m_deadHitColl;

        /// The list of sensitive detectors that I own and manage
        SDList_t m_sdList;

        /// A fastsim SD name that I do not own but invoke in order to
        /// share my hit container. For now I'm assuming there is only one.
        std::string m_fastSimSDName;

    }; // class SDWrapper

    template<>
    void SDWrapper<LArG4CalibSD, CaloCalibrationHitContainer>::EndOfAthenaEvent();

  } // namespace detail


  /// Template instantiation for LArG4SimpleSD
  using SimpleSDWrapper = detail::SDWrapper<LArG4SimpleSD, LArHitContainer>;

  /// Template instantiation for LArG4CalibSD
  using CalibSDWrapper = detail::SDWrapper<LArG4CalibSD, CaloCalibrationHitContainer>;

} // namespace LArG4

#endif
