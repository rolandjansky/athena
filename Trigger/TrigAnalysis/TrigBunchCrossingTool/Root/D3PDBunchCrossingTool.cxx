/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: D3PDBunchCrossingTool.cxx 618313 2014-09-24 10:05:46Z krasznaa $

// ROOT include(s):
#include "TTree.h"
#include "TChain.h"
#include "TBranch.h"

// Local include(s):
#include "TrigBunchCrossingTool/D3PDBunchCrossingTool.h"

namespace Trig {

   D3PDBunchCrossingTool::D3PDBunchCrossingTool( const std::string& name )
      : BunchCrossingToolBase( name ),
        m_configLoaded( false ), m_currentID( 0 ),
        m_validConfig( false ) {

   }

   /**
    * This function should be called whenever a new file is opened by the
    * analysis code. It reads in all the information from the metadata TTree
    * into memory in one go, so the users can access the metadata tree however
    * they want after the function is finished.
    *
    * @param metaTree Pointer to the metadata tree holding the bunch
    *                 configuration
    * @param clearCache If set to <code>true</code> (default value) then the
    *                   already known configurations are forgotten by the tool.
    *                   This is good practice in general to keep the memory
    *                   consumption of the tool low in a big job.
    * @returns <code>StatusCode::SUCCESS</code> if the caching was successful,
    *          or <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode D3PDBunchCrossingTool::cacheConfig( ::TTree* metaTree,
                                                  bool clearCache ) {

      // Do a very simple sanity check:
      if( ! metaTree ) {
         ATH_MSG_ERROR( "Function called with null-pointer!" );
         return StatusCode::FAILURE;
      }

      // Decide which version of the metadata we have available:
      if( metaTree->GetBranch( "ConfigID" ) &&
          metaTree->GetBranch( "FilledBunches" ) &&
          metaTree->GetBranch( "BunchIntensities" ) ) {

         ATH_CHECK( cacheConfigV1( metaTree, clearCache ) );
         return StatusCode::SUCCESS;

      } else if( metaTree->GetBranch( "ConfigID" ) &&
                 metaTree->GetBranch( "FilledBunches" ) &&
                 metaTree->GetBranch( "FilledBunchIntBeam1" ) &&
                 metaTree->GetBranch( "FilledBunchIntBeam2" ) &&
                 metaTree->GetBranch( "UnpairedBeam1" ) &&
                 metaTree->GetBranch( "UnpairedBeam2" ) &&
                 metaTree->GetBranch( "UnpairedIntBeam1" ) &&
                 metaTree->GetBranch( "UnpairedIntBeam2" ) ) {

         ATH_CHECK( cacheConfigV2( metaTree, clearCache ) );
         return StatusCode::SUCCESS;
      }

      // This is an unknown TTree format:
      ATH_MSG_ERROR( "Didn't recognize the format of the metadata tree" );
      return StatusCode::FAILURE;
   }

   /**
    * This function should be called whenever processing a new D3PD event. It
    * tries to be as smart as possible, to do as little as possible. So it
    * should be safe to call it in every event.
    *
    * You should yourself read the configuration ID from the event information
    * in the D3PD, and pass that to this function.
    *
    * Of course you should also not forget about caching the configurations of
    * the current file using <code>cacheConfig(...)</code>.
    *
    * @param configID The configuration ID of the current event
    * @returns <code>StatusCode::SUCCESS</code> if the loading of the
    *          configuration was successful, or
    *          <code>StatusCode::FAILURE</code> otherwise
    */
   StatusCode D3PDBunchCrossingTool::loadConfig( UInt_t configID ) {

      // Check whether we already have this configuration loaded:
      if( ( configID == m_currentID ) && m_configLoaded ) {
         ATH_MSG_VERBOSE( "Configuration key already loaded: " << configID );
         return StatusCode::SUCCESS;
      }

      // Search for this configuration in the cache:
      std::map< UInt_t, BunchConfig >::const_iterator conf_itr;
      if( ( conf_itr = m_cache.find( configID ) ) == m_cache.end() ) {
         ATH_MSG_ERROR( "Configuration ID " << configID << " not known!" );
         ATH_MSG_ERROR( "Did you call \"cacheConfig(...)\" on the new files?" );
         return StatusCode::FAILURE;
      }

      // Let the user know what we're doing:
      ATH_MSG_DEBUG( "Loading the configuration with key: " << configID );

      // Check if the cached information makes sense for the filled bunches:
      if( ( conf_itr->second.filledIntBeam1.size() &&
            ( conf_itr->second.filledBCIDs.size() !=
              conf_itr->second.filledIntBeam1.size() ) ) ||
          ( conf_itr->second.filledIntBeam2.size() &&
            ( conf_itr->second.filledBCIDs.size() !=
              conf_itr->second.filledIntBeam2.size() ) ) ) {
         ATH_MSG_WARNING( "Infortmation about the filled bunches is "
                          << "invalid for config ID: " << configID );
         m_configLoaded = true;
         m_validConfig = false;
         return StatusCode::SUCCESS;
      }

      // Check if the cached information makes sense for the unpaired bunches:
      if( ( conf_itr->second.unpairedIntBeam1.size() &&
            ( conf_itr->second.unpairedBCIDsBeam1.size() !=
              conf_itr->second.unpairedIntBeam1.size() ) ) ||
          ( conf_itr->second.unpairedIntBeam2.size() &&
            ( conf_itr->second.unpairedBCIDsBeam2.size() !=
              conf_itr->second.unpairedIntBeam2.size() ) ) ) {
         ATH_MSG_WARNING( "Infortmation about the unpaired bunches is "
                          << "invalid for config ID: " << configID );
         m_configLoaded = true;
         m_validConfig = false;
         return StatusCode::SUCCESS;
      }

      // Let the base class load this configuration:
      ATH_CHECK( loadSingleBunches( conf_itr->second.filledBCIDs,
                                    conf_itr->second.filledIntBeam1,
                                    conf_itr->second.filledIntBeam2 ) );
      ATH_CHECK( loadBunchTrains( conf_itr->second.filledBCIDs,
                                  conf_itr->second.filledIntBeam1,
                                  conf_itr->second.filledIntBeam2 ) );
      ATH_CHECK( loadUnpairedBunches( conf_itr->second.unpairedBCIDsBeam1,
                                      conf_itr->second.unpairedBCIDsBeam2,
                                      conf_itr->second.unpairedIntBeam1,
                                      conf_itr->second.unpairedIntBeam2 ) );

      // Remember what we just did:
      m_currentID = configID;
      m_validConfig = true;
      m_configLoaded = true;

      // Print the configuration to give some feedback to the user:
      ATH_MSG_INFO( "Loaded configuration key " << m_currentID );
      printConfig();

      // Return gracefully:
      return StatusCode::SUCCESS;
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
   bool D3PDBunchCrossingTool::isAvailable( UInt_t configID ) const {

      return ( m_cache.find( configID ) != m_cache.end() );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingTool::isFilled( bcid_type bcid ) const {

      if( ! m_validConfig ) return true;
      else return BunchCrossingToolBase::isFilled( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingTool::isInTrain( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isInTrain( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingTool::isUnpaired( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isUnpaired( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingTool::isBeam1( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isBeam1( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   bool D3PDBunchCrossingTool::isBeam2( bcid_type bcid ) const {

      if( ! m_validConfig ) return false;
      else return BunchCrossingToolBase::isBeam2( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   float D3PDBunchCrossingTool::bcIntensity( bcid_type bcid,
                                               BeamType type ) const {

      if( ! m_validConfig ) return 1.0;
      else return BunchCrossingToolBase::bcIntensity( bcid, type );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   IBunchCrossingTool::BunchCrossingType
   D3PDBunchCrossingTool::bcType( bcid_type bcid ) const {

      if( ! m_validConfig ) return Single;
      else return BunchCrossingToolBase::bcType( bcid );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   std::vector< bool >
   D3PDBunchCrossingTool::bunchesInFront( bcid_type bcid,
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
   D3PDBunchCrossingTool::bunchesAfter( bcid_type bcid,
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
   D3PDBunchCrossingTool::bunchIntInFront( bcid_type bcid,
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
   D3PDBunchCrossingTool::bunchIntAfter( bcid_type bcid,
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
   int D3PDBunchCrossingTool::gapBeforeBunch( bcid_type bcid,
                                              BunchDistanceType dtype,
                                              BunchFillType ftype ) const {

      if( ! m_validConfig ) return -1;
      else return BunchCrossingToolBase::gapBeforeBunch( bcid, dtype, ftype );
   }

   /**
    * Function re-implemented to give meaningful answers for non-pileup MC files
    * as well.
    */
   int D3PDBunchCrossingTool::gapAfterBunch( bcid_type bcid,
                                             BunchDistanceType dtype,
                                             BunchFillType ftype ) const {

      if( ! m_validConfig ) return -1;
      else return BunchCrossingToolBase::gapAfterBunch( bcid, dtype, ftype );
   }

   /**
    * In case the tool is not configured correctly (we're processing a
    * non-pileup MC file) it should not report 0 filled bunches, but 1. The
    * override of this function makes sure that this is what happens...
    */
   unsigned int D3PDBunchCrossingTool::numberOfFilledBunches() const {

      if( ! m_validConfig ) return 1;
      else return BunchCrossingToolBase::numberOfFilledBunches();
   }

   StatusCode D3PDBunchCrossingTool::cacheConfigV1( ::TTree* metaTree,
                                                    bool clearCache ) {

      // Do a very simple sanity check:
      if( ! metaTree ) {
         ATH_MSG_ERROR( "Function called with null-pointer!" );
         return StatusCode::FAILURE;
      }

      // Let the user know what we're doing:
      ATH_MSG_DEBUG( "Caching configuration..." );

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
         ATH_MSG_ERROR( "The supplied TTree doesn't seem to be a bunch "
                        "configuration metadata tree" );
         return StatusCode::FAILURE;
      }

      // Check if the specified TTree has he unpaired branches:
      bool hasUnpairedInfo = true;
      if( ( ! metaTree->GetBranch( "UnpairedBunches" ) ) ||
          ( ! metaTree->GetBranch( "UnpairedBunchIntensities" ) ) ) {
         ATH_MSG_DEBUG( "The supplied TTree doesn't seem to hold the unpaired "
                        << "bunch configuration branches" );
         hasUnpairedInfo = false;
      }

      // Connect all the required branches:
      TBranch* b_configID = 0;
      TBranch* b_confFilledBCIDs = 0;
      TBranch* b_confFilledIntensities = 0;
      if( metaTree->SetBranchAddress( "ConfigID", &configID, &b_configID ) ||
          metaTree->SetBranchAddress( "FilledBunches", &confFilledBCIDs,
                                      &b_confFilledBCIDs ) ||
          metaTree->SetBranchAddress( "BunchIntensities",
                                      &confFilledIntensities,
                                      &b_confFilledIntensities ) ) {
         ATH_MSG_ERROR( "Couldn't connect variables to the metadata tree "
                        "branches" );
         return StatusCode::FAILURE;
      }
      TBranch* b_confUnpairedBCIDs = 0;
      TBranch* b_confUnpairedIntensities = 0;
      if( hasUnpairedInfo ) {
         b_confUnpairedBCIDs = 0;
         b_confUnpairedIntensities = 0;
         if( metaTree->SetBranchAddress( "UnpairedBunches", &confUnpairedBCIDs,
                                         &b_confUnpairedBCIDs ) ||
             metaTree->SetBranchAddress( "UnpairedBunchIntensities", 
                                         &confUnpairedIntensities,
                                         &b_confUnpairedIntensities ) ) {
            ATH_MSG_ERROR( "Couldn't connect variables to the metadata tree "
                           "branches" );
            return StatusCode::FAILURE;
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

         ATH_MSG_VERBOSE( "Cached the configuration for config ID: "
                          << configID );
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

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

   StatusCode D3PDBunchCrossingTool::cacheConfigV2( ::TTree* metaTree,
                                                    bool clearCache ) {

      // Do a very simple sanity check:
      if( ! metaTree ) {
         ATH_MSG_ERROR( "Function called with null-pointer!" );
         return StatusCode::FAILURE;
      }

      // Let the user know what we're doing:
      ATH_MSG_DEBUG( "Caching configuration..." );

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

         ATH_MSG_ERROR( "The supplied TTree doesn't seem to be a bunch "
                        "configuration metadata tree" );
         return StatusCode::FAILURE;
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

         ATH_MSG_ERROR( "Couldn't connect variables to the metadata tree "
                        "branches" );
         return StatusCode::FAILURE;
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

         ATH_MSG_VERBOSE( "Cached the configuration for config ID: "
                          << configID );
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
   }

} // namespace Trig
