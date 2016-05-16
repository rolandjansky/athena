/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfigSvcD3PD.cxx 747438 2016-05-16 09:56:25Z tamartin $

// ROOT include(s):
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>

// Local include(s):
#include "../TrigRootAnalysis/TrigConfigSvcD3PD.h"
#include "../TrigRootAnalysis/VarHandle.h"

// STL include(s):
#include <limits.h>
#include <utility>

ClassImp( D3PD::TrigConfigSvcD3PD )

namespace D3PD {

   /**
    * The class allows itself to be instantiated without a proper configuration tree.
    * This is supposed to allow one to create the configuration object only once, and
    * then use it to read the configuration from all the files which are opened in
    * the analysis job.
    *
    * @param confTree Pointer to a trigger configuration metadata TTree
    */
   TrigConfigSvcD3PD::TrigConfigSvcD3PD( ::TTree* confTree )
      : TNamed( "TrigConfigSvcD3PD", "Trigger configuration service" ),
        m_initialized( kFALSE ), m_key( MakeDBKeys( 0, 0, 0 ) ),
        m_dummy( "" ), m_dummyVec() {

      if( confTree ) Initialize( confTree );
   }

   /**
    * This function can be used to set the object to use a new trigger configuration
    * metadata TTree. If you give it a null-pointer, the object resets itself, and lets
    * go of the previous TTree without attaching itself to a new TTree yet.
    *
    * Note that unfortunately we can't just check if the passed TTree pointer
    * is the same as the cached one. ROOT has the habit of putting the same sort
    * of TTree into the same memory space when the second file is opened for instance.
    * I've seen some weird problems because of this.
    *
    * @param confTree Pointer to a trigger configuration metadata TTree
    * @returns <code>kTRUE</code> if the operation was successful,
    *          <code>kFALSE</code> otherwise
    */
   ::Bool_t TrigConfigSvcD3PD::SetConfigTree( ::TTree* confTree ) {

      //
      // If the user gave a null-pointer, reset the object and return gracefully:
      //
      if( ! confTree ) {
         Reset();
         return kTRUE;
      }

      //
      // Reset the object and start using a new TTree:
      //
      Reset();
      return Initialize( confTree );
   }

   /**
    * This function can be used to load a new particular configuration in memory.
    * The object can of course only load configurations which exist in the input
    * file, and complains if the user wants to load something non-available.
    *
    * The function is fairly quick, so it can in principle be used in every event
    * in an event loop.
    *
    * @param SMK Super Master Key
    * @param L1PSK LVL1 Prescale Key
    * @param HLTPSK HLT Prescale Key
    * @param silent Set to <code>kTRUE</code> if you don't want info messages
    *               from the function
    * @returns <code>kTRUE</code> if the operation was successful,
    *          <code>kFALSE</code> otherwise
    */
   ::Bool_t TrigConfigSvcD3PD::Load( ::Int_t SMK, ::Int_t L1PSK, ::Int_t HLTPSK,
                                     ::Bool_t silent ) {

      //
      // Make quick work if the correct entry is already loaded:
      //
      if( m_key == MakeDBKeys( SMK, L1PSK, HLTPSK ) ) {
         return kTRUE;
      }

      //
      // Find which entry corresponds to this configuration:
      //
      DBKeys_t tempkey = MakeDBKeys( SMK, L1PSK, HLTPSK );
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator itr;
      if( ( itr = m_lvl1NameMap.find( tempkey ) ) == m_lvl1NameMap.end() ) {
         if( ! silent ) {
            Error( "Load", "Configuration with the following keys not available:" );
            Error( "Load", "  SMK: %i, L1PSK: %i, HLTPSK: %i",
                   SMK, L1PSK, HLTPSK );
            Error( "Load", "Available configurations are:" );
            std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
               db_itr = m_lvl1NameMap.begin();
            std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
               db_end = m_lvl1NameMap.end();
            for( ; db_itr != db_end; ++db_itr ) {
               Error( "Load", "  SMK: %i, L1PSK: %i, HLTPSK: %i",
                      db_itr->first.first, db_itr->first.second.first,
                      db_itr->first.second.second );
            }
         }
         return kFALSE;
      }

      m_key = tempkey;

      if( ! silent ) {
         Info( "Load", "Loaded configuration: SMK: %i, L1PSK: %i, HLTPSK: %i",
               SMK, L1PSK, HLTPSK );
      }
      return kTRUE;
   }

   /**
    * This function checks if a given configuration is known to the object
    * at the moment. It can be very useful in determining whether the metadata
    * needs to be read from a newly open file or not.
    *
    * @param SMK Super Master Key
    * @param L1PSK LVL1 Prescale Key
    * @param HLTPSK HLT Prescale Key
    * @returns <code>kTRUE</code> if the configuration is available,
    *          <code>kFALSE</code> if it isn't
    */
   ::Bool_t TrigConfigSvcD3PD::IsAvailable( ::Int_t SMK, ::Int_t L1PSK,
                                            ::Int_t HLTPSK ) const {

      if( m_lvl1NameMap.find( MakeDBKeys( SMK, L1PSK, HLTPSK ) ) !=
          m_lvl1NameMap.end() ) {
         return kTRUE;
      } else {
         return kFALSE;
      }
   }

   /**
    * This function checks whether a particular LVL1 item or HLT chain is available
    * in the currently loaded configuration.
    *
    * @param name Name of the LVL1 item or HLT chain
    * @returns <code>kTRUE</code> if the trigger is available in the configuration,
    *          <code>kFALSE</code> otherwise
    */
   ::Bool_t TrigConfigSvcD3PD::IsConfigured( const std::string& name ) const {

      // Do a small sanity check:
      if( ! m_initialized ) {
         Error( "IsConfigured", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Try to find it as an HLT chain (I guess this is a more common query):
      //
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         hltMap = m_hltNameMap.find( m_key );
      if( hltMap == m_hltNameMap.end() ) {
         Error( "IsConfigured", "Current configuration not available!" );
         return kFALSE;
      }
      if( hltMap->second.find( name ) != hltMap->second.end() ) {
         return kTRUE;
      }

      //
      // Now try to find it as a LVL1 item:
      //
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         lvl1Map = m_lvl1NameMap.find( m_key );
      if( lvl1Map == m_lvl1NameMap.end() ) {
         Error( "IsConfigured", "Current configuration not available!" );
         return kFALSE;
      }
      if( lvl1Map->second.find( name ) != lvl1Map->second.end() ) {
         return kTRUE;
      }

      // If it hasn't been found, then it wasn't configured:
      return kFALSE;
   }

   /**
    * This function can return the CTP ID of a LVL1 item. The CTP ID is an ID from 0
    * to 255 that specifies which bit in the CTP result shows the result of the
    * particular LVL1 item.
    *
    * @param name Name of the LVL1 item
    * @returns The CTP ID belonging to the specified LVL1 item
    */
   ::Int_t TrigConfigSvcD3PD::GetCTPId( const std::string& name ) const {

      // Do a small sanity check:
      if( ! m_initialized ) {
         Error( "GetCTPId", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Find the ID of the requested item:
      //
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         lvl1Map = m_lvl1NameMap.find( m_key );
      if( lvl1Map == m_lvl1NameMap.end() ) {
         Error( "GetCTPId", "Current configuration not available!" );
         return -1;
      }
      std::map< std::string, int >::const_iterator itr;
      if( ( itr = lvl1Map->second.find( name ) ) == lvl1Map->second.end() ) {
         Warning( "GetCTPId",  "LVL1 item not known: %s",
                  name.c_str() );
         return -1;
      }

      return itr->second;
   }

   /**
    * This function can return the chain ID of an HLT chain. The chain ID is an ID from 0
    * to 8192, specifying which bit in the HLT result shows the result of the particular
    * HLT chain.
    *
    * @param name Name of the HLT chain
    * @returns The chain ID belonging to the specified HLT chain
    */
   ::Int_t TrigConfigSvcD3PD::GetChainId( const std::string& name ) const {

      // Do a small sanity check:
      if( ! m_initialized ) {
         Error( "GetChainId", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Find the ID of the requested chain:
      //
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         hltMap = m_hltNameMap.find( m_key );
      if( hltMap == m_hltNameMap.end() ) {
         Error( "GetChainId", "Current configuration not available!" );
         return -1;
      }
      std::map< std::string, int >::const_iterator itr;
      if( ( itr = hltMap->second.find( name ) ) == hltMap->second.end() ) {
         Warning( "GetChainId", "HLT chain not known: %s",
                  name.c_str() );
         return -1;
      }

      return itr->second;
   }

   /**
    * This function can return the prescale value of a LVL1 item or HLT chain.
    * Note that the function doesn't return the cumulative prescale! So for an EF trigger
    * chain it only returns the prescale value that was used at the EF level, not
    * taking the LVL1 and LVL2 prescales into account.
    *
    * @param name Name of the LVL1 item or HLT chain name
    * @returns The prescale value of the specified trigger
    */
   ::Float_t TrigConfigSvcD3PD::GetPrescale( const std::string& name ) const {

      // Do a small sanity check:
      if( ! m_initialized ) {
         Error( "GetPrescale", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Check if this is an HLT chain:
      //
      std::map< DBKeys_t, std::map< std::string, float > >::const_iterator
         psMap = m_hltPrescaleMap.find( m_key );
      if( psMap == m_hltPrescaleMap.end() ) {
         Error( "GetPrescale", "Current configuration not available!" );
         return -1.0;
      }
      std::map< std::string, float >::const_iterator itr;
      if( ( itr = psMap->second.find( name ) ) != psMap->second.end() ) {
         return itr->second;
      }

      //
      // If not, then it should be a LVL1 item:
      //
      psMap = m_lvl1PrescaleMap.find( m_key );
      if( psMap == m_lvl1PrescaleMap.end() ) {
         Error( "GetPrescale", "Current configuration not available!" );
         return -1.0;
      }
      if( ( itr = psMap->second.find( name ) ) != psMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetPrescale", "Trigger not known: %s",
               name.c_str() );
      return -1.0;
   }

   /**
    * This function can return the rerun prescale of a given HLT chain. This is
    * usually only needed for in-depth trigger analyses.
    *
    * @param name The name of the HLT chain
    * @returns The rerun prescale value belonging to the specified HLT chain
    */
   ::Float_t TrigConfigSvcD3PD::GetRerunPrescale( const std::string& name ) const {

      //
      // Check if this information is available:
      //
      if( ! m_initialized ) {
         Error( "GetRerunPrescale", "Configuration meta-tree not initialized yet!" );
         return -1.0;
      }

      //
      // Find the rerun prescale of the requested chain:
      //
      std::map< DBKeys_t, std::map< std::string, float > >::const_iterator
         psMap = m_hltRerunPrescaleMap.find( m_key );
      if( psMap == m_hltRerunPrescaleMap.end() ) {
         Warning( "GetRerunPrescale", "The requested information is not available" );
         return -1.0;
      }
      std::map< std::string, float >::const_iterator itr;
      if( ( itr = psMap->second.find( name ) ) != psMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetRerunPrescale", "HLT chain now known: %s",
               name.c_str() );
      return -1.0;
   }

   /**
    * This function can return the passthrough value set for a given HLT chain.
    * This is usually only needed for in-depth trigger analyses.
    *
    * @param name The name of the HLT chain
    * @returns The passthrough value belonging to the specified HLT chain
    */
   ::Float_t TrigConfigSvcD3PD::GetPassthrough( const std::string& name ) const {

      //
      // Check if this information is available:
      //
      if( ! m_initialized ) {
         Error( "GetPassthrough", "Configuration meta-tree not initialized yet!" );
         return -1.0;
      }

      //
      // Find the passthrough value of the requested chain:
      //
      std::map< DBKeys_t, std::map< std::string, float > >::const_iterator
         ptMap = m_hltPassthroughMap.find( m_key );
      if( ptMap == m_hltPassthroughMap.end() ) {
         Warning( "GetPassthrough", "The requested information is not available" );
         return -1.0;
      }
      std::map< std::string, float >::const_iterator itr;
      if( ( itr = ptMap->second.find( name ) ) != ptMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetPassthrough", "HLT chain now known: %s",
               name.c_str() );
      return -1.0;
   }

   /**
    * This function can be used to find the name of the HLT chain / LVL1 item that seeds
    * another HLT chain. For instance "EF_mu20" is usually seeded by "L2_mu20".
    *
    * Note: This information may not be available in some older D3PDs.
    *
    * @param name The name of the HLT chain
    * @returns The name of the item/chain seeding the specified HLT chain
    */
   const std::string&
   TrigConfigSvcD3PD::GetLowerChainName( const std::string& name ) const {

      //
      // Check if this information is available:
      //
      if( ! m_initialized ) {
         Error( "GetLowerChainName", "Configuration meta-tree not initialized yet!" );
         return m_dummy;
      }

      //
      // Find the lower chain name of the requested chain:
      //
      std::map< DBKeys_t, std::map< std::string, std::string > >::const_iterator
         cnMap = m_lowerChainNameMap.find( m_key );
      if( cnMap == m_lowerChainNameMap.end() ) {
         Warning( "GetLowerChainName", "The requested information is not available" );
         return m_dummy;
      }
      std::map< std::string, std::string >::const_iterator itr;
      if( ( itr = cnMap->second.find( name ) ) != cnMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetLowerChainName", "HLT chain now known: %s",
               name.c_str() );
      return m_dummy;
   }

   /**
    * This function can be used to do a "reverse lookup" in the LVL1 configuration.
    * Meaning, that if you know that a certain CTP Id fired in the current event,
    * you can use this function to check which LVL1 item that corresponds to.
    *
    * @param ctpid The CTP Id of the item in question
    * @returns The name of the LVL1 item, or an empty string if the lookup is
    *          not successful
    */
   const std::string&
   TrigConfigSvcD3PD::GetNameFromCTPId( ::Int_t ctpid ) const {

      //
      // Find this CTP Id in the internal reverse list:
      //
      std::map< DBKeys_t, std::map< int, std::string > >::const_iterator
         idMap = m_lvl1IDMap.find( m_key );
      if( idMap == m_lvl1IDMap.end() ) {
         Error( "GetNameFromCTPId", "Current configuration not available!" );
         return m_dummy;
      }
      std::map< int, std::string >::const_iterator itr =
         idMap->second.find( ctpid );
      if( itr != idMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetNameFromCTPId", "Couldn't find LVL1 item for CTP Id: %i",
               ctpid );
      return m_dummy;
   }

   /**
    * This function can be used to do a "reverse lookup" in the LVL2 configuration.
    * Meaning, that if you know that a certain Chain Id fired in the current event,
    * you can use this function to check which LVL2 chain that corresponds to.
    *
    * @param chainid The Chain Id of the chain in question
    * @returns The name of the LVL2 chain, or an empty string if the lookup is
    *          not successful
    */
   const std::string&
   TrigConfigSvcD3PD::GetL2NameFromChainId( ::Int_t chainid ) const {

      //
      // Find this Chain Id in the internal reverse list:
      //
      std::map< DBKeys_t, std::map< int, std::string > >::const_iterator
         idMap = m_lvl2IDMap.find( m_key );
      if( idMap == m_lvl2IDMap.end() ) {
         Error( "GetL2NameFromChainId", "Current configuration not available!" );
         return m_dummy;
      }
      std::map< int, std::string >::const_iterator itr =
         idMap->second.find( chainid );
      if( itr != idMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetL2NameFromChainId", "Couldn't find LVL2 chain for Chain Id: %i",
               chainid );
      return m_dummy;
   }

   /**
    * This function can be used to do a "reverse lookup" in the EF configuration.
    * Meaning, that if you know that a certain Chain Id fired in the current event,
    * you can use this function to check which EF chain that corresponds to.
    *
    * @param chainid The Chain Id of the chain in question
    * @returns The name of the EF chain, or an empty string if the lookup is
    *          not successful
    */
   const std::string&
   TrigConfigSvcD3PD::GetEFNameFromChainId( ::Int_t chainid ) const {

      //
      // Find this Chain Id in the internal reverse list:
      //
      std::map< DBKeys_t, std::map< int, std::string > >::const_iterator
         idMap = m_efIDMap.find( m_key );
      if( idMap == m_efIDMap.end() ) {
         Error( "GetEFNameFromChainId", "Current configuration not available!" );
         return m_dummy;
      }
      std::map< int, std::string >::const_iterator itr =
         idMap->second.find( chainid );
      if( itr != idMap->second.end() ) {
         return itr->second;
      }

      Warning( "GetEFNameFromChainId", "Couldn't find EF chain for Chain Id: %i",
               chainid );
      return m_dummy;
   }

   /**
    * This function can be used to get a list of all the configured LVL1 trigger
    * items for the current configuration.
    *
    * @returns A list of all the configured LVL1 trigger item names
    */
   const std::vector< std::string >& TrigConfigSvcD3PD::GetLVL1Items() const {

      std::map< DBKeys_t, std::vector< std::string > >::const_iterator
         items = m_confLVL1Items.find( m_key );
      if( items == m_confLVL1Items.end() ) {
         Error( "GetLVL1Items", "Current configuration not available!" );
         return m_dummyVec;
      }

      return items->second;
   }

   /**
    * This function can be used to get a list of all the configured HLT trigger chains
    * for the current configuration.
    *
    * @returns A list of all the configured HLT trigger chain names
    */
   const std::vector< std::string >& TrigConfigSvcD3PD::GetHLTChains() const {

      std::map< DBKeys_t, std::vector< std::string > >::const_iterator
         chains = m_confHLTChains.find( m_key );
      if( chains == m_confHLTChains.end() ) {
         Error( "GetHLTChains", "Current configuration not available!" );
         return m_dummyVec;
      }

      return chains->second;
   }

   /**
    * This function can be used to print some short summary about the trigger
    * configurations stored in a given D3PD file. Mostly just for debugging.
    */
   void TrigConfigSvcD3PD::PrintSummary() const {

      //
      // Make sure that the object is initialized:
      //
      if( ! m_initialized ) {
         Error( "PrintSummary", "No configurations available yet!" );
         return;
      }

      //
      // Print some minimal information about the available configurations:
      //
      Info( "PrintSummary", "The following configurations are available:" );
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         db_itr = m_lvl1NameMap.begin();
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         db_end = m_lvl1NameMap.end();
      for( ; db_itr != db_end; ++db_itr ) {

         // The number of LVL1 items in this configuration:
         const long unsigned int nItems =
            static_cast< long unsigned int >( db_itr->second.size() );

         // The number of HLT chains in this configuration:
         std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
            hlt_itr = m_hltNameMap.find( db_itr->first );
         if( hlt_itr == m_hltNameMap.end() ) {
            Error( "PrintSummary", "There is a serious coding error here..." );
            return;
         }
         const long unsigned int nChains =
            static_cast< long unsigned int >( hlt_itr->second.size() );

         // Print a summary about this configuration:
         Info( "PrintSummary", "  SMK: %i, L1PSK: %i, HLTPSK: %i, "
               "#%lu LVL1 items, #%lu HLT chains",
               db_itr->first.first, db_itr->first.second.first,
               db_itr->first.second.second, nItems, nChains );
      }

      return;
   }

   /**
    * This function can be used to print quite detailed information about the currently
    * loaded trigger configuration. Mostly used for debugging.
    */
   void TrigConfigSvcD3PD::PrintConfig() const {

      //
      // Make sure that the object is initialized:
      //
      if( ! m_initialized ) {
         Error( "PrintSummary", "No configurations available yet!" );
         return;
      }

      //
      // Print the information about the loaded LVL1 items:
      //
      /*
      Info( "PrintConfig", "*** LVL1 configuration ***" );
      std::map< std::string, int >::const_iterator name_itr = m_lvl1NameMap()->begin();
      std::map< std::string, int >::const_iterator name_end = m_lvl1NameMap()->end();
      std::map< std::string, float >::const_iterator prescale_itr;
      for( ; name_itr != name_end; ++name_itr ) {
         if( ( prescale_itr = m_lvl1PrescaleMap()->find( name_itr->first ) ) ==
             m_lvl1PrescaleMap()->end() ) {
            Fatal( "PrintConfig",
                   "There is an internal inconsistency in the LVL1 configuration!" );
            return;
         }
         Info( "PrintConfig", "Name: %s, \tCTP ID: %i, \tPrescale: %f",
               name_itr->first.c_str(), name_itr->second, prescale_itr->second );
      }
      */

      //
      // Print the information about the loaded HLT chains:
      //
      /*
      Info( "PrintConfig", "*** HLT configuration ***" );
      name_itr = m_hltNameMap()->begin();
      name_end = m_hltNameMap()->end();
      std::map< std::string, float >::const_iterator rprescale_itr;
      std::map< std::string, float >::const_iterator passthrough_itr;
      std::map< std::string, std::string >::const_iterator lowerChain_itr;
      for( ; name_itr != name_end; ++name_itr ) {
         if( ( ( prescale_itr = m_hltPrescaleMap()->find( name_itr->first ) ) ==
               m_hltPrescaleMap()->end() ) ||
             ( ( rprescale_itr = m_hltRerunPrescaleMap()->find( name_itr->first ) ) ==
               m_hltRerunPrescaleMap()->end() ) ||
             ( ( passthrough_itr = m_hltPassthroughMap()->find( name_itr->first ) ) ==
               m_hltPassthroughMap()->end() ) ||
             ( ( lowerChain_itr = m_lowerChainNameMap()->find( name_itr->first ) ) ==
               m_lowerChainNameMap()->end() ) ) {
            Fatal( "PrintConfig",
                   "There is an internal inconsistency in the HLT configuration!" );
            return;
         }
         Info( "PrintConfig", "Name: %s, \tChain ID: %i, \tPrescale: %f, "
               "\tRerun-prescale: %f, \tPassthrough: %f, "
               "\tLower chain name: %s",
               name_itr->first.c_str(), name_itr->second,
               prescale_itr->second, rprescale_itr->second,
               passthrough_itr->second,
               lowerChain_itr->second.c_str() );
      }
      */

      return;
   }

   /**
    * This function is used internally to attach the different branches of the trigger
    * configuration metadata TTree to the internal pointers of the object. It also
    * maps the available configurations for faster access. In the end it sets the object
    * to describe the first configuration available in the metadata TTree.
    *
    * @returns <code>kTRUE</code> if successful,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t TrigConfigSvcD3PD::Initialize( TTree* tree ) {

      // Check if a metadata tree has already been defined:
      if( ! tree ) {
         Error( "Initialize", "No trigger configuration metadata TTree specified!" );
         return kFALSE;
      }

      // Check if we actually received a TChain, not a TTree:
      TChain* chain = dynamic_cast< TChain* >( tree );
      if( chain ) {
         // Just to make sure that the first TTree is loaded...
         if( ! chain->GetTree() ) {
            chain->LoadTree( 0 );
         }
      }

      // Variable used in the TTree reading:
      ::Long64_t entry;

      //
      // Connect the most basic variables:
      //
      D3PDReaderPriv::VarHandle< ::Int_t > smk( this, "SMK", &entry );
      D3PDReaderPriv::VarHandle< ::Int_t > l1psk( this, "L1PSK", &entry );
      D3PDReaderPriv::VarHandle< ::Int_t > hltpsk( this, "HLTPSK", &entry );
      smk.ReadFrom( tree );
      l1psk.ReadFrom( tree );
      hltpsk.ReadFrom( tree );

      //
      // Check whether these exist in the TTree:
      //
      if( ( ! smk.IsAvailable() ) ||
          ( ! l1psk.IsAvailable() ) ||
          ( ! hltpsk.IsAvailable() ) ) {
         Error( "Initialize", "The supplied TTree doesn't seem to be a trigger "
                "configuration metadata tree!" );
         return kFALSE;
      }

      //
      // Connect all the other variables as well:
      //
      D3PDReaderPriv::VarHandle< std::map< std::string, int >* >
         lvl1NameMap( this, "LVL1NameMap", &entry );
      D3PDReaderPriv::VarHandle< std::map< std::string, float >* >
         lvl1PrescaleMap( this, "LVL1PrescaleMap", &entry );
      lvl1NameMap.ReadFrom( tree );
      lvl1PrescaleMap.ReadFrom( tree );

      D3PDReaderPriv::VarHandle< std::map< std::string, int >* >
         hltNameMap( this, "HLTNameMap", &entry );
      D3PDReaderPriv::VarHandle< std::map< std::string, float >* >
         hltPrescaleMap( this, "HLTPrescaleMap", &entry );
      D3PDReaderPriv::VarHandle< std::map< std::string, float >* >
         hltRerunPrescaleMap( this, "HLTRerunPrescaleMap", &entry );
      D3PDReaderPriv::VarHandle< std::map< std::string, float >* >
         hltPassthroughMap( this, "HLTPassthroughMap", &entry );
      D3PDReaderPriv::VarHandle< std::map< std::string, std::string >* >
         lowerChainNameMap( this, "HLTLowerChainNameMap", &entry );
      hltNameMap.ReadFrom( tree );
      hltPrescaleMap.ReadFrom( tree );
      hltRerunPrescaleMap.ReadFrom( tree );
      hltPassthroughMap.ReadFrom( tree );
      lowerChainNameMap.ReadFrom( tree );

      // [TrigMonConfig]
      // Load variables for CHAIN
      D3PDReaderPriv::VarHandle< uint16_t >                                   chainN( this, "chainConfig_n", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint32_t >* >                   chainID( this, "chainConfig_ID", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint16_t >* >                   chainCounter( this, "chainConfig_counter", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::string >* >                chainName( this, "chainConfig_name", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint32_t >* >                   chainLowerID( this, "chainConfig_lowerID", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    chainLowerIDs( this, "chainConfig_lowerChainIDs", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::string >* >                chainLowerName( this, "chainConfig_lowerName", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint16_t >* >                   chainLowerCounter( this, "chainConfig_lowerCounter", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint8_t >* >                    chainLevel( this, "chainConfig_level", &entry );
      D3PDReaderPriv::VarHandle< std::vector< float >* >                      chainPrescale( this, "chainConfig_prescale", &entry );
      D3PDReaderPriv::VarHandle< std::vector< float >* >                      chainPassthrough( this, "chainConfig_passthrough", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< std::string > >* > chainStreamNames( this, "chainConfig_streamNames", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< float > >* >       chainStreamPrescales( this, "chainConfig_streamPrescales", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< std::string > >* > chainGroupNames( this, "chainConfig_groupNames", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< std::string > >* > chainEBHypoNames( this, "chainConfig_EBHypoNames", &entry );
      chainN.ReadFrom( tree );
      chainID.ReadFrom( tree );
      chainCounter.ReadFrom( tree );
      chainName.ReadFrom( tree );
      chainLowerID.ReadFrom( tree );
      chainLowerIDs.ReadFrom( tree );
      chainLowerName.ReadFrom( tree );
      chainLowerCounter.ReadFrom( tree );
      chainLevel.ReadFrom( tree );
      chainPrescale.ReadFrom( tree );
      chainPassthrough.ReadFrom( tree );
      chainStreamNames.ReadFrom( tree );
      chainStreamPrescales.ReadFrom( tree );
      chainGroupNames.ReadFrom( tree );
      chainEBHypoNames.ReadFrom( tree );
      // variables for CHAIN -> SIGNATURE
      D3PDReaderPriv::VarHandle< std::vector< uint16_t >* >                   chainSigN( this, "chainConfig_sig_n", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    chainSigCounter( this, "chainConfig_sig_counter", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    chainSigLogic( this, "chainConfig_sig_logic", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< std::string > >* > chainSigLabel( this, "chainConfig_sig_label", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint16_t > >* >    chainSigOutputTEn( this, "chainConfig_sig_outputTE_n", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    chainSigOutputTEIndex( this, "chainConfig_sig_outputTE_index", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    chainSigOutputTEs( this, "chainConfig_sigOutputTEs", &entry );
      chainSigN.ReadFrom( tree );
      chainSigCounter.ReadFrom( tree );
      chainSigLogic.ReadFrom( tree );
      chainSigLabel.ReadFrom( tree );
      chainSigOutputTEn.ReadFrom( tree );
      chainSigOutputTEIndex.ReadFrom( tree );
      chainSigOutputTEs.ReadFrom( tree );
      // Load variables for SEQUENCE
      D3PDReaderPriv::VarHandle< uint16_t >                                   seqN( this, "seqConfig_n", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint32_t >* >                   seqID( this, "seqConfig_outputTEID", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint16_t >* >                   seqIndex( this, "seqConfig_outputTEIndex", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::string >* >                seqName( this, "seqConfig_outputTEName", &entry );
      D3PDReaderPriv::VarHandle< std::vector< uint32_t >* >                   seqTopoTEID( this, "seqConfig_topoTEID", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    seqInputTEs( this, "seqConfig_inputTEs", &entry );
      seqN.ReadFrom( tree );
      seqID.ReadFrom( tree );
      seqIndex.ReadFrom( tree );
      seqName.ReadFrom( tree );
      seqTopoTEID.ReadFrom( tree );
      seqInputTEs.ReadFrom( tree );
      // variables for SEQUENCE -> ALG
      D3PDReaderPriv::VarHandle< std::vector< uint16_t >* >                   seqAlgN( this, "seqConfig_alg_n", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint16_t > >* >    seqAlgIndex( this, "seqConfig_alg_index", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint8_t > >* >     seqAlgPosition( this, "seqConfig_alg_position", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    seqAlgNameID( this, "seqConfig_alg_nameID", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< uint32_t > >* >    seqAlgTypeID( this, "seqConfig_alg_typeID", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< std::string > >* > seqAlgName( this, "seqConfig_alg_name", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::vector< std::string > >* > seqAlgTypeName( this, "seqConfig_alg_typeName", &entry );
      seqAlgN.ReadFrom( tree );
      seqAlgIndex.ReadFrom( tree );
      seqAlgPosition.ReadFrom( tree );
      seqAlgNameID.ReadFrom( tree );
      seqAlgTypeID.ReadFrom( tree );
      seqAlgName.ReadFrom( tree );
      seqAlgTypeName.ReadFrom( tree );
      // Variable for META
      D3PDReaderPriv::VarHandle< std::vector< std::string >* >                metaStringKey( this, "config_miscStringKey", &entry );
      D3PDReaderPriv::VarHandle< std::vector< std::string >* >                metaStringVal( this, "config_miscStringVal", &entry );
      metaStringKey.ReadFrom( tree );
      metaStringVal.ReadFrom( tree );

      //
      // Check whether the LVL1 branches exist:
      //
      if( ( ! lvl1NameMap.IsAvailable() ) ||
          ( ! lvl1PrescaleMap.IsAvailable() ) ) {
         Error( "Initialize", "The supplied TTree doesn't hold the LVL1 branches" );
         Error( "Initialize", "This is VERY strange" );
         return kFALSE;
      }

      //
      // Check whether the basic HLT branches exist:
      //
      if( ( ! hltNameMap.IsAvailable() ) ||
          ( ! hltPrescaleMap.IsAvailable() ) ) {
         Error( "Initialize", "The supplied TTree doesn't hold the HLT branches" );
         Error( "Initialize", "This is VERY strange" );
         return kFALSE;
      }

      //
      // Check whether these extra branches exist:
      //
      ::Bool_t extraBrAvailable = kTRUE;
      if( ( ! hltRerunPrescaleMap.IsAvailable() ) ||
          ( ! hltPassthroughMap.IsAvailable() ) ||
          ( ! lowerChainNameMap.IsAvailable() ) ) {
         Warning( "Initialize", "The supplied TTree doesn't have some HLT branches" );
         Warning( "Initialize", "This is expected for slightly older D3PD files" );
         extraBrAvailable = kFALSE;
      }

      //
      // Check whether extended [TrigMonConfig] branches exist (don't check them all):
      //
      ::Bool_t trigMonConfigAvailable = kTRUE;
      if( ( ! chainN.IsAvailable() ) ||
          ( ! chainSigN.IsAvailable() ) ||
          ( ! seqN.IsAvailable() ) ||
          ( ! seqAlgN.IsAvailable() )) {
         // Dont warn user as this is a special use case
         trigMonConfigAvailable = kFALSE;
      }


      //
      // Load all configurations from the metadata tree:
      //
      for( Long64_t i = 0; i < tree->GetEntries(); ++i ) {

         // If the user specified a TChain as the metadata tree, then we have to be
         // a bit smarter:
         if( chain ) {
            entry = chain->LoadTree( i );
            if( entry < 0 ) break;
         }
         // In case it's a simple TTree, then we don't have to be all that careful:
         else {
            entry = i;
         }

         const DBKeys_t keys = MakeDBKeys( smk(), l1psk(), hltpsk() );

         // reprocessing can give us the same keys many times - only do this once per keyset!
         if (m_lvl1NameMap.count(keys) == 1) continue;

         m_lvl1NameMap[ keys ]     = *( lvl1NameMap() );
         m_lvl1PrescaleMap[ keys ] = *( lvl1PrescaleMap() );

         m_hltNameMap[ keys ]     = *( hltNameMap() );
         m_hltPrescaleMap[ keys ] = *( hltPrescaleMap() );

         if( extraBrAvailable ) {
            m_hltRerunPrescaleMap[ keys ] = *( hltRerunPrescaleMap() );
            m_hltPassthroughMap[ keys ]   = *( hltPassthroughMap() );
            m_lowerChainNameMap[ keys ]   = *( lowerChainNameMap() );
         }

         // [TrigMonConfig]
         if( trigMonConfigAvailable ) {
            // variables for CHAIN
            m_chainN[ keys ] = chainN();
            m_chainID[ keys ] = *( chainID() );
            m_chainCounter[ keys ] = *( chainCounter() );
            m_chainName[ keys ] = *( chainName() );
            m_chainLowerID[ keys ] = *( chainLowerID() );
            m_chainLowerIDs[ keys ] = *( chainLowerIDs() );
            m_chainLowerName[ keys ] = *( chainLowerName() );
            m_chainLowerCounter[ keys ] = *( chainLowerCounter() );
            m_chainLevel[ keys ] = *( chainLevel() );
            m_chainPrescale[ keys ] = *( chainPrescale() );
            m_chainPassthrough[ keys ] = *( chainPassthrough() );
            m_chainStreamNames[ keys ] = *( chainStreamNames() );
            m_chainStreamPrescales[ keys ] = *( chainStreamPrescales() );
            m_chainGroupNames[ keys ] = *( chainGroupNames() );
            m_chainEBHypoNames[ keys ] = *( chainEBHypoNames() );
            // variables for CHAIN -> SIGNATURE
            m_chainSigN[ keys ] = *( chainSigN() );
            m_chainSigCounter[ keys ] = *( chainSigCounter() );
            m_chainSigLogic[ keys ] = *( chainSigLogic() );
            m_chainSigLabel[ keys ] = *( chainSigLabel() );
            m_chainSigOutputTEn[ keys ] = *( chainSigOutputTEn() );
            m_chainSigOutputTEIndex[ keys ] = *( chainSigOutputTEIndex() );
            m_chainSigOutputTEs[ keys ] = *( chainSigOutputTEs() );
            // variables for SEQUENCE
            m_seqN[ keys ] = seqN();
            m_seqID[ keys ] = *( seqID() );
            m_seqIndex[ keys ] = *( seqIndex() );
            m_seqName[ keys ] = *( seqName() );
            m_seqTopoTEID[ keys ] = *( seqTopoTEID() );
            m_seqInputTEs[ keys ] = *( seqInputTEs() );
            // variables for SEQUENCE -> ALG
            m_seqAlgN[ keys ] = *( seqAlgN() );
            m_seqAlgIndex[ keys ] = *( seqAlgIndex() );
            m_seqAlgPosition[ keys ] = *( seqAlgPosition() );
            m_seqAlgNameID[ keys ] = *( seqAlgNameID() );
            m_seqAlgTypeID[ keys ] = *( seqAlgTypeID() );
            m_seqAlgName[ keys ] = *( seqAlgName() );
            m_seqAlgTypeName[ keys ] = *( seqAlgTypeName() );
            // variables for META
            m_metaStringKey[ keys ] = *( metaStringKey() );
            m_metaStringVal[ keys ] = *( metaStringVal() );

            // Initialise the memory for the helper maps.
            m_chainCounterMap[ keys ] = std::map< unsigned int, unsigned int >();
            m_TEIndexMap[ keys ] = std::map< unsigned int, unsigned int >();
            m_hltSequenceNameMap[ keys ] = std::map< unsigned int , std::string >();
            m_hltAlgorithmNameMap  [ keys ] = std::map< std::pair< int, int > , std::string >();
            m_hltAlgorithmNameIDMap[ keys ] = std::map< std::pair< int, int > , unsigned int >();
            m_hltAlgorithmTypeMap  [ keys ] = std::map< std::pair< int, int > , std::string >();
            m_hltAlgorithmTypeIDMap[ keys ] = std::map< std::pair< int, int > , unsigned int >();

            //
            // [TrigMonConfig]
            // Temporarily set the 'global' key m_key and populate the newly formed helper maps
            //
            m_key = keys;
            GenerateSignatureSequenceMap();
         }
      }

      //
      // Reset the branches to the first entry:
      //
      if( chain ) {
         chain->LoadTree( 0 );
      }
      entry = 0;
      m_key = MakeDBKeys( smk(), l1psk(), hltpsk() );

      //
      // Update the cached trigger names:
      //
      if( ! UpdateTriggerNames() ) {
         Error( "Initialize", "Failed to cache the configured trigger names" );
         return kFALSE;
      }

      Info( "Initialize",
            "Loaded configuration: SMK: %i, L1PSK: %i, HLTPSK: %i",
            smk(), l1psk(), hltpsk() );

      m_initialized = kTRUE;
      return kTRUE;
   }

   /**
    * This function is used internally to clear the memory allocated by the object.
    */
   void TrigConfigSvcD3PD::Reset() {

      m_initialized = kFALSE;

      m_lvl1NameMap.clear();
      m_lvl1PrescaleMap.clear();

      m_hltNameMap.clear();
      m_hltPrescaleMap.clear();
      m_hltRerunPrescaleMap.clear();
      m_hltPassthroughMap.clear();

      m_lowerChainNameMap.clear();

      // [TrigMonConfig]
      m_chainCounterMap.clear();
      m_TEIndexMap.clear();
      m_hltSequenceNameMap.clear();
      m_hltAlgorithmNameMap.clear();
      m_hltAlgorithmNameIDMap.clear();
      m_hltAlgorithmTypeMap.clear();
      m_hltAlgorithmTypeIDMap.clear();

      return;
   }

   /**
    * This function is used internally to update the variables caching the names of
    * all the LVL1 trigger items and HLT trigger chains.
    *
    * @returns <code>kTRUE</code> if the operation was successful, or
    *          <code>kFALSE</code> otherwise
    */
   Bool_t TrigConfigSvcD3PD::UpdateTriggerNames() {

      // Clear the currently cached information:
      m_confLVL1Items.clear();
      m_confHLTChains.clear();
      m_lvl1IDMap.clear();
      m_lvl2IDMap.clear();
      m_efIDMap.clear();

      // Cache the names of the LVL1 items:
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         map_itr = m_lvl1NameMap.begin();
      std::map< DBKeys_t, std::map< std::string, int > >::const_iterator
         map_end = m_lvl1NameMap.end();
      for( ; map_itr != map_end; ++map_itr ) {
         std::map< std::string, int >::const_iterator itr = map_itr->second.begin();
         std::map< std::string, int >::const_iterator end = map_itr->second.end();
         for( ; itr != end; ++itr ) {
            m_confLVL1Items[ map_itr->first ].push_back( itr->first );
            m_lvl1IDMap[ map_itr->first ][ itr->second ] = itr->first;
         }
      }

      // Cache the names of the HLT chains:
      map_itr = m_hltNameMap.begin();
      map_end = m_hltNameMap.end();
      for( ; map_itr != map_end; ++map_itr ) {
         std::map< std::string, int >::const_iterator itr = map_itr->second.begin();
         std::map< std::string, int >::const_iterator end = map_itr->second.end();
         for( ; itr != end; ++itr ) {
            m_confHLTChains[ map_itr->first ].push_back( itr->first );
            if( itr->first.find( "L2_" ) == 0 || itr->first.find( "HLT_" ) == 0 ) {
               m_lvl2IDMap[ map_itr->first ][ itr->second ] = itr->first;
            } else if( itr->first.find( "EF_" ) == 0 ) {
               m_efIDMap[ map_itr->first ][ itr->second ] = itr->first;
            } else {
               Error( "UpdateTriggerNames",
                      "Can't decide if chain \"%s\" is a LVL2, EF or HLT chain",
                      itr->first.c_str() );
               return kFALSE;
            }
         }
      }

      return kTRUE;
   }

   /**
    * This is just a convenience function for generating DBKeys_t objects quickly.
    *
    * @param SMK Super Master Key
    * @param L1PSK LVL1 Prescale Key
    * @param HLTPSK HLT Prescale Key
    * @returns A DBKeys_t object describing the specified configuration
    */
   TrigConfigSvcD3PD::DBKeys_t
   TrigConfigSvcD3PD::MakeDBKeys( Int_t SMK, Int_t L1PSK, Int_t HLTPSK ) {

      return std::make_pair( SMK, std::make_pair( L1PSK, HLTPSK ) );
   }

   /**
    * [TrigMonConf] Returns the index of the chain with the given counter
    *
    * @param _counter Chain counter to locate in D3PD.
    * @returns The chain's index.
    */
   UInt_t TrigConfigSvcD3PD::GetChainIndexFromCounter( UInt_t counter ) const {
      std::map< DBKeys_t, std::map< unsigned int, unsigned int > >::const_iterator map = m_chainCounterMap.find( m_key );

      if( map == m_chainCounterMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetChainIndexFromCounter", "Current configuration not available!" );
         return UINT_MAX;
      }

      std::map< unsigned int, unsigned int >::const_iterator itr = map->second.find( counter );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetChainIndexFromCounter", "Couldn't find config treee D3PD index for chain with counter: %i", counter );
      return UINT_MAX;
   }

   /**
    * [TrigMonConf] Returns the index of the sequence with given output TE.
    *
    * @param _outputTE Output trigger element to fetch sequence index for.
    * @returns The sequence index.
    */
   UInt_t TrigConfigSvcD3PD::GetSequenceIndex( UInt_t outputTE ) const {
      std::map< DBKeys_t, std::map< unsigned int, unsigned int > >::const_iterator map = m_TEIndexMap.find( m_key );

      if( map == m_TEIndexMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetSequenceIndex", "Current configuration not available!" );
         return UINT_MAX;
      }

      std::map< unsigned int, unsigned int >::const_iterator itr = map->second.find( outputTE );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetSequenceIndex", "Couldn't find Sequence Index for OutputTE: %i", outputTE );
      return UINT_MAX;
   }

   /**
    * [TrigMonConf] Returns the name of a sequence from its index ID
    *
    * @param _index The sequence index.
    * @returns The name of the sequence with given index.
    */
    const std::string& TrigConfigSvcD3PD::GetSequenceNameFromIndex( unsigned int index ) const {
      std::map< DBKeys_t, std::map< unsigned int, std::string > >::const_iterator map = m_hltSequenceNameMap.find( m_key );

      if( map == m_hltSequenceNameMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetSequenceNameFromIndex", "Current configuration not available!" );
         return m_dummy;
      }

      std::map< unsigned int, std::string >::const_iterator itr = map->second.find( index );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetSequenceNameFromIndex", "Couldn't find Sequence Name for Index: %i", index );
      return m_dummy;
    }

   /**
    * [TrigMonConf] Returns the name of an algorithm specified by its parent sequence and location within the sequence.
    *
    * @param _sequenceID Index of algorithms parent sequence.
    * @param _algorithmPosition Position in sequence of algorithm.
    * @returns The algorithm name.
    */
    const std::string& TrigConfigSvcD3PD::GetAlgNameFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const {
      std::map< DBKeys_t, std::map< std::pair<int,int> , std::string > >::const_iterator map = m_hltAlgorithmNameMap.find( m_key );
      std::pair<int,int> toFind( sequenceID, algorithmPosition );

      if( map == m_hltAlgorithmNameMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetAlgNameFromSeqIDAndAlgPos", "Current configuration not available!" );
         return m_dummy;
      }

      std::map< std::pair<Int_t,Int_t>, std::string >::const_iterator itr = map->second.find( toFind );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetAlgNameFromSeqIDAndAlgPos", "Couldn't find Algorithm Name for Seq Index: %i Alg Pos:%i", sequenceID, algorithmPosition );
      return m_dummy;
    }

   /**
    * [TrigMonConf] Returns the hash of the name of an algorithm specified by its parent sequence and location within the sequence.
    *
    * @param sequenceID Index of algorithms parent sequence.
    * @param algorithmPosition Position in sequence of algorithm.
    * @returns The hash of the algorithm name.
    */
    UInt_t TrigConfigSvcD3PD::GetAlgNameIDFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const {
      std::map< DBKeys_t, std::map< std::pair<int,int> , unsigned int > >::const_iterator map = m_hltAlgorithmNameIDMap.find( m_key );
      std::pair<int,int> toFind( sequenceID, algorithmPosition );

      if( map == m_hltAlgorithmNameIDMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetAlgNameIDFromSeqIDAndAlgPos", "Current configuration not available!" );
         return 0;
      }

      std::map< std::pair<int,int>, unsigned int >::const_iterator itr = map->second.find( toFind );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetAlgNameIDFromSeqIDAndAlgPos", "Couldn't find Algorithm Name for Seq Index: %i Alg Pos:%i", sequenceID, algorithmPosition );
      return 0;
    }

   /**
    * [TrigMonConf] Returns the class name of an algorithm specified by its parent sequence and location within the sequence.
    *
    * @param sequenceID Index of algorithms parent sequence.
    * @param algorithmPosition Position in sequence of algorithm.
    * @returns The algorithm class name.
    */
    const std::string& TrigConfigSvcD3PD::GetAlgClassNameFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const {
      std::map< DBKeys_t, std::map< std::pair<int,int> , std::string > >::const_iterator map = m_hltAlgorithmTypeMap.find( m_key );
      std::pair<int,int> toFind( sequenceID, algorithmPosition );

      if( map == m_hltAlgorithmTypeMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetAlgClassNameFromSeqIDAndAlgPos", "Current configuration not available!" );
         return m_dummy;
      }

      std::map< std::pair<int,int>, std::string >::const_iterator itr = map->second.find( toFind );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetAlgClassNameFromSeqIDAndAlgPos", "Couldn't find Algorithm Type for Seq Index: %i Alg Pos:%i", sequenceID, algorithmPosition );
      return m_dummy;
    }

   /**
    * [TrigMonConf] Returns the hash of the class name of an algorithm specified by its parent sequence and location within the sequence.
    *
    * @param sequenceID Index of algorithms parent sequence.
    * @param algorithmPosition Position in sequence of algorithm.
    * @returns The hash of the algorithm class name.
    */
    UInt_t TrigConfigSvcD3PD::GetAlgClassNameIDFromSeqIDAndAlgPos( int sequenceID, int algorithmPosition ) const {
      std::map< DBKeys_t, std::map< std::pair<int,int> , unsigned int > >::const_iterator map = m_hltAlgorithmTypeIDMap.find( m_key );
      std::pair<int,int> toFind( sequenceID, algorithmPosition );

      if( map == m_hltAlgorithmTypeIDMap.end() ) {
         Error( "TrigConfigSvcD3PD::GetAlgClassNameIDFromSeqIDAndAlgPos", "Current configuration not available!" );
         return 0;
      }

      std::map< std::pair<int,int>, unsigned int >::const_iterator itr = map->second.find( toFind );
      if( itr != map->second.end() ) {
         return itr->second;
      }
      Warning( "TrigConfigSvcD3PD::GetAlgClassNameIDFromSeqIDAndAlgPos", "Couldn't find Algorithm Type for Seq Index: %i Alg Pos:%i", sequenceID, algorithmPosition );
      return 0;
    }

   /**
    * [TrigMonConf] Returns the number of configured trigger chains.
    *
    * @returns The the algorithm name.
    */
   UInt_t TrigConfigSvcD3PD::GetChainN() const{
     std::map< DBKeys_t, uint16_t >::const_iterator map = m_chainN.find( m_key );

     if( map == m_chainN.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainN", "The requested information is not available" );
       return 0;
     }

     return map->second;
   }

   /**
    * [TrigMonConf] Returns the level of the chain at given index.
    *
    * @param c D3PD index of chain.
    * @returns The chain level (1 = LVL1, 2 = L2 or HLT, 3 = EF).
    */
   UInt_t TrigConfigSvcD3PD::GetChainLevel(UInt_t c) const {
     std::map< DBKeys_t, std::vector< uint8_t > >::const_iterator map = m_chainLevel.find( m_key );

     if( map == m_chainLevel.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainLevel", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c);
   }

   /**
    * [TrigMonConf] Returns the menu counter index for a chain.
    *
    * @param c D3PD index of chain.
    * @returns The chain counter index (HLT/L2/EF) or CTPID (LVL1).
    */
   UInt_t TrigConfigSvcD3PD::GetChainCounter(UInt_t c) const {
     std::map< DBKeys_t, std::vector< uint16_t > >::const_iterator map = m_chainCounter.find( m_key );

     if( map == m_chainCounter.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainCounter", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c);
   }

   /**
    * [TrigMonConf] Returns the name of the chain at given index.
    *
    * @param c D3PD index of chain.
    * @returns The chain name.
    */
   std::string TrigConfigSvcD3PD::GetChainName(UInt_t c) const {
     std::map< DBKeys_t, std::vector< std::string > >::const_iterator map = m_chainName.find( m_key );

     if( map == m_chainName.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainName", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c);
   }

   /**
    * [TrigMonConf] Returns the size of the Enhanced Bias vector of chain items for a given chain.
    *
    * @param c D3PD index of chain.
    * @returns The size of the enhanced bias name vector.
    */
   UInt_t TrigConfigSvcD3PD::GetChainEBHypoNameSize(UInt_t c) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_chainEBHypoNames.find( m_key );

     if( map == m_chainEBHypoNames.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainEBHypoNameSize", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c).size();
   }

   /**
    * [TrigMonConf] Returns the Enhanced Bias item name for a given entry in the EB hypo vector for the chain at given index.
    *
    * @param c D3PD index of chain.
    * @param _h Vector index in the Enhanced Bias Hypo vector for this chain.
    * @returns The Enhanced Bias hypo item name.
    */
   std::string TrigConfigSvcD3PD::GetChainEBHypoName(UInt_t c, UInt_t h) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_chainEBHypoNames.find( m_key );

     if( map == m_chainEBHypoNames.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainEBHypoName", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c).at(h);
   }

   /**
    * [TrigMonConf] Returns the size of the Group Name vector of chain items for a given chain.
    *
    * @param c D3PD index of chain.
    * @returns The size of the group name vector.
    */
   UInt_t TrigConfigSvcD3PD::GetChainGroupNameSize(UInt_t c) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_chainGroupNames.find( m_key );

     if( map == m_chainGroupNames.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainGroupNameSize", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c).size();
   }

   /**
    * [TrigMonConf] Returns the Group Name for a given entry in the group name vector for the chain at given index.
    *
    * @param c D3PD index of chain.
    * @param _g Vector index in the group name vector.
    * @returns The group name.
    */
   std::string TrigConfigSvcD3PD::GetChainGroupName(UInt_t c, UInt_t g) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_chainGroupNames.find( m_key );

     if( map == m_chainGroupNames.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainGroupName", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c).at(g);
   }

    /**
    * [TrigMonConf] Returns the size of the Stream Name vector of chain items for a given chain.
    *
    * @param _c D3PD index of chain.
    * @returns The size of the Stream name vector.
    */
   UInt_t TrigConfigSvcD3PD::GetChainStreamNameSize(UInt_t c) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator chainmap = m_chainStreamNames.find( m_key );

     if( chainmap == m_chainStreamNames.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainStreamNameSize", "The requested information is not available" );
       return 0;
     }

     return chainmap->second.at(c).size();
   }

   /**
    * [TrigMonConf] Returns the Stream Name for a given entry in the Stream name vector for the chain at given index.
    *
    * @param c D3PD index of chain.
    * @param _g Vector index in the Stream name vector.
    * @returns The Stream name.
    */
   std::string TrigConfigSvcD3PD::GetChainStreamName(UInt_t c, UInt_t g) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator chainmap = m_chainStreamNames.find( m_key );

     if( chainmap == m_chainStreamNames.end() ) {
       Warning( "TrigConfigSvcD3PD::GetChainStreamName", "The requested information is not available" );
       return 0;
     }

     return chainmap->second.at(c).at(g);
   }

   /**
    * [TrigMonConf] Returns the number of configured signatures for a given chain.
    *
    * @param c D3PD index of chain.
    * @returns The number of signature steps configured for this chain.
    */
   UInt_t TrigConfigSvcD3PD::GetSigN(UInt_t c) const {
     std::map< DBKeys_t, std::vector<  uint16_t > >::const_iterator map = m_chainSigN.find( m_key );

     if( map == m_chainSigN.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigN", "The requested information is not available" );
       return 0;
     }

     return map->second.at(c);
   }

   /**
    * [TrigMonConf] Returns the signature ID for a given signature in a given chain.
    *
    * @param c D3PD index of chain.
    * @param _s D3PD index of signature.
    * @returns The signatures counter.
    */
   UInt_t  TrigConfigSvcD3PD::GetSigCounter(UInt_t c, UInt_t s) const {
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_chainSigCounter.find( m_key );

     if( map == m_chainSigCounter.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigCounter", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(c)).at(s);
   }

   /**
    * [TrigMonConf] Returns the signature logic for a given signature in a given chain.
    *
    * @param c D3PD index of chain.
    * @param s D3PD index of signature.
    * @returns The signatures logic.
    */
   UInt_t  TrigConfigSvcD3PD::GetSigLogic(UInt_t c, UInt_t s) const {
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_chainSigLogic.find( m_key );

     if( map == m_chainSigLogic.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigLogic", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(c)).at(s);
   }

   /**
    * [TrigMonConf] Returns the signature label (name) for a given signature in a given chain.
    *
    * @param c D3PD index of chain.
    * @param s D3PD index of signature.
    * @returns The signatures label (name).
    */
   std::string TrigConfigSvcD3PD::GetSigLabel(UInt_t c, UInt_t s) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_chainSigLabel.find( m_key );

     if( map == m_chainSigLabel.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigLabel", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(c)).at(s);
   }

   /**
    * [TrigMonConf] Returns the number of output trigger elements for a given signature in a given chain.
    *
    * @param c D3PD index of chain.
    * @param s D3PD index of signature.
    * @returns The number of output trigger elements.
    */
   UInt_t TrigConfigSvcD3PD::GetSigNOutputTE(UInt_t c, UInt_t s) const {
     std::map< DBKeys_t, std::vector< std::vector< uint16_t > > >::const_iterator map = m_chainSigOutputTEn.find( m_key );

     if( map == m_chainSigOutputTEn.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigNOutputTE", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(c)).at(s);
   }

   /**
    * [TrigMonConf] Returns the requested output trigger element for a given signature in a given chain.
    *
    * @param c D3PD index of chain.
    * @param s D3PD index of signature.
    * @param t Output trigger element to request.
    * @returns The output trigger element ID.
    */
   UInt_t TrigConfigSvcD3PD::GetSigOutputTE(UInt_t c, UInt_t s, UInt_t t) const {
     // Nested information, first look up index then fetch required entry
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator indexMap = m_chainSigOutputTEIndex.find( m_key );
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_chainSigOutputTEs.find( m_key );

     if( indexMap == m_chainSigOutputTEIndex.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigOutputTE::index", "The requested information is not available" );
       return 0;
     }
     if( map == m_chainSigOutputTEs.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSigOutputTE::outputTEs", "The requested information is not available" );
       return 0;
     }

     UInt_t index = (indexMap->second.at(c)).at(s);
     return (map->second.at(index)).at(t);
   }

   /**
    * [TrigMonConf] Returns the number of configures sequences.
    *
    * @returns The number of sequences.
    */
   UInt_t TrigConfigSvcD3PD::GetSeqN() const {
     std::map< DBKeys_t, uint16_t >::const_iterator map = m_seqN.find( m_key );

     if( map == m_seqN.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSeqN", "The requested information is not available" );
       return 0;
     }

     return map->second;
   }

   /**
    * [TrigMonConf] Returns the ID for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @returns The sequence ID.
    */
   UInt_t TrigConfigSvcD3PD::GetSeqID(UInt_t s) const {
      std::map< DBKeys_t, std::vector< uint32_t > >::const_iterator map = m_seqID.find( m_key );

     if( map == m_seqID.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSeqID", "The requested information is not available" );
       return 0;
     }

     return map->second.at(s);
   }

   /**
    * [TrigMonConf] Returns the index for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @returns The sequence inxed.
    */
   UInt_t TrigConfigSvcD3PD::GetSeqIndex(UInt_t s) const {
      std::map< DBKeys_t, std::vector< uint16_t > >::const_iterator map = m_seqIndex.find( m_key );

     if( map == m_seqIndex.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSeqIndex", "The requested information is not available" );
       return 0;
     }

     return map->second.at(s);
   }

   /**
    * [TrigMonConf] Returns the name for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @returns The sequence name.
    */
   std::string TrigConfigSvcD3PD::GetSeqName(UInt_t s) const {
     std::map< DBKeys_t, std::vector< std::string > >::const_iterator map = m_seqName.find( m_key );

     if( map == m_seqName.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSeqName", "The requested information is not available" );
       return 0;
     }

     return map->second.at(s);
   }

   /**
    * [TrigMonConf] Returns the number of input trigger elements for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @returns The number of input trigger elements.
    */
   UInt_t TrigConfigSvcD3PD::GetSeqNInputTEs(UInt_t s) const {
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_seqInputTEs.find( m_key );

     if( map == m_seqInputTEs.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSeqNInputTEs", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).size();
   }


   /**
    * [TrigMonConf] Returns the ID of an input trigger element for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @param Input trigger element to request.
    * @returns ID of requested input trigger element.
    */
   UInt_t TrigConfigSvcD3PD::GetSeqInputTE(UInt_t s, UInt_t t) const {
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_seqInputTEs.find( m_key );

     if( map == m_seqInputTEs.end() ) {
       Warning( "TrigConfigSvcD3PD::GetSeqInputTE", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(t);
   }

   /**
    * [TrigMonConf] Returns the number of algorithms for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @returns The number of algorithms.
    */
   UInt_t TrigConfigSvcD3PD::GetAlgN(UInt_t s) const {
     std::map< DBKeys_t, std::vector<  uint16_t > >::const_iterator map = m_seqAlgN.find( m_key );

     if( map == m_seqAlgN.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgN", "The requested information is not available" );
       return 0;
     }

     return map->second.at(s);
   }

   /**
    * [TrigMonConf] Returns the index of an algorithm for a given sequence.
    *
    * @param s D3PD index of sequence.
    * @param a Algorithm to request
    * @returns The index of the algorithm.
    */
   UInt_t TrigConfigSvcD3PD::GetAlgIndex(UInt_t s, UInt_t a) const {
     std::map< DBKeys_t, std::vector< std::vector< uint16_t > > >::const_iterator map = m_seqAlgIndex.find( m_key );

     if( map == m_seqAlgIndex.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgIndex", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(a);
   }

   /**
    * [TrigMonConf] Returns the position of an algorithm in its parent sequence.
    *
    * @param s D3PD index of sequence.
    * @param a Algorithm to request
    * @returns The position in the sequence of the algorithm.
    */
   UInt_t TrigConfigSvcD3PD::GetAlgPosition(UInt_t s, UInt_t a) const {
     std::map< DBKeys_t, std::vector< std::vector< uint8_t > > >::const_iterator map = m_seqAlgPosition.find( m_key );

     if( map == m_seqAlgPosition.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgPosition", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(a);
   }

   /**
    * [TrigMonConf] Returns the hash of the name of an algorithm from its position in its parent sequence.
    *
    * @param s D3PD index of sequence.
    * @param a Algorithm to request
    * @returns The hash of the name of the algorithm.
    */
   UInt_t TrigConfigSvcD3PD::GetAlgNameID(UInt_t s, UInt_t a) const {
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_seqAlgNameID.find( m_key );

     if( map == m_seqAlgNameID.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgNameID", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(a);
   }

   /**
    * [TrigMonConf] Returns the name of an algorithm from its position in its parent sequence.
    *
    * @param s D3PD index of sequence.
    * @param a Algorithm to request
    * @returns The name of the algorithm.
    */
   std::string TrigConfigSvcD3PD::GetAlgName(UInt_t s, UInt_t a) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_seqAlgName.find( m_key );

     if( map == m_seqAlgName.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgName", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(a);
   }

   /**
    * [TrigMonConf] Returns the hast of the type-name of an algorithm from its position in its parent sequence.
    *
    * @param s D3PD index of sequence.
    * @param a Algorithm to request
    * @returns The hash of the type-name of the algorithm.
    */
   UInt_t TrigConfigSvcD3PD::GetAlgTypeID(UInt_t s, UInt_t a) const {
     std::map< DBKeys_t, std::vector< std::vector< uint32_t > > >::const_iterator map = m_seqAlgTypeID.find( m_key );

     if( map == m_seqAlgTypeID.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgTypeID", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(a);
   }

   /**
    * [TrigMonConf] Returns the type-name (class name) of an algorithm from its position in its parent sequence.
    *
    * @param s D3PD index of sequence.
    * @param a Algorithm to request
    * @returns The type-name of the algorithm.
    */
   std::string TrigConfigSvcD3PD::GetAlgTypeName(UInt_t s, UInt_t a) const {
     std::map< DBKeys_t, std::vector< std::vector< std::string > > >::const_iterator map = m_seqAlgTypeName.find( m_key );

     if( map == m_seqAlgTypeName.end() ) {
       Warning( "TrigConfigSvcD3PD::GetAlgTypeName", "The requested information is not available" );
       return 0;
     }

     return (map->second.at(s)).at(a);
   }

   /**
    * [TrigMonConf] Returns the number of string->string metadata pairs stored along with this config
    *
    * @returns The number of meta data entries
    */
   UInt_t TrigConfigSvcD3PD::GetMetaStringN() const {
     std::map< DBKeys_t, std::vector< std::string > >::const_iterator map = m_metaStringKey.find( m_key );

     if( map == m_metaStringKey.end() ) {
       Warning( "TrigConfigSvcD3PD::GetMetaStringN", "The requested information is not available" );
       return 0;
     }

     return (UInt_t) map->second.size();
   }

   /**
    * [TrigMonConf] Returns the string key of metadata at location m
    *
    * @param m D3PD index of metadata.
    * @returns The key of the metadata
    */
   std::string TrigConfigSvcD3PD::GetMetaStringKey(UInt_t m) const {
     std::map< DBKeys_t, std::vector< std::string > >::const_iterator map = m_metaStringKey.find( m_key );

     if( map == m_metaStringKey.end() ) {
       Warning( "TrigConfigSvcD3PD::GetMetaStringKey", "The requested information is not available" );
       return 0;
     }

     return map->second.at(m);
   }

   /**
    * [TrigMonConf] Returns the string value of metadata at location m
    *
    * @param m D3PD index of metadata.
    * @returns The metadata payload
    */
   std::string TrigConfigSvcD3PD::GetMetaStringVal(UInt_t m) const {
     std::map< DBKeys_t, std::vector< std::string > >::const_iterator map = m_metaStringVal.find( m_key );

     if( map == m_metaStringVal.end() ) {
       Warning( "TrigConfigSvcD3PD::GetMetaStringValue", "The requested information is not available" );
       return 0;
     }

     return map->second.at(m);
   }

   /**
    * [TrigMonConf] Populates helper maps with trigger configuration using the full dump performed with TrigMonConfig
    * running of the trigger with TrigCost enabled. Uses m_key as the key to populate, as do all the helper functions
    */
   void TrigConfigSvcD3PD::GenerateSignatureSequenceMap() {

     std::map< DBKeys_t, std::map< unsigned int, unsigned int > >::iterator map = m_TEIndexMap.find( m_key );

      if( map == m_TEIndexMap.end() ) {
         Error( "TrigConfigSvcD3PD::GenerateSignatureSequenceMap",
           "Current configuration (%i, %i, %i) not available!", m_key.first, m_key.second.first, m_key.second.second );
         return;
      }

     //Info("TrigConfigSvcD3PD::GenerateSignatureSequenceMap", "Forming trigger configuration helper maps for (%i, %i, %i)",
     //    m_key.first, m_key.second.first, m_key.second.second);

     std::map< DBKeys_t, std::map< unsigned int , unsigned int > >::iterator chainIndex = m_chainCounterMap.find( m_key );

     map->second.clear();

      // For every configured chain
      for (UInt_t c = 0; c < GetChainN(); ++c) {
         if (GetChainLevel(c) > 1) chainIndex->second.insert( std::pair<unsigned int, unsigned int>(GetChainCounter(c), c) );
         // Look at all signatures run by this chain
         for (UInt_t s = 0; s < GetSigN(c); ++s) {
            // How many output TEs are there for this signature?
            UInt_t nOutputTEs = GetSigNOutputTE(c, s);
            // For each of these output TEs
            for (UInt_t t = 0; t < nOutputTEs; ++t) {
               // Get ID of the output TE
               unsigned int outputTE = GetSigOutputTE(c, s, t);
               bool isMatched = false;
               unsigned int inputTE = UINT_MAX;
               // For every configured sequence
               for(unsigned int l = 0; l < GetSeqN(); ++l) {
                  // Is this sequence's Input TE the same as the Output TE we're currently investigating?
                  if(GetSeqID(l) == outputTE) {
                     // Yes! We have a match.
                     if(isMatched == true) {
                        // Hmm, we already matched this TE. Odd, this should not happen.
                        Warning("TrigConfigSvcD3PD::GenerateSignatureSequenceMap",
                          "Resolved two InputTEs (%s and %s) for OutputTE #%i of %s",
                          GetSeqName(l).c_str(), GetSeqName(inputTE).c_str(), t, GetSigLabel(c, s).c_str() );
                        break;
                     }
                     isMatched = true;
                     // Make note of the match.
                     inputTE = l;
                  }
               }
               // If we were unable to find a match then print a warning.
               // Corrupted or incomplete trigger configuration?
               if(isMatched == 0) {
                 Warning("TrigConfigSvcD3PD::GenerateSignatureSequenceMap",
                   "Unable to find InputTE for OutputTE #%i of %s (%i)",
                   t, GetSigLabel(c, s).c_str(), outputTE );
               } else {
                  // Debug print the mapping. Can be left commented out most of the time to avoid terminal spam.
                  //Info("TrigConfigSvcD3PD::GenerateSignatureSequenceMap", "Debug: Linked OutputTE #%i of %s to %s [ %i -> %i ]",
                  //     t, GetSigLabel(c, s).c_str(), GetSeqName(inputTE).c_str(), outputTE, inputTE);
               }
               // Record this mapping
               map->second.insert( std::pair<unsigned int, unsigned int>(outputTE,inputTE) );
            }
         }
      }

      std::map< DBKeys_t, std::map< unsigned int , std::string > >::iterator seqMap = m_hltSequenceNameMap.find( m_key );
      std::map< DBKeys_t, std::map< std::pair< int, int > , std::string > >::iterator algNameMap = m_hltAlgorithmNameMap.find( m_key );
      std::map< DBKeys_t, std::map< std::pair< int, int > , unsigned int > >::iterator algNameIDMap = m_hltAlgorithmNameIDMap.find( m_key );
      std::map< DBKeys_t, std::map< std::pair< int, int > , std::string > >::iterator algTypeMap = m_hltAlgorithmTypeMap.find( m_key );
      std::map< DBKeys_t, std::map< std::pair< int, int > , unsigned int > >::iterator algTypeIDMap = m_hltAlgorithmTypeIDMap.find( m_key );
      seqMap->second.clear();
      algNameMap->second.clear();
      algNameIDMap->second.clear();
      algTypeMap->second.clear();
      algTypeIDMap->second.clear();

      // For all sequences
      for(unsigned int s = 0; s < GetSeqN(); ++s) {
         // Map sequence index to sequence name
         seqMap->second.insert( std::pair<unsigned int, std::string>( GetSeqIndex(s), GetSeqName(s)) );
         // For all sequence algs
         for(unsigned int a = 0; a < GetAlgN(s); ++a) {
            std::pair<int,int> algID( (int)GetSeqIndex(s) , (int)GetAlgPosition(s,a) );
            // Map algorithm name, name ID, class name and class name ID to location ID (combination of sequence ID and algorithm position).
            algNameMap  ->second.insert( std::pair< std::pair< int, int > , std::string > ( algID , GetAlgName(s,a) ) );
            algNameIDMap->second.insert( std::pair< std::pair< int, int > , unsigned int >( algID , GetAlgNameID(s,a) ) );
            algTypeMap  ->second.insert( std::pair< std::pair< int, int > , std::string > ( algID , GetAlgTypeName(s,a) ) );
            algTypeIDMap->second.insert( std::pair< std::pair< int, int > , unsigned int >( algID , GetAlgTypeID(s,a) ) );

         }
      }

   }


} // namespace D3PD
