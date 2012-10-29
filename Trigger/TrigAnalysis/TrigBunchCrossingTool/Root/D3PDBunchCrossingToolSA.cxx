/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D3PDBunchCrossingToolSA.cxx 520130 2012-10-04 07:14:13Z krasznaa $

// ROOT include(s):
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"

// Local include(s):
#include "TrigBunchCrossingTool/D3PDBunchCrossingToolSA.h"

namespace Trig {

   /**
    * You can either supply the constructor with a TTree pointer, or leave it at its
    * default 0 value. In the latter case the object is created without a loaded
    * configuration. This can be advantageous for instance when using the tool
    * as a member object of a TSelector.
    *
    * @param metaTree Pointer to the metadata tree holding the bunch configuration
    */
   D3PDBunchCrossingToolSA::D3PDBunchCrossingToolSA( ::TTree* metaTree )
      : BunchCrossingToolBase(), m_configLoaded( false ), m_currentID( 0 ),
        m_validConfig( false ) {

      // Set the proper message source:
      log().setSource( "D3PDBunchCrossingToolSA" );

      // If we're given a TTree, let's use it:
      if( metaTree ) cacheConfig( metaTree );
   }

   /**
    * This function should be called whenever a new file is opened by the analysis code.
    * It reads in all the information from the metadata TTree into memory in one go,
    * so the users can access the metadata tree however they want after the function
    * is finished.
    *
    * @param metaTree Pointer to the metadata tree holding the bunch configuration
    * @param clearCache If set to <code>true</code> (default value) then the already
    *        known configurations are forgotten by the tool. This is good practice
    *        in general to keep the memory consumption of the tool low in a big job.
    * @returns <code>true</code> if the caching was successful, or
    *          <code>false</code> otherwise
    */
   bool D3PDBunchCrossingToolSA::cacheConfig( ::TTree* metaTree, bool clearCache ) {

      // Do a very simple sanity check:
      if( ! metaTree ) {
         REPORT_ERROR_MSG( "Function called with null-pointer!" );
         return false;
      }

      // Decide which version of the metadata we have available:
      if( metaTree->GetBranch( "ConfigID" ) &&
          metaTree->GetBranch( "FilledBunches" ) &&
          metaTree->GetBranch( "BunchIntensities" ) ) {

         return cacheConfigV1( metaTree, clearCache );

      } else if( metaTree->GetBranch( "ConfigID" ) &&
                 metaTree->GetBranch( "FilledBunches" ) &&
                 metaTree->GetBranch( "FilledBunchIntBeam1" ) &&
                 metaTree->GetBranch( "FilledBunchIntBeam2" ) &&
                 metaTree->GetBranch( "UnpairedBeam1" ) &&
                 metaTree->GetBranch( "UnpairedBeam2" ) &&
                 metaTree->GetBranch( "UnpairedIntBeam1" ) &&
                 metaTree->GetBranch( "UnpairedIntBeam2" ) ) {

         return cacheConfigV2( metaTree, clearCache );
      }

      // This is an unknown TTree format:
      REPORT_ERROR_MSG( "Didn't recognize the format of the metadata tree" );
      return false;
   }

   /**
    * This function should be called whenever processing a new D3PD event. It tries to
    * be as smart as possible, to do as little as possible. So it should be safe to call
    * it in every event.
    *
    * You should yourself read the configuration ID from the event information in the
    * D3PD, and pass that to this function.
    *
    * Of course you should also not forget about caching the configurations of the
    * current file using <code>cacheConfig(...)</code>.
    *
    * @param configID The configuration ID of the current event
    * @returns <code>true</code> if the loading of the configuration was successful, or
    *          <code>false</code> otherwise
    */
   bool D3PDBunchCrossingToolSA::loadConfig( UInt_t configID ) {

      // Check whether we already have this configuration loaded:
      if( ( configID == m_currentID ) && m_configLoaded ) {
         REPORT_VERBOSE_MSG( "Configuration key already loaded: " << configID );
         return true;
      }

      // Search for this configuration in the cache:
      std::map< UInt_t, BunchConfig >::const_iterator conf_itr;
      if( ( conf_itr = m_cache.find( configID ) ) == m_cache.end() ) {
         REPORT_ERROR_MSG( "Configuration ID " << configID << " not known!" );
         REPORT_ERROR_MSG( "Did you call \"cacheConfig(...)\" on the new files?" );
         return false;
      }

      // Let the user know what we're doing:
      log() << DEBUG << "Loading the configuration with key: " << configID
            << MsgLogger::endmsg;

      // Check if the cached information makes sense for the filled bunches:
      if( ( conf_itr->second.filledIntBeam1.size() &&
            ( conf_itr->second.filledBCIDs.size() !=
              conf_itr->second.filledIntBeam1.size() ) ) ||
          ( conf_itr->second.filledIntBeam2.size() &&
            ( conf_itr->second.filledBCIDs.size() !=
              conf_itr->second.filledIntBeam2.size() ) ) ) {
         log() << WARNING << "Infortmation about the filled bunches is "
               << "invalid for config ID: " << configID << MsgLogger::endmsg;
         m_configLoaded = true;
         m_validConfig = false;
         return true;
      }

      // Check if the cached information makes sense for the unpaired bunches:
      if( ( conf_itr->second.unpairedIntBeam1.size() &&
            ( conf_itr->second.unpairedBCIDsBeam1.size() !=
              conf_itr->second.unpairedIntBeam1.size() ) ) ||
          ( conf_itr->second.unpairedIntBeam2.size() &&
            ( conf_itr->second.unpairedBCIDsBeam2.size() !=
              conf_itr->second.unpairedIntBeam2.size() ) ) ) {
         log() << WARNING << "Infortmation about the unpaired bunches is "
               << "invalid for config ID: " << configID << MsgLogger::endmsg;
         m_configLoaded = true;
         m_validConfig = false;
         return true;
      }

      // Let the base class load this configuration:
      if( ! loadSingleBunches( conf_itr->second.filledBCIDs,
                               conf_itr->second.filledIntBeam1,
                               conf_itr->second.filledIntBeam2 ) ) {
         REPORT_ERROR_MSG( "Failed to interpret single bunches with key: "
                           << configID );
         return false;
      }
      if( ! loadBunchTrains( conf_itr->second.filledBCIDs,
                             conf_itr->second.filledIntBeam1,
                             conf_itr->second.filledIntBeam2 ) ) {
         REPORT_ERROR_MSG( "Failed to interpret bunch trains with key: "
                           << configID );
         return false;
      }
      if( ! loadUnpairedBunches( conf_itr->second.unpairedBCIDsBeam1,
                                 conf_itr->second.unpairedBCIDsBeam2,
                                 conf_itr->second.unpairedIntBeam1,
                                 conf_itr->second.unpairedIntBeam2 ) ) {
         REPORT_ERROR_MSG( "Failed to interpret unpaired bunches with key: "
                           << configID );
         return false;
      }
      // Remember what we just did:
      m_currentID = configID;
      m_validConfig = true;
      m_configLoaded = true;

      // Print the configuration to give some feedback to the user:
      log() << INFO << "Loaded configuration key " << m_currentID << MsgLogger::endmsg;
      printConfig();

      return true;
   }

   /**
    * This function can be used to figure out if a given configuration is
    * available to the tool currently. It is mostly used by the Athena version
    * of the tool to decide when to read in new metadata.
    *
    * @param configID The configuration ID that is to be checked
    * @returns <code>true</code> if the configuration is available,
    *          <code>false</code> if it's not
    */
   bool D3PDBunchCrossingToolSA::isAvailable( UInt_t configID ) const {

      return ( m_cache.find( configID ) != m_cache.end() );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingToolSA::isFilled( bcid_type bcid ) const {

      if( ! m_validConfig ) return true;
      else return BunchCrossingToolBase::isFilled( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingToolSA::isInTrain( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isInTrain( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingToolSA::isUnpaired( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isUnpaired( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingToolSA::isBeam1( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isBeam1( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingToolSA::isBeam2( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isBeam2( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   float D3PDBunchCrossingToolSA::bcIntensity( bcid_type bcid,
                                               BeamType type ) const {

      if( ! m_validConfig ) return 1.0;
      else return BunchCrossingToolBase::bcIntensity( bcid, type );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   IIBunchCrossingTool::BunchCrossingType
   D3PDBunchCrossingToolSA::bcType( bcid_type bcid ) const {

      if( ! m_validConfig ) return Single;
      else return BunchCrossingToolBase::bcType( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< bool >
   D3PDBunchCrossingToolSA::bunchesInFront( bcid_type bcid,
                                            int bunches ) const {

      if( ! m_validConfig ) {
         std::vector< bool > result;
         result.push_back( true );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( false );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchesInFront( bcid, bunches );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< bool >
   D3PDBunchCrossingToolSA::bunchesAfter( bcid_type bcid,
                                          int bunches ) const {

      if( ! m_validConfig ) {
         std::vector< bool > result;
         result.push_back( true );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( false );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchesAfter( bcid, bunches );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< float >
   D3PDBunchCrossingToolSA::bunchIntInFront( bcid_type bcid,
                                             int bunches,
                                             BeamType type ) const {

      if( ! m_validConfig ) {
         std::vector< float > result;
         result.push_back( 1.0 );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( 0.0 );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchIntInFront( bcid, bunches, type );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< float >
   D3PDBunchCrossingToolSA::bunchIntAfter( bcid_type bcid,
                                           int bunches,
                                           BeamType type ) const {

      if( ! m_validConfig ) {
         std::vector< float > result;
         result.push_back( 1.0 );
         for( int i = 0; i < bunches - 1; ++i ) {
            result.push_back( 0.0 );
         }
         return result;
      } else {
         return BunchCrossingToolBase::bunchIntAfter( bcid, bunches, type );
      }
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   int D3PDBunchCrossingToolSA::gapBeforeBunch( bcid_type bcid,
                                                BunchDistanceType dtype,
                                                BunchFillType ftype ) const {

      if( ! m_validConfig ) return -1;
      else return BunchCrossingToolBase::gapBeforeBunch( bcid, dtype, ftype );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   int D3PDBunchCrossingToolSA::gapAfterBunch( bcid_type bcid,
                                               BunchDistanceType dtype,
                                               BunchFillType ftype ) const {

      if( ! m_validConfig ) return -1;
      else return BunchCrossingToolBase::gapAfterBunch( bcid, dtype, ftype );
   }

   /**
    * In case the tool is not configured correctly (we're processing a non-pileup MC file) it
    * should not report 0 filled bunches, but 1. The override of this function makes
    * sure that this is what happens...
    */
   unsigned int D3PDBunchCrossingToolSA::numberOfFilledBunches() const {

      if( ! m_validConfig ) return 1;
      else return BunchCrossingToolBase::numberOfFilledBunches();
   }

   void D3PDBunchCrossingToolSA::setMaxBunchSpacing( int spacing ) {

      BunchCrossingToolBase::setMaxBunchSpacing( spacing );
      return;
   }

   int D3PDBunchCrossingToolSA::maxBunchSpacing() const {

      return BunchCrossingToolBase::maxBunchSpacing();
   }

   void D3PDBunchCrossingToolSA::setFrontLength( int length ) {

      BunchCrossingToolBase::setFrontLength( length );
      return;
   }

   int D3PDBunchCrossingToolSA::frontLength() const {

      return BunchCrossingToolBase::frontLength();
   }

   void D3PDBunchCrossingToolSA::setTailLength( int length ) {

      BunchCrossingToolBase::setTailLength( length );
      return;
   }

   int D3PDBunchCrossingToolSA::tailLength() const {

      return BunchCrossingToolBase::tailLength();
   }

   bool D3PDBunchCrossingToolSA::cacheConfigV1( ::TTree* metaTree,
                                                bool clearCache ) {

      // Do a very simple sanity check:
      if( ! metaTree ) {
         REPORT_ERROR_MSG( "Function called with null-pointer!" );
         return false;
      }

      // Let the user know what we're doing:
      log() << DEBUG << "Caching configuration..." << MsgLogger::endmsg;

      // The variables that should be connected to the branches:
      UInt_t                configID;
      std::vector< int >*   confFilledBCIDs = 0;
      std::vector< float >* confFilledIntensities = 0;
      std::vector< int >*   confUnpairedBCIDs = 0;
      std::vector< float >* confUnpairedIntensities = 0;

      // Check if the specified TTree has all the needed branches:
      if( ( ! metaTree->GetBranch( "ConfigID" ) ) ||
          ( ! metaTree->GetBranch( "FilledBunches" ) ) ||
          ( ! metaTree->GetBranch( "BunchIntensities" ) ) ) {
         REPORT_ERROR_MSG( "The supplied TTree doesn't seem to be a bunch "
                           "configuration metadata tree" );
         return false;
      }

      // Check if the specified TTree has he unpaired branches:
      bool hasUnpairedInfo = true;
      if( ( ! metaTree->GetBranch( "UnpairedBunches" ) ) ||
          ( ! metaTree->GetBranch( "UnpairedBunchIntensities" ) ) ) {
         log() << DEBUG << "The supplied TTree doesn't seem to hold the unpaired bunch "
               << "configuration branches" << MsgLogger::endmsg;
         hasUnpairedInfo = false;
      }

      // Connect all the required branches:
      TBranch* b_configID = 0;
      TBranch* b_confFilledBCIDs = 0;
      TBranch* b_confFilledIntensities = 0;
      if( metaTree->SetBranchAddress( "ConfigID", &configID, &b_configID ) ||
          metaTree->SetBranchAddress( "FilledBunches", &confFilledBCIDs,
                                      &b_confFilledBCIDs ) ||
          metaTree->SetBranchAddress( "BunchIntensities", &confFilledIntensities,
                                      &b_confFilledIntensities ) ) {
         REPORT_ERROR_MSG( "Couldn't connect variables to the metadata tree branches" );
         return false;
      }
      TBranch* b_confUnpairedBCIDs = 0;
      TBranch* b_confUnpairedIntensities = 0;
      if( hasUnpairedInfo ) {
         b_confUnpairedBCIDs = 0;
         b_confUnpairedIntensities = 0;
         if( metaTree->SetBranchAddress( "UnpairedBunches", &confUnpairedBCIDs,
                                         &b_confUnpairedBCIDs ) ||
             metaTree->SetBranchAddress( "UnpairedBunchIntensities", &confUnpairedIntensities,
                                         &b_confUnpairedIntensities ) ) {
            REPORT_ERROR_MSG( "Couldn't connect variables to the metadata tree branches" );
            return false;
         }
      }

      // Clear the cache:
      if( clearCache ) {
         m_cache.clear();
         m_configLoaded = false;
         m_currentID = 0;
         m_validConfig = false;
      }

      // Decide whether the TTree is actually a TChain:
      TChain* metaChain = dynamic_cast< TChain* >( metaTree );

      // Cache all the configurations:
      for( Int_t i = 0; i < metaTree->GetEntries(); ++i ) {

         // The entry to load:
         Long64_t entry = i;
         if( metaChain ) {
            entry = metaChain->LoadTree( i );
         }

         // Load the configuration ID:
         b_configID->GetEntry( entry );

         // Skip already known configurations (can happen in case of
         // using merged files or TChains):
         if( m_cache.find( configID ) != m_cache.end() ) {
            continue;
         }

         // Load the rest of the entry:
         b_confFilledBCIDs->GetEntry( entry );
         b_confFilledIntensities->GetEntry( entry );
         if( hasUnpairedInfo ) {
            b_confUnpairedBCIDs->GetEntry( entry );
            b_confUnpairedIntensities->GetEntry( entry );
         }

         // Create the configuration:
         BunchConfig config;
         config.filledBCIDs = *confFilledBCIDs;
         config.filledIntBeam1 = *confFilledIntensities;
         if( hasUnpairedInfo ) {
            config.unpairedBCIDsBeam1 = *confUnpairedBCIDs;
            config.unpairedBCIDsBeam2 = *confUnpairedBCIDs;
            config.unpairedIntBeam1 = *confUnpairedIntensities;
         }

         // Now remember it:
         m_cache[ configID ] = config;

         REPORT_VERBOSE_MSG( "Cached the configuration for config ID: " << configID );
      }

      // Disconnect and delete everything:
      b_configID->SetAddress( 0 );
      b_confFilledBCIDs->SetAddress( 0 );
      b_confFilledIntensities->SetAddress( 0 );
      delete confFilledBCIDs; delete confFilledIntensities;
      if( hasUnpairedInfo ) {
         b_confUnpairedBCIDs->SetAddress( 0 );
         b_confUnpairedIntensities->SetAddress( 0 );
         delete confUnpairedBCIDs; delete confUnpairedIntensities;
      }

      return true;
   }

   bool D3PDBunchCrossingToolSA::cacheConfigV2( ::TTree* metaTree,
                                                bool clearCache ) {

      // Do a very simple sanity check:
      if( ! metaTree ) {
         REPORT_ERROR_MSG( "Function called with null-pointer!" );
         return false;
      }

      // Let the user know what we're doing:
      log() << DEBUG << "Caching configuration..." << MsgLogger::endmsg;

      // The variables that should be connected to the branches:
      UInt_t                configID;
      std::vector< int >*   confFilledBCIDs = 0;
      std::vector< float >* confFilledIntBeam1 = 0;
      std::vector< float >* confFilledIntBeam2 = 0;
      std::vector< int >*   confUnpairedBeam1 = 0;
      std::vector< int >*   confUnpairedBeam2 = 0;
      std::vector< float >* confUnpairedIntBeam1 = 0;
      std::vector< float >* confUnpairedIntBeam2 = 0;

      ::TBranch* b_configID = 0;
      ::TBranch* b_confFilledBCIDs = 0;
      ::TBranch* b_confFilledIntBeam1 = 0;
      ::TBranch* b_confFilledIntBeam2 = 0;
      ::TBranch* b_confUnpairedBeam1 = 0;
      ::TBranch* b_confUnpairedBeam2 = 0;
      ::TBranch* b_confUnpairedIntBeam1 = 0;
      ::TBranch* b_confUnpairedIntBeam2 = 0;

      // Check that all branches exist:
      if( ( ! metaTree->GetBranch( "ConfigID" ) ) ||
          ( ! metaTree->GetBranch( "FilledBunches" ) ) ||
          ( ! metaTree->GetBranch( "FilledBunchIntBeam1" ) ) ||
          ( ! metaTree->GetBranch( "FilledBunchIntBeam2" ) ) ||
          ( ! metaTree->GetBranch( "UnpairedBeam1" ) ) ||
          ( ! metaTree->GetBranch( "UnpairedBeam2" ) ) ||
          ( ! metaTree->GetBranch( "UnpairedIntBeam1" ) ) ||
          ( ! metaTree->GetBranch( "UnpairedIntBeam2" ) ) ) {

         REPORT_ERROR_MSG( "The supplied TTree doesn't seem to be a bunch "
                           "configuration metadata tree" );
         return false;
      }

      // Connect all the branches:
      if( metaTree->SetBranchAddress( "ConfigID", &configID, &b_configID ) ||
          metaTree->SetBranchAddress( "FilledBunches", &confFilledBCIDs,
                                      &b_confFilledBCIDs ) ||
          metaTree->SetBranchAddress( "FilledBunchIntBeam1",
                                      &confFilledIntBeam1,
                                      &b_confFilledIntBeam1 ) ||
          metaTree->SetBranchAddress( "FilledBunchIntBeam2",
                                      &confFilledIntBeam2,
                                      &b_confFilledIntBeam2 ) ||
          metaTree->SetBranchAddress( "UnpairedBeam1",
                                      &confUnpairedBeam1,
                                      &b_confUnpairedBeam1 ) ||
          metaTree->SetBranchAddress( "UnpairedBeam2",
                                      &confUnpairedBeam2,
                                      &b_confUnpairedBeam2 ) ||
          metaTree->SetBranchAddress( "UnpairedIntBeam1",
                                      &confUnpairedIntBeam1,
                                      &b_confUnpairedIntBeam1 ) ||
          metaTree->SetBranchAddress( "UnpairedIntBeam2",
                                      &confUnpairedIntBeam2,
                                      &b_confUnpairedIntBeam2 ) ) {

         REPORT_ERROR_MSG( "Couldn't connect variables to the metadata tree branches" );
         return false;
      }

      // Clear the cache:
      if( clearCache ) {
         m_cache.clear();
         m_configLoaded = false;
         m_currentID = 0;
         m_validConfig = false;
      }

      // Decide whether the TTree is actually a TChain:
      TChain* metaChain = dynamic_cast< TChain* >( metaTree );

      // Cache all the configurations:
      for( Int_t i = 0; i < metaTree->GetEntries(); ++i ) {

         // The entry to load:
         Long64_t entry = i;
         if( metaChain ) {
            entry = metaChain->LoadTree( i );
         }

         // Load the configuration ID:
         b_configID->GetEntry( entry );

         // Skip already known configurations (can happen in case of
         // using merged files or TChains):
         if( m_cache.find( configID ) != m_cache.end() ) {
            continue;
         }

         // Load the rest of the entry:
         b_confFilledBCIDs->GetEntry( entry );
         b_confFilledIntBeam1->GetEntry( entry );
         b_confFilledIntBeam2->GetEntry( entry );
         b_confUnpairedBeam1->GetEntry( entry );
         b_confUnpairedBeam2->GetEntry( entry );
         b_confUnpairedIntBeam1->GetEntry( entry );
         b_confUnpairedIntBeam2->GetEntry( entry );

         // Create the configuration:
         BunchConfig config;
         config.filledBCIDs = *confFilledBCIDs;
         config.filledIntBeam1 = *confFilledIntBeam1;
         config.filledIntBeam2 = *confFilledIntBeam2;
         config.unpairedBCIDsBeam1 = *confUnpairedBeam1;
         config.unpairedBCIDsBeam2 = *confUnpairedBeam2;
         config.unpairedIntBeam1 = *confUnpairedIntBeam1;
         config.unpairedIntBeam2 = *confUnpairedIntBeam2;

         // Now remember it:
         m_cache[ configID ] = config;

         REPORT_VERBOSE_MSG( "Cached the configuration for config ID: " << configID );
      }

      return true;
   }

} // namespace Trig
