//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "RDataSource.h"

// xAOD include(s).
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/tools/Message.h"

// ROOT include(s).
#include <TChain.h>
#include <TFile.h>
#include <TROOT.h>
#include <TError.h>

// System include(s).
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace {

   /// Convenience function for creating a chain
   std::unique_ptr< TChain >
   makeChain( const std::vector< std::string >& fileNames,
              std::string_view treeName ) {

      // Create the chain object.
      std::unique_ptr< TChain > chain;
      {
         R__LOCKGUARD( gROOTMutex );
         chain = std::make_unique< TChain >( treeName.data() );
      }

      // Set it up.
      chain->ResetBit( kMustCleanup );
      for( const std::string& fileName : fileNames ) {
         chain->Add( fileName.c_str() );
      }

      // Return the newly created chain.
      return chain;
   }

} // private namespace

namespace xAOD {

   /// Helper print operator
   template< typename FIRST, typename SECOND >
   std::ostream& operator<< ( std::ostream& out,
                              const std::pair< FIRST, SECOND >& pair ) {

      out << "[" << pair.first << ", " << pair.second << "]";
      return out;
   }

   /// Helper print operator
   template< typename T >
   std::ostream& operator<< ( std::ostream& out, const std::vector< T >& vec ) {

      out << "[";
      for( size_t i = 0; i < vec.size(); ++i ) {
         out << vec[ i ];
         if( i + 1 < vec.size() ) {
            out << ", ";
         }
      }
      out << "]";
      return out;
   }

   /// Helper print operator
   template< typename T >
   std::ostream& operator<< ( std::ostream& out,
                              const std::unordered_map< T, T >& umap ) {

      out << "{";
      bool first = true;
      for( auto& upair : umap ) {
         if( ! first ) {
            out << ", ";
         }
         out << upair.first << " : " << upair.second;
         first = false;
      }
      out << "}";
      return out;
   }

} // namespace xAOD

/// Helper macro for printing verbose messages for debugging
#define PRINT_VERBOSE(MSG)                                              \
   do {                                                                 \
      if( m_verboseOutput ) {                                           \
         std::cout << "xAOD::RDataSource VERBOSE " << MSG << std::endl; \
      }                                                                 \
   } while( false )

namespace xAOD {

   RDataSource::RDataSource( std::string_view fileNameGlob,
                             std::string_view treeName )
      : RDataSource( std::vector< std::string >( { fileNameGlob.data() } ),
                     treeName ) {

   }

   RDataSource::RDataSource( const std::vector< std::string >& fileNames,
                             std::string_view treeName )
      : m_fileNames( fileNames ), m_treeName( treeName ),
        m_verboseOutput( kFALSE ) {

      readInputMetadata();
   }

   RDataSource::~RDataSource() {

      // I don't understand why, but ROOT really doesn't like it if the
      // chains are not the first to be deleted from memory. :-/
      m_chains.clear();
   }

   void RDataSource::SetNSlots( unsigned int slots ) {

      // Some sanity checks.
      if( slots == 0 ) {
         ::Error( "xAOD::RDataSource::SetNSlots",
                  XAOD_MESSAGE( "Zero slots requested" ) );
         throw std::invalid_argument( "Zero slots requested" );
      }
      if( m_events.size() != 0 ) {
         ::Error( "xAOD::RDataSource::SetNSlots",
                  XAOD_MESSAGE( "Function called multiple times" ) );
         throw std::runtime_error( "Function called multiple times" );
      }

      // Reserve the correct number of elements.
      m_chains.reserve( slots );
      m_events.reserve( slots );
      m_stores.reserve( slots );
      PRINT_VERBOSE( "SetNSlots: Reserved " << slots
                     << " slots for the chains, events and stores" );

      // Create the event objects already at this point.
      for( unsigned int i = 0; i < slots; ++i ) {

         // Set up the chain, event and store.
         m_chains.push_back( ::makeChain( m_fileNames, m_treeName ) );
         m_events.push_back( std::make_unique< RDataSourceEvent >() );
         m_stores.push_back( std::make_unique< TStore >() );
         TChain* chain = m_chains.back().get();
         RDataSourceEvent* event = m_events.back().get();

         // Initialise the event object.
         if( ! event->readFrom( chain ).isSuccess() ) {
            ::Error( "xAOD::RDataSource::SetNSlots",
                     XAOD_MESSAGE( "Failed to set up xAOD::RDataSourceEvent "
                                   "for slot %u" ), i );
            throw std::runtime_error( "Failed to set up "
                                      "xAOD::RDataSourceEvent" );
         }

         // Load entry 0 for it. Notice that this is a waste of CPU and I/O
         // on the surface. But it's not... This triggers the initialisation of
         // the files/trees used by these chains. Which happens much more
         // quickly in a serial way in a single thread than in multiple threads
         // at the same time. To be followed up with the ROOT developers...
         if( event->getEntry( 0 ) < 0 ) {
            ::Error( "xAOD::RDataSource::SetNSlots",
                     XAOD_MESSAGE( "Failed to load entry 0 for slot %u" ), i );
            throw std::runtime_error( "Failed to load entry for slot" );
         }
         PRINT_VERBOSE( "SetNSlots: Initialised objects for slot " << i );
      }

      // Return gracefully.
      return;
   }

   void RDataSource::Initialise() {

      // A sanity check.
      if( m_entryRanges.size() != 0 ) {
         ::Fatal( "xAOD::RDataSource::Initialise",
                  XAOD_MESSAGE( "Function called on an initialised object" ) );
      }
      PRINT_VERBOSE( "Initialise: Initialising the data source" );

      // Create a chain that will help determine the optimal entry ranges
      // to process.
      auto chain = ::makeChain( m_fileNames, m_treeName );
      TObjArray* filesInChain = chain->GetListOfFiles();

      // Loop over the input files of the chain.
      Long64_t fileOffset = 0;
      for( Int_t ifile = 0; ifile < filesInChain->GetEntries(); ++ifile ) {

         // Open the file directly.
         const char* fileName = filesInChain->At( ifile )->GetTitle();
         auto file = std::unique_ptr< TFile >( TFile::Open( fileName,
                                                            "READ" ) );
         if( ( ! file ) || file->IsZombie() ) {
            ::Error( "xAOD::RDataSource::Initialise",
                     XAOD_MESSAGE( "Failed to open file: %s" ), fileName );
            throw std::runtime_error( "Failed to open file: " +
                                      std::string( fileName ) );
         }

         // Access the event tree inside of it.
         TTree* tree =
            dynamic_cast< TTree* >( file->Get( m_treeName.c_str() ) );
         if( ! tree ) {
            // A file with no event tree is not necessarily a problem. It could
            // just be a file that has no events left in it after all
            // selections.
            continue;
         }

         // Extract the ideal entry ranges from the file.
         const Long64_t entries = tree->GetEntries();
         TTree::TClusterIterator clusterIter( tree->GetClusterIterator( 0 ) );
         Long64_t clusterStart = 0;
         while( ( clusterStart = clusterIter() ) < entries ) {
            m_entryRanges.emplace_back( fileOffset + clusterStart,
                                        fileOffset +
                                        clusterIter.GetNextEntry() );
         }

         // Increment the file offset value.
         fileOffset += entries;
      }
      PRINT_VERBOSE( "Initialise: Created entry ranges: " << m_entryRanges );

      // Return gracefully.
      return;
   }

   void RDataSource::InitSlot( unsigned int slot, ULong64_t firstEntry ) {

      // A sanity check.
      if( m_events.size() <= slot ) {
         ::Error( "xAOD::RDataSource::InitSlot",
                  XAOD_MESSAGE( "Invalid slot (%u) received" ), slot );
         throw std::runtime_error( "Invalid slot received" );
      }

      // Load the first entry for it.
      if( m_events[ slot ]->getEntry( firstEntry ) < 0 ) {
         ::Error( "xAOD::RDataSource::InitSlot",
                  XAOD_MESSAGE( "Failed to load entry %lld for slot %u" ),
                  firstEntry, slot );
         throw std::runtime_error( "Failed to load entry for slot" );
      }
      PRINT_VERBOSE( "InitSlot: Retrieved entry " << firstEntry << " for slot "
                     << slot );

      // Activate and clear the store.
      m_stores[ slot ]->setActive();
      m_stores[ slot ]->clear();
      PRINT_VERBOSE( "InitSlot: Activated and cleared transient store for slot "
                     << slot );

      // Return gracefully.
      return;
   }

   void RDataSource::FinaliseSlot( unsigned int slot ) {

      // Simply print what's happening.
      PRINT_VERBOSE( "FinaliseSlot: Called for slot " << slot );

      // Return gracefully.
      return;
   }

   void RDataSource::Finalise() {

      // Simply print what's happening.
      PRINT_VERBOSE( "Finalise: Function called" );

      // Return gracefully.
      return;
   }

   const std::vector< std::string >& RDataSource::GetColumnNames() const {

      return m_columnNames;
   }

   bool RDataSource::HasColumn( std::string_view name ) const {

      return std::find( m_columnNames.begin(), m_columnNames.end(),
                        name ) != m_columnNames.end();
   }

   std::string RDataSource::GetTypeName( std::string_view column ) const {

      // Make sure that the column/object is known.
      if( ! HasColumn( column ) ) {
         ::Error( "xAOD::RDataSource::GetTypeName",
                  XAOD_MESSAGE( "Column/object \"%s\" not available" ),
                  column.data() );
         throw std::runtime_error( "Column/object \"" + std::string( column ) +
                                   "\" not available" );
      }

      // Get the type.
      auto itr = m_classNameMap.find( column.data() );
      if( itr == m_classNameMap.end() ) {
         // Note that the fatal message will abort the entire job in all cases.
         ::Fatal( "xAOD::RDataSource::GetTypeName",
                  XAOD_MESSAGE( "Internal logic error found" ) );
      }
      PRINT_VERBOSE( "GetTypeName: Type name for column \"" << column
                     << "\" is: " << itr->second );
      return itr->second;
   }

   RDataSource::EntryRanges_t RDataSource::GetEntryRanges() {

      // When ROOT asks for the entry ranges, we have to tell it which ones
      // have not been processed yet. Since we process all entries right away
      // (SetEntry(...) does not have logic for not processing a requested
      // entry), the logic here is to empty out the m_entryRanges variable on
      // this call. So that on the next call an empty range would be returned.
      const EntryRanges_t dummy( std::move( m_entryRanges ) );
      return dummy;
   }

   bool RDataSource::SetEntry( unsigned int slot, ULong64_t entry ) {

      // A sanity check.
      if( m_events.size() <= slot ) {
         ::Error( "xAOD::RDataSource::SetEntry",
                  XAOD_MESSAGE( "Invalid slot (%u) received" ), slot );
         throw std::runtime_error( "Invalid slot received" );
      }
      PRINT_VERBOSE( "SetEntry: Called for slot " << slot << " and entry "
                     << entry );

      // Switch to the requested entry.
      m_events[ slot ]->updateObjectsForEntry( entry );

      // Activate and clear the store.
      m_stores[ slot ]->setActive();
      m_stores[ slot ]->clear();

      // The entry is always processed.
      return true;
   }

   void RDataSource::setVerboseOutput( Bool_t value ) {

      m_verboseOutput = value;
      return;
   }

   Bool_t RDataSource::isVerboseOutput() const {

      return m_verboseOutput;
   }

   RDataSource::Record_t
   RDataSource::GetColumnReadersImpl( std::string_view column,
                                      const std::type_info& typeInfo ) {

      // Make sure that the column/object is known.
      if( ! HasColumn( column ) ) {
         ::Error( "xAOD::RDataSource::GetColumnReadersImpl",
                  XAOD_MESSAGE( "Column/object \"%s\" not available" ),
                  column.data() );
         throw std::runtime_error( "Column/object \"" + std::string( column ) +
                                   "\" not available" );
      }
      PRINT_VERBOSE( "GetColumnReadersImpl: Creating column readers for \""
                     << column << "/" << SG::normalizedTypeinfoName( typeInfo )
                     << "\"" );

      // Create the comlumn reader pointers.
      Record_t result( m_events.size() );
      for( size_t i = 0; i < m_events.size(); ++i ) {
         result[ i ] = m_events[ i ]->columnReader( column, typeInfo );
      }
      return result;
   }

   void RDataSource::readInputMetadata() {

      // Create a temporary event object.
      auto chain = ::makeChain( m_fileNames, m_treeName );
      RDataSourceEvent event;
      if( ! event.readFrom( chain.get() ).isSuccess() ) {
         ::Error( "xAOD::RDataSource::readInputMetadata",
                  XAOD_MESSAGE( "Failed to connect to the input chain" ) );
         throw std::runtime_error( "Failed to connect to the input chain" );
      }

      // Load the first event of the input, if one is available.
      if( event.getEntries() > 0 ) {
         if( event.getEntry( 0 ) < 0 ) {
            ::Error( "xAOD::RDataSource::readInputMetadata",
                     "Couldn't load the first event of the input" );
            throw std::runtime_error( "Couldn't load the first event of the "
                                      "input" );
         }
      }

      // Fill the column and type name variables.
      m_columnNames.clear(); m_classNameMap.clear();
      auto names = event.columnAndTypeNames();
      m_columnNames.reserve( names.size() );
      m_classNameMap.reserve( names.size() );
      for( const auto& pair : names ) {
         m_columnNames.push_back( pair.first );
         m_classNameMap[ pair.first ] = pair.second;
      }
      PRINT_VERBOSE( "readInputMetadata: m_columnNames = " << m_columnNames );
      PRINT_VERBOSE( "readInputMetadata: m_classNameMap = " << m_classNameMap );

      // ROOT memory management is weird... We must delete the chain first,
      // before the TEvent object on top of it would be deleted...
      chain.reset();

      // Return gracefully.
      return;
   }

} // namespace xAOD
