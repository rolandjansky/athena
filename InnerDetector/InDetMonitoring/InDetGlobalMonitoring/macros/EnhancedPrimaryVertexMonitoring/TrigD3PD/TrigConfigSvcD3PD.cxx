/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigConfigSvcD3PD.cxx 339267 2011-01-07 16:30:10Z krasznaa $

// ROOT include(s):
#include <TTree.h>
#include <TChain.h>
#include <TBranch.h>
#include <TString.h>

// Local include(s):
#include "TrigConfigSvcD3PD.h"

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
   TrigConfigSvcD3PD::TrigConfigSvcD3PD( TTree* confTree )
      : TNamed( "TrigConfigSvcD3PD", "Trigger configuration service" ),
        m_tree( confTree ), m_initialized( kFALSE ), m_smk( 0 ), m_l1psk( 0 ),
        m_hltpsk( 0 ), m_lvl1NameMap( 0 ), m_lvl1PrescaleMap( 0 ),
        m_hltNameMap( 0 ), m_hltPrescaleMap( 0 ), m_hltRerunPrescaleMap( 0 ),
        m_hltPassthroughMap( 0 ), m_lowerChainNameMap( 0 ),
        m_b_smk( 0 ), m_b_l1psk( 0 ), m_b_hltpsk( 0 ),
        m_b_lvl1NameMap( 0 ), m_b_lvl1PrescaleMap( 0 ),
        m_dummy( "" ) {

      if( m_tree ) Initialize();
   }

   /**
    * The destructor tried to clean up after the object as good as it can.
    */
   TrigConfigSvcD3PD::~TrigConfigSvcD3PD() {

      Reset();
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
    * @returns <code>kTRUE</code> if the operation wa successful,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t TrigConfigSvcD3PD::SetConfigTree( TTree* confTree ) {

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
      m_tree = confTree;
      return Initialize();
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
    * @returns <code>kTRUE</code> if the operation wa successful,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t TrigConfigSvcD3PD::Load( Int_t SMK, Int_t L1PSK, Int_t HLTPSK ) {

      //
      // Make sure that the object is initialized:
      //
      if( ! Initialize() ) {
         Error( "Load", "Couldn't load configuration" );
         return kFALSE;
      }

      //
      // Make quick work if the correct entry is already loaded:
      //
      if( ( SMK == m_smk ) && ( L1PSK == m_l1psk ) && ( HLTPSK == m_hltpsk ) ) {
         return kTRUE;
      }

      //
      // Find which entry corresponds to this configuration:
      //
      DBKeys_t keys = MakeDBKeys( SMK, L1PSK, HLTPSK );
      std::map< DBKeys_t, Int_t >::const_iterator itr;
      if( ( itr = m_configKeyMap.find( keys ) ) == m_configKeyMap.end() ) {
         Error( "Load", "Configuration with the following keys not available:" );
         Error( "Load", TString::Format( "  SMK: %i, L1PSK: %i, HLTPSK: %i",
                                         SMK, L1PSK, HLTPSK ) );
         Error( "Load", "Available configurations are:" );
         std::map< DBKeys_t, Int_t >::const_iterator db_itr = m_configKeyMap.begin();
         std::map< DBKeys_t, Int_t >::const_iterator db_end = m_configKeyMap.end();
         for( ; db_itr != db_end; ++db_itr ) {
            Error( "Load", TString::Format( "  SMK: %i, L1PSK: %i, HLTPSK: %i",
                                            db_itr->first.first, db_itr->first.second.first,
                                            db_itr->first.second.second ) );
         }
         return kFALSE;
      }

      //
      // Load the requested entry:
      //
      Int_t bytes = 0;
      Int_t entry = itr->second;
      TChain* chain = dynamic_cast< TChain* >( m_tree );
      if( chain ) {
         entry = chain->LoadTree( itr->second );
      }
      if( m_b_smk )    bytes += m_b_smk->GetEntry( entry );
      if( m_b_l1psk )  bytes += m_b_l1psk->GetEntry( entry );
      if( m_b_hltpsk ) bytes += m_b_hltpsk->GetEntry( entry );

      if( m_b_lvl1NameMap )     bytes += m_b_lvl1NameMap->GetEntry( entry );
      if( m_b_lvl1PrescaleMap ) bytes += m_b_lvl1PrescaleMap->GetEntry( entry );

      if( m_b_hltNameMap )          bytes += m_b_hltNameMap->GetEntry( entry );
      if( m_b_hltPrescaleMap )      bytes += m_b_hltPrescaleMap->GetEntry( entry );
      if( m_b_hltRerunPrescaleMap ) bytes += m_b_hltRerunPrescaleMap->GetEntry( entry );
      if( m_b_hltPassthroughMap )   bytes += m_b_hltPassthroughMap->GetEntry( entry );

      if( m_b_lowerChainNameMap ) bytes += m_b_lowerChainNameMap->GetEntry( entry );

      //
      // Update the cached trigger names:
      //
      if( ! UpdateTriggerNames() ) {
         Error( "Load", "Failed to cache the configured trigger names" );
         return kFALSE;
      }

      Info( "Load", TString::Format( "Loaded configuration: SMK: %i, L1PSK: %i, HLTPSK: %i",
                                     SMK, L1PSK, HLTPSK ) );
      return kTRUE;
   }

   /**
    * This function checks whether a particular LVL1 item or HLT chain is available
    * in the currently loaded configuration.
    *
    * @param name Name of the LVL1 item or HLT chain
    * @returns <code>kTRUE</code> if the trigger is available in the configuration,
    *          <code>kFALSE</code> otherwise
    */
   Bool_t TrigConfigSvcD3PD::IsConfigured( const std::string& name ) const {

      // Do a small sanity check:
      if( ( ! m_hltNameMap ) || ( ! m_lvl1NameMap ) ) {
         Error( "IsConfigured", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Try to find it as an HLT chain (I guess this is a more common query):
      //
      if( m_hltNameMap->find( name ) != m_hltNameMap->end() ) {
         return kTRUE;
      }

      //
      // Now try to find it as a LVL1 item:
      //
      if( m_lvl1NameMap->find( name ) != m_lvl1NameMap->end() ) {
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
   Int_t TrigConfigSvcD3PD::GetCTPId( const std::string& name ) const {

      // Do a small sanity check:
      if( ! m_lvl1NameMap ) {
         Error( "GetCTPId", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Find the ID of the requested item:
      //
      std::map< std::string, int >::const_iterator itr;
      if( ( itr = m_lvl1NameMap->find( name ) ) == m_lvl1NameMap->end() ) {
         Warning( "GetCTPId", TString::Format( "LVL1 item not known: %s",
                                               name.c_str() ) );
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
   Int_t TrigConfigSvcD3PD::GetChainId( const std::string& name ) const {

      // Do a small sanity check:
      if( ! m_hltNameMap ) {
         Error( "GetChainId", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Find the ID of the requested chain:
      //
      std::map< std::string, int >::const_iterator itr;
      if( ( itr = m_hltNameMap->find( name ) ) == m_hltNameMap->end() ) {
         Warning( "GetChainId", TString::Format( "HLT chain not known: %s",
                                                 name.c_str() ) );
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
   Float_t TrigConfigSvcD3PD::GetPrescale( const std::string& name ) const {

      // Do a small sanity check:
      if( ( ! m_hltPrescaleMap ) || ( ! m_lvl1PrescaleMap ) ) {
         Error( "GetPrescale", "Configuration meta-tree not initialized yet!" );
         return kFALSE;
      }

      //
      // Check if this is an HLT chain:
      //
      std::map< std::string, float >::const_iterator itr;
      if( ( itr = m_hltPrescaleMap->find( name ) ) != m_hltPrescaleMap->end() ) {
         return itr->second;
      }

      //
      // If not, then it should be a LVL1 item:
      //
      if( ( itr = m_lvl1PrescaleMap->find( name ) ) != m_lvl1PrescaleMap->end() ) {
         return itr->second;
      }

      Warning( "GetPrescale", TString::Format( "Trigger not known: %s",
                                               name.c_str() ) );
      return -1.0;
   }

   /**
    * This function can return the rerun prescale of a given HLT chain. This is
    * usually only needed for in-depts trigger analyses.
    *
    * @param name The name of the HLT chain
    * @returns The rerun prescale value belonging to the specified HLT chain
    */
   Float_t TrigConfigSvcD3PD::GetRerunPrescale( const std::string& name ) const {

      //
      // Check if this information is available:
      //
      if( ! m_hltRerunPrescaleMap ) {
         Warning( "GetRerunPrescale", "The requested information is not available" );
         return -1.0;
      }

      //
      // Find the rerun prescale of the requested chain:
      //
      std::map< std::string, float >::const_iterator itr;
      if( ( itr = m_hltRerunPrescaleMap->find( name ) ) != m_hltRerunPrescaleMap->end() ) {
         return itr->second;
      }

      Warning( "GetRerunPrescale", TString::Format( "HLT chain now known: %s",
                                                    name.c_str() ) );
      return -1.0;
   }

   /**
    * This function can return the passthrough value set for a given HLT chain.
    * This is usually only needed for in-depth trigger analyses.
    *
    * @param name The name of the HLT chain
    * @returns The passthrough value belonging to the specified HLT chain
    */
   Float_t TrigConfigSvcD3PD::GetPassthrough( const std::string& name ) const {

      //
      // Check if this information is available:
      //
      if( ! m_hltPassthroughMap ) {
         Warning( "GetPassthrough", "The requested information is not available" );
         return -1.0;
      }

      //
      // Find the passthrough value of the requested chain:
      //
      std::map< std::string, float >::const_iterator itr;
      if( ( itr = m_hltPassthroughMap->find( name ) ) != m_hltPassthroughMap->end() ) {
         return itr->second;
      }

      Warning( "GetPassthrough", TString::Format( "HLT chain now known: %s",
                                                  name.c_str() ) );
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
   const std::string& TrigConfigSvcD3PD::GetLowerChainName( const std::string& name ) const {

      //
      // Check if this information is available:
      //
      if( ! m_lowerChainNameMap ) {
         Warning( "GetLowerChainName", "The requested information is not available" );
         return m_dummy;
      }

      //
      // Find the lower chain name of the requested chain:
      //
      std::map< std::string, std::string >::const_iterator itr;
      if( ( itr = m_lowerChainNameMap->find( name ) ) != m_lowerChainNameMap->end() ) {
         return itr->second;
      }

      Warning( "GetLowerChainName", TString::Format( "HLT chain now known: %s",
                                                     name.c_str() ) );
      return m_dummy;
   }

   /**
    * This function can be used to get a list of all the configured LVL1 trigger items
    * for the current configuration.
    *
    * @returns A list of all the configured LVL1 trigger item names
    */
   const std::vector< std::string >& TrigConfigSvcD3PD::GetLVL1Items() const {

      return m_confLVL1Items;
   }

   /**
    * This function can be used to get a list of all the configured HLT trigger chains
    * for the current configuration.
    *
    * @returns A list of all the configured HLT trigger chain names
    */
   const std::vector< std::string >& TrigConfigSvcD3PD::GetHLTChains() const {

      return m_confHLTChains;
   }

   /**
    * This function can be used to print some short summary about the trigger
    * configurations stored in a given D3PD file. Mostly just for debugging.
    */
   void TrigConfigSvcD3PD::PrintSummary() {

      //
      // Make sure that the object is initialized:
      //
      if( ! Initialize() ) {
         Error( "PrintSummary", "Couldn't load configuration" );
         return;
      }

      //
      // Print some minimal information about the available configurations:
      //
      Info( "PrintSummary", "The following configurations are available:" );
      std::map< DBKeys_t, Int_t >::const_iterator db_itr = m_configKeyMap.begin();
      std::map< DBKeys_t, Int_t >::const_iterator db_end = m_configKeyMap.end();
      for( ; db_itr != db_end; ++db_itr ) {
         if( ! Load( db_itr->first.first, db_itr->first.second.first,
                     db_itr->first.second.second ) ) {
            Fatal( "PrintSummary", "Couldn't load an apparently existing confgiuration!" );
            Fatal( "PrintSummary", "There is some serious problem with the code!" );
            return;
         }

         Info( "PrintSummary", TString::Format( "  SMK: %i, L1PSK: %i, HLTPSK: %i, "
                                                "#%i LVL1 items, #%i HLT chains",
                                                db_itr->first.first,
                                                db_itr->first.second.first,
                                                db_itr->first.second.second,
                                                m_lvl1NameMap->size(),
                                                m_hltNameMap->size() ) );
      }

      return;
   }

   /**
    * This function can be used to print quite detailed information about the currently
    * loaded trigger configuration. Mostly used for debugging.
    */
   void TrigConfigSvcD3PD::PrintConfig() {

      //
      // Make sure that the object is initialized:
      //
      if( ! Initialize() ) {
         Error( "PrintSummary", "Couldn't load configuration" );
         return;
      }

      //
      // Print the information about the loaded LVL1 items:
      //
      Info( "PrintConfig", "*** LVL1 configuration ***" );
      std::map< std::string, int >::const_iterator name_itr = m_lvl1NameMap->begin();
      std::map< std::string, int >::const_iterator name_end = m_lvl1NameMap->end();
      std::map< std::string, float >::const_iterator prescale_itr;
      for( ; name_itr != name_end; ++name_itr ) {
         if( ( prescale_itr = m_lvl1PrescaleMap->find( name_itr->first ) ) ==
             m_lvl1PrescaleMap->end() ) {
            Fatal( "PrintConfig",
                   "There is an internal inconsistency in the LVL1 configuration!" );
            return;
         }
         Info( "PrintConfig", "Name: %s, \tCTP ID: %i, \tPrescale: %f",
               name_itr->first.c_str(), name_itr->second, prescale_itr->second );
      }

      //
      // Print the information about the loaded HLT chains:
      //
      Info( "PrintConfig", "*** HLT configuration ***" );
      name_itr = m_hltNameMap->begin();
      name_end = m_hltNameMap->end();
      std::map< std::string, float >::const_iterator rprescale_itr;
      std::map< std::string, float >::const_iterator passthrough_itr;
      std::map< std::string, std::string >::const_iterator lowerChain_itr;
      for( ; name_itr != name_end; ++name_itr ) {
         if( ( ( prescale_itr = m_hltPrescaleMap->find( name_itr->first ) ) ==
               m_hltPrescaleMap->end() ) ||
             ( ( rprescale_itr = m_hltRerunPrescaleMap->find( name_itr->first ) ) ==
               m_hltRerunPrescaleMap->end() ) ||
             ( ( passthrough_itr = m_hltPassthroughMap->find( name_itr->first ) ) ==
               m_hltPassthroughMap->end() ) ||
             ( ( lowerChain_itr = m_lowerChainNameMap->find( name_itr->first ) ) ==
               m_lowerChainNameMap->end() ) ) {
            Fatal( "PrintConfig",
                   "There is an internal inconsistency in the HLT configuration!" );
            return;
         }
         Info( "PrintConfig", TString::Format( "Name: %s, \tChain ID: %i, \tPrescale: %f, "
                                               "\tRerun-prescale: %f, \tPassthrough: %f, "
                                               "\tLower chain name: %s",
                                               name_itr->first.c_str(), name_itr->second,
                                               prescale_itr->second, rprescale_itr->second,
                                               passthrough_itr->second,
                                               lowerChain_itr->second.c_str() ) );
      }

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
   Bool_t TrigConfigSvcD3PD::Initialize() {

      if( m_initialized ) return kTRUE;

      m_configKeyMap.clear();

      // Check if a metadata tree has already been defined:
      if( ! m_tree ) {
         Error( "Initialize", "No trigger configuration metadata TTree specified!" );
         return kFALSE;
      }

      // Check if we actually received a TChain, not a TTree:
      TChain* chain = dynamic_cast< TChain* >( m_tree );
      if( chain ) {
         // Just to make sure that the first TTree is loaded...
         if( ! chain->GetTree() ) {
            chain->LoadTree( 0 );
         }
      }

      //
      // Check whether the most basic branches exist in the TTree:
      //
      if( ( ! ( m_b_smk    = m_tree->GetBranch( "SMK" ) ) ) ||
          ( ! ( m_b_l1psk  = m_tree->GetBranch( "L1PSK" ) ) ) ||
          ( ! ( m_b_hltpsk = m_tree->GetBranch( "HLTPSK" ) ) ) ) {
         Error( "Initialize", "The supplied TTree doesn't seem to be a trigger "
                "configuration metadata tree!" );
         return kFALSE;
      }

      //
      // Connect these most basic branches:
      //
      m_tree->SetBranchAddress( "SMK",    &m_smk,    &m_b_smk );
      m_tree->SetBranchAddress( "L1PSK",  &m_l1psk,  &m_b_l1psk );
      m_tree->SetBranchAddress( "HLTPSK", &m_hltpsk, &m_b_hltpsk );

      //
      // Index the configuration entries:
      //
      for( Int_t i = 0; i < m_tree->GetEntries(); ++i ) {

         // If the user specified a TChain as the metadata tree, then we have to be
         // a bit smarter:
         if( chain ) {
            Long64_t entry = chain->LoadTree( i );
            if( entry < 0 ) break;
            m_b_smk->GetEntry( entry );
            m_b_l1psk->GetEntry( entry );
            m_b_hltpsk->GetEntry( entry );
         }
         // In case it's a simple TTree, then we don't have to be all that careful:
         else {
            m_b_smk->GetEntry( i );
            m_b_l1psk->GetEntry( i );
            m_b_hltpsk->GetEntry( i );
         }

         DBKeys_t keys = MakeDBKeys( m_smk, m_l1psk, m_hltpsk );
         m_configKeyMap[ keys ] = i;
      }

      //
      // Reset the branches to the first entry:
      //
      if( chain ) {
         chain->LoadTree( 0 );
      }
      m_b_smk->GetEntry( 0 );
      m_b_l1psk->GetEntry( 0 );
      m_b_hltpsk->GetEntry( 0 );

      //
      // Check whether the LVL1 branches exist:
      //
      if( ( ! ( m_b_lvl1NameMap = m_tree->GetBranch( "LVL1NameMap" ) ) ) ||
          ( ! ( m_b_lvl1PrescaleMap = m_tree->GetBranch( "LVL1PrescaleMap" ) ) ) ) {
         Error( "Initialize", "The supplied TTree doesn't hold the LVL1 branches" );
         Error( "Initialize", "This is VERY strange" );
         return kFALSE;
      }

      //
      // Connect the LVL1 branches:
      //
      m_tree->SetBranchAddress( "LVL1NameMap",     &m_lvl1NameMap,     &m_b_lvl1NameMap );
      m_tree->SetBranchAddress( "LVL1PrescaleMap", &m_lvl1PrescaleMap, &m_b_lvl1PrescaleMap );

      //
      // Reset the branches to the first entry:
      //
      m_b_lvl1NameMap->GetEntry( 0 );
      m_b_lvl1PrescaleMap->GetEntry( 0 );

      //
      // Check whether the basic HLT branches exist:
      //
      if( ( ! ( m_b_hltNameMap = m_tree->GetBranch( "HLTNameMap" ) ) ) ||
          ( ! ( m_b_hltPrescaleMap = m_tree->GetBranch( "HLTPrescaleMap" ) ) ) ) {
         Error( "Initialize", "The supplied TTree doesn't hold the HLT branches" );
         Error( "Initialize", "This is VERY strange" );
         return kFALSE;
      }

      //
      // Connect the basic HLT branches:
      //
      m_tree->SetBranchAddress( "HLTNameMap",     &m_hltNameMap,     &m_b_hltNameMap );
      m_tree->SetBranchAddress( "HLTPrescaleMap", &m_hltPrescaleMap, &m_b_hltPrescaleMap );

      //
      // Reset the branches to the first entry:
      //
      m_b_hltNameMap->GetEntry( 0 );
      m_b_hltPrescaleMap->GetEntry( 0 );

      //
      // Check whether the recently added HLT branches exist:
      //
      if( ( ! ( m_b_hltRerunPrescaleMap = m_tree->GetBranch( "HLTRerunPrescaleMap" ) ) ) ||
          ( ! ( m_b_hltPassthroughMap = m_tree->GetBranch( "HLTPassthroughMap" ) ) ) ||
          ( ! ( m_b_lowerChainNameMap = m_tree->GetBranch( "HLTLowerChainNameMap" ) ) ) ) {
         Warning( "Initialize", "The supplied TTree doesn't have some HLT branches" );
         Warning( "Initialize", "This is expected for slightly older D3PD files" );
      } else {
         //
         // Connect the extra HLT branches:
         //
         m_tree->SetBranchAddress( "HLTRerunPrescaleMap", &m_hltRerunPrescaleMap,
                                   &m_b_hltRerunPrescaleMap );
         m_tree->SetBranchAddress( "HLTPassthroughMap", &m_hltPassthroughMap,
                                   &m_b_hltPassthroughMap );
         m_tree->SetBranchAddress( "HLTLowerChainNameMap", &m_lowerChainNameMap,
                                   &m_b_lowerChainNameMap );

         //
         // Reset the branches to the first entry:
         //
         m_b_hltRerunPrescaleMap->GetEntry( 0 );
         m_b_hltPassthroughMap->GetEntry( 0 );
         m_b_lowerChainNameMap->GetEntry( 0 );
      }

      //
      // Update the cached trigger names:
      //
      if( ! UpdateTriggerNames() ) {
         Error( "Initialize", "Failed to cache the configured trigger names" );
         return kFALSE;
      }

      Info( "Initialize",
            TString::Format( "Loaded configuration: SMK: %i, L1PSK: %i, HLTPSK: %i",
                             m_smk, m_l1psk, m_hltpsk ) );

      m_initialized = kTRUE;
      return kTRUE;
   }

   /**
    * This function is used internally to clear the memory allocated by the object.
    */
   void TrigConfigSvcD3PD::Reset() {

      m_initialized = kFALSE;

      if( m_lvl1NameMap )     delete m_lvl1NameMap;     m_lvl1NameMap = 0;
      if( m_lvl1PrescaleMap ) delete m_lvl1PrescaleMap; m_lvl1PrescaleMap = 0;

      if( m_hltNameMap )          delete m_hltNameMap;          m_hltNameMap = 0;
      if( m_hltPrescaleMap )      delete m_hltPrescaleMap;      m_hltPrescaleMap = 0;
      if( m_hltRerunPrescaleMap ) delete m_hltRerunPrescaleMap; m_hltRerunPrescaleMap = 0;
      if( m_hltPassthroughMap )   delete m_hltPassthroughMap;   m_hltPassthroughMap = 0;

      if( m_lowerChainNameMap ) delete m_lowerChainNameMap; m_lowerChainNameMap = 0;

      m_b_smk = 0;
      m_b_l1psk = 0;
      m_b_hltpsk = 0;

      m_b_lvl1NameMap = 0;
      m_b_lvl1PrescaleMap = 0;

      m_b_hltNameMap = 0;
      m_b_hltPrescaleMap = 0;
      m_b_hltRerunPrescaleMap = 0;
      m_b_hltPassthroughMap = 0;

      m_b_lowerChainNameMap = 0;

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

      // Do a small sanity check:
      if( ( ! m_lvl1NameMap ) || ( ! m_hltNameMap ) ) {
         Error( "UpdateTriggerNames", "Basic trigger config branches not configured yet!" );
         return kFALSE;
      }

      // Clear the currently cached information:
      m_confLVL1Items.clear();
      m_confHLTChains.clear();

      // Cache the names of the LVL1 items:
      std::map< std::string, int >::const_iterator itr = m_lvl1NameMap->begin();
      std::map< std::string, int >::const_iterator end = m_lvl1NameMap->end();
      for( ; itr != end; ++itr ) {
         m_confLVL1Items.push_back( itr->first );
      }

      // Cache the names of the HLT chains:
      itr = m_hltNameMap->begin();
      end = m_hltNameMap->end();
      for( ; itr != end; ++itr ) {
         m_confHLTChains.push_back( itr->first );
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

} // namespace D3PD
