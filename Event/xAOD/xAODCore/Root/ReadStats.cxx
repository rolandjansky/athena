/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ReadStats.cxx 642099 2015-01-27 16:43:18Z krasznaa $

// System include(s):
#include <cstring>
#include <functional>
#include <algorithm>

// ROOT include(s):
#include <TCollection.h>
#include <TTree.h>
#include <TH1.h>

// EDM include(s):
#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/normalizedTypeinfoName.h"

// Local include(s):
#include "xAODCore/tools/ReadStats.h"
#include "xAODCore/tools/Utils.h"

ClassImp( xAOD::BranchStats )
ClassImp( xAOD::ReadStats )

namespace {

   /// Strict weak ordering based on the number of entries read from a branch
   ///
   /// This helper function is used together with the STL std::sort algorithm
   /// to sort the xAOD branches based on how many events were read in from
   /// them.
   ///
   /// @param b1 The first branch's access statistics
   /// @param b2 The second branch's access statistics
   /// @returns <code>true</code> if the first branch was accessed more times
   ///         than the second one. <code>false</code> otherwise.
   ///         If equal, then compare based on the branch name.
   ///
   bool sortByEntries( const xAOD::BranchStats& b1,
                       const xAOD::BranchStats& b2 ) {

      if ( b1.readEntries() > b2.readEntries() ) {
        return true;
      }
      else if ( b1.readEntries() < b2.readEntries() ) {
        return false;
      }

      return strcmp (b1.GetName(), b2.GetName()) < 0;
   }

   /// Strict weak ordering based on the number of bytes unpacked from a branch
   ///
   /// This helper function is used together with the STL std::sort algorithm
   /// to sort the xAOD branches based on how much data was unpacked from them.
   ///
   /// @param b1 The first branch's access statistics
   /// @param b2 The second branch's access statistics
   /// @returns <code>true</code> if the first variable unpacked more data
   ///          than the second one. <code>true</code> otherwise.
   ///
   bool sortByUnzippedBytes( const xAOD::BranchStats& b1,
                             const xAOD::BranchStats& b2 ) {

      return ( b1.unzippedBytesRead() > b2.unzippedBytesRead() );
   }

   /// Smart class for selecting branches for caching
   ///
   /// This class can be used to select branches for the cache which
   /// were accessed more than some number of times in a previous job.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 642099 $
   /// $Date: 2015-01-27 17:43:18 +0100 (Tue, 27 Jan 2015) $
   ///
   class SelectByEntries :
      public std::unary_function< const xAOD::BranchStats&, bool > {

   public:
      /// Constructor specifying the minimum number of entries
      SelectByEntries( ::Long64_t entries ) : m_entries( entries ) {}
      /// Constructor finding the minimum number of entries
      SelectByEntries( const xAOD::ReadStats::Map_t& branches,
                       ::Double_t minEvFrac )
         : m_entries( 0 ) {

         ::Long64_t maxEntries = 0;
         xAOD::ReadStats::Vector_t::const_iterator vitr, vend;
         xAOD::ReadStats::Map_t::const_iterator itr = branches.begin();
         xAOD::ReadStats::Map_t::const_iterator end = branches.end();
         for( ; itr != end; ++itr ) {
            vitr = itr->second.begin();
            vend = itr->second.end();
            for( ; vitr != vend; ++vitr ) {
               if( ! *vitr ) continue;
               if( ( *vitr )->readEntries() > maxEntries ) {
                  maxEntries = ( *vitr )->readEntries();
               }
            }
         }

         m_entries = static_cast< ::Long64_t >( minEvFrac * maxEntries );
      }

      /// Operator evaluating if a variable should be selected
      result_type operator()( argument_type var ) const {

         if( var.readEntries() < m_entries ) {
            return false;
         } else {
            return true;
         }
      }

   private:
      ::Long64_t m_entries; ///< Minimum number of entries for the branch

   }; // class SelectByEntries

   /// Smart class for selecting branches for caching
   ///
   /// This class can be used to select branches for the cache from which
   /// more than some number bytes were read in a previous job.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 642099 $
   /// $Date: 2015-01-27 17:43:18 +0100 (Tue, 27 Jan 2015) $
   ///
   class SelectByBytes :
      public std::unary_function< const xAOD::BranchStats&, bool > {

   public:
      /// Constructor specifying the minimum number of bytes read
      SelectByBytes( ::Long64_t bytes ) : m_bytes( bytes ) {}

      /// Constructor finding the minimum number of bytes read
      SelectByBytes( const xAOD::ReadStats::Map_t& branches,
                     ::Double_t minByteFrac )
         : m_bytes( 0 ) {

         ::Long64_t maxBytes = 0;
         xAOD::ReadStats::Vector_t::const_iterator vitr, vend;
         xAOD::ReadStats::Map_t::const_iterator itr = branches.begin();
         xAOD::ReadStats::Map_t::const_iterator end = branches.end();
         for( ; itr != end; ++itr ) {
            vitr = itr->second.begin();
            vend = itr->second.end();
            for( ; vitr != vend; ++vitr ) {
               if( ! *vitr ) continue;
               if( ( *vitr )->zippedBytesRead() > maxBytes ) {
                  maxBytes = ( *vitr )->zippedBytesRead();
               }
            }
         }

         m_bytes = static_cast< ::Long64_t >( minByteFrac * maxBytes );
      }

      /// Operator evaluating if a variable should be selected
      result_type operator()( argument_type var ) const {

         if( var.zippedBytesRead() < m_bytes ) {
            return false;
         } else {
            return true;
         }
      }

   private:
      ::Long64_t m_bytes; ///< Minimum number of bytes read from the branch

   }; // class SelectByBytes

   /// Function adding branches to the TTreeCache
   ///
   /// The code internally uses this function to select which branches to add
   /// to the TTreeCache.
   ///
   /// @param tree The TTree to enable the caching for
   /// @param branches All the xAOD branches we know about
   /// @param selector Functor selecting the branches to cache
   /// @returns The number of branches added to the cache
   ///
   template< class SELECTOR >
   int addToCache( ::TTree* tree, const xAOD::ReadStats::Map_t& branches,
                   const SELECTOR& selector ) {

      int result = 0;

      xAOD::ReadStats::Map_t::const_iterator itr = branches.begin();
      xAOD::ReadStats::Map_t::const_iterator end = branches.end();
      for( ; itr != end; ++itr ) {
         for( size_t auxid = 0; auxid < itr->second.size(); ++auxid ) {
            if( ! itr->second[ auxid ] ) continue;
            if( ! selector( *( itr->second[ auxid ] ) ) ) continue;
            if( tree ) {
               const std::string brName = itr->first +
                  SG::AuxTypeRegistry::instance().getName( auxid );
               tree->AddBranchToCache( brName.c_str(), kTRUE );
            }
            ++result;
         }
      }

      return result;
   }

   /// Function selecting branches with some condition
   /**
    * This function is used internally to select branch names.
    *
    * @param branches All the xAOD branches we know about
    * @param selector Functor selecting the variables
    * @returns The selected branch names
    */
   template< class SELECTOR >
   std::vector< std::string >
   getBranches( const xAOD::ReadStats::Map_t& branches,
                const SELECTOR& selector ) {

      // The result object:
      std::vector< std::string > result;

      // Iterate over all the variables:
      xAOD::ReadStats::Map_t::const_iterator itr = branches.begin();
      xAOD::ReadStats::Map_t::const_iterator end = branches.end();
      for( ; itr != end; ++itr ) {
         for( size_t auxid = 0; auxid < itr->second.size(); ++auxid ) {
            // Check if the object exists:
            if( ! itr->second[ auxid ] ) continue;
            // Ignore the ones failing the selection:
            if( ! selector( *( itr->second[ auxid ] ) ) ) continue;
            // Add this variable to the result:
            const std::string brName = itr->first +
               SG::AuxTypeRegistry::instance().getName( auxid );
            result.push_back( brName );
         }
      }

      return result;
   }

} // private namespace

namespace xAOD {

   /// The constructor can specify all properties of the object in one go. All
   /// the parameters take a default value, to make it possible to create the
   /// object without giving the constructor any parameters. (Needed by CINT.)
   ///
   /// @param name Name of the variable being described
   /// @param type Full type name of the variable being described
   /// @param nTreesAccessed Number of TTrees accessed to read this variable
   /// @param nReadEntries Number of GetEntry(...) operations performed on this
   ///                     branch
   /// @param nUnzippedBytes Number of bytes read after decompression
   /// @param nZippedBytes Number of bytes physically read from disk
   ///
   BranchStats::BranchStats( const char* name, const char* type,
                             ::Int_t nTreesAccessed, ::Long64_t nReadEntries,
                             ::Long64_t nUnzippedBytes, ::Long64_t nZippedBytes )
      : TNamed( name, type ), m_treesAccessed( nTreesAccessed ),
        m_readEntries( nReadEntries ), m_unzippedBytes( nUnzippedBytes ),
        m_zippedBytes( nZippedBytes ) {

   }

   /// It's probably not needed actually, but I wanted to be explicit about
   /// how the copy constructor should behave.
   ///
   /// @param parent The object that's being copied
   ///
   BranchStats::BranchStats( const BranchStats& parent )
      : TNamed( parent ), m_treesAccessed( parent.m_treesAccessed ),
        m_readEntries( parent.m_readEntries ),
        m_unzippedBytes( parent.m_unzippedBytes ),
        m_zippedBytes( parent.m_zippedBytes ) {

   }

   /// The assignment operator is quite an important piece of code, as it's
   /// used all the time when using such objects in STL containers.
   ///
   /// @param parent The object that's being copied
   /// @returns A reference to the copied object
   ///
   BranchStats& BranchStats::operator= ( const BranchStats& parent ) {

      // Check for self-assignment:
      if( &parent == this ) {
         return *this;
      }

      // Set the properties of TNamed:
      SetName( parent.GetName() );
      SetTitle( parent.GetTitle() );

      // Set the properties of this class:
      m_treesAccessed = parent.m_treesAccessed;
      m_readEntries   = parent.m_readEntries;
      m_unzippedBytes = parent.m_unzippedBytes;
      m_zippedBytes   = parent.m_zippedBytes;

      return *this;
   }

   ::Int_t BranchStats::treesAccessed() const {

      return m_treesAccessed;
   }

   void BranchStats::setTreesAccessed( ::Int_t nTreesAccessed ) {

      m_treesAccessed = nTreesAccessed;
      return;
   }

   ::Long64_t BranchStats::readEntries() const {

      return m_readEntries;
   }

   void BranchStats::setReadEntries( ::Long64_t nReadEntries ) {

      m_readEntries = nReadEntries;
      return;
   }

   ::Long64_t BranchStats::unzippedBytesRead() const {

      return m_unzippedBytes;
   }

   void BranchStats::setUnzippedBytesRead( ::Long64_t nUnzippedBytes ) {

      m_unzippedBytes = nUnzippedBytes;
      return;
   }

   ::Long64_t BranchStats::zippedBytesRead() const {

      return m_zippedBytes;
   }

   void BranchStats::setZippedBytesRead( ::Long64_t nZippedBytes ) {

      m_zippedBytes = nZippedBytes;
      return;
   }

   BranchStats& BranchStats::add( const BranchStats& rh ) {

      // Set the properties of TNamed:
      SetName( rh.GetName() );
      SetTitle( rh.GetTitle() );

      // Accumulate the gathered statistics:
      m_treesAccessed += rh.m_treesAccessed;
      m_readEntries   += rh.m_readEntries;
      m_unzippedBytes += rh.m_unzippedBytes;
      m_zippedBytes   += rh.m_zippedBytes;

      return *this;
   }

   BranchStats& BranchStats::operator+= ( const BranchStats& rh ) {

      return add( rh );
   }

   /// This function makes it possible to properly merge objects coming from
   /// PROOF workers, or to properly merge objects saved in ROOT files.
   ///
   /// @param coll A collection of xAOD::BranchStats objects
   /// @returns <code>0</code> in case of failure, a positive number
   ///          in case of success
   ///
   ::Int_t BranchStats::Merge( ::TCollection* coll ) {

      // Some security checks:
      if( ! coll ) return 0;
      if( coll->IsEmpty() ) return 0;

      // Iterate over the elements of the collection:
      ::TIter next( coll );
      ::TObject* obj = 0;
      ::Int_t result = 0;
      while( ( obj = next() ) ) {

         // Check that the element is of the right type:
         BranchStats* vobj = dynamic_cast< BranchStats* >( obj );
         if( ! vobj ) {
            Error( "Merge", "Unknown object type encountered: %s",
                   obj->ClassName() );
            return 0;
         }

         // Add this element to this object:
         add( *vobj );
         ++result;
      }

      // Return how many objects were merged into this one:
      return result;
   }

   /// Standard ROOT printing function. It prints all information about the
   /// statistics gathered about a single xAOD branch.
   ///
   /// The option parameter understands the following value(s):
   ///   - "All": Prints the most verbose information available
   ///
   /// @param option Possible options for the printing verbosity
   ///
   void BranchStats::Print( ::Option_t* option ) const {

      // Print the most basic info:
      Info( "Print", "Branch name \"%s\", type \"%s\", read %lli times",
            GetName(), GetTitle(), m_readEntries );

      // Print the access statistics only if requested:
      if( ! ::strcmp( option, "All" ) ) {
         Info( "Print", "  TTrees accessed for this branch: %i",
               m_treesAccessed );
         Info( "Print", "  Number of entries read         : %lli",
               m_readEntries );
         Info( "Print", "  Number of unzipped bytes read  : %lli",
               m_unzippedBytes );
         Info( "Print", "  Number of zipped bytes read    : %lli",
               m_zippedBytes );
      }

      return;
   }

   /// In order to be able to stream such objects, they have to have an
   /// explicit name. It can be specified using this constructor, but
   /// usually leaving the default is just fine.
   ///
   /// @param name Name for the object
   /// @param title Optional title for the object (not used for anything)
   ///
   ReadStats::ReadStats( const char* name, const char* title )
     : ::TNamed( name, title ), m_branches(), m_containers(),
        m_bytesRead( 0 ), m_branchNum( 0 ),
        m_fileReads( 0 ), m_cacheSize( 0 ), m_readTime( 0.0 ),
        m_unzipTime( 0.0 ), m_processTime( 0.0 ),
        m_nEvents( 0 ), m_nEventsProcessed ( 0 ) {

   }

   /// It's probably not needed actually, but I wanted to be explicit about
   /// how the copy constructor should behave.
   ///
   /// @param parent The object that's being copied
   ///
   ReadStats::ReadStats( const ReadStats& parent )
     : ::TNamed( parent ), m_branches( parent.m_branches ),
        m_containers(parent.m_containers),
        m_bytesRead( parent.m_bytesRead ), m_branchNum( parent.m_branchNum ),
        m_fileReads( parent.m_fileReads ), m_cacheSize( parent.m_cacheSize ),
        m_readTime( parent.m_readTime ),
        m_unzipTime( parent.m_unzipTime ),
        m_processTime( parent.m_processTime ), 
	     m_nEvents( parent.m_nEvents ),
	     m_nEventsProcessed( parent.m_nEventsProcessed ){

   }

   /// The destructor needs to clean up all the BranchStats objects that were
   /// created on the heap.
   ///
   ReadStats::~ReadStats() {

      Map_t::iterator mitr = m_branches.begin();
      Map_t::iterator mend = m_branches.end();
      for( ; mitr != mend; ++mitr ) {
         Vector_t::iterator vitr = mitr->second.begin();
         Vector_t::iterator vend = mitr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            delete *vitr;
         }
      }
   }

   /// This is probably not needed either, but again, I wanted to be
   /// explicit.
   ///
   /// @param parent The object that's being copied
   /// @returns A reference to the copied object
   ///
   ReadStats& ReadStats::operator= ( const ReadStats& parent ) {

      // Check for self-assignment:
      if( &parent == this ) {
         return *this;
      }

      // Set the properties of TNamed:
      SetName( parent.GetName() );
      SetTitle( parent.GetTitle() );

      // Set the properties of this class:
      m_branches         = parent.m_branches;
      m_containers       = parent.m_containers;
      m_bytesRead        = parent.m_bytesRead;
      m_branchNum        = parent.m_branchNum;
      m_fileReads        = parent.m_fileReads;
      m_cacheSize        = parent.m_cacheSize;
      m_readTime         = parent.m_readTime;
      m_unzipTime        = parent.m_unzipTime;
      m_processTime      = parent.m_processTime;
      m_nEvents          = parent.m_nEvents;
      m_nEventsProcessed = parent.m_nEventsProcessed;

      return *this;
   }

   void ReadStats::Clear( ::Option_t* ) {

      // Clear all accumulated statistics:
      m_branches.clear();
      m_containers.clear();
      m_bytesRead = 0;
      m_branchNum = 0;
      m_fileReads = 0;
      m_cacheSize = 0;
      m_readTime  = 0.0;
      m_unzipTime = 0.0;
      m_processTime = 0.0;
      m_nEvents = 0;
      m_nEventsProcessed = 0;

      return;
   }

   void ReadStats::setBytesRead( ::Long64_t num ) {

      m_bytesRead = num;
      return;
   }

   ::Long64_t ReadStats::bytesRead() const {

      return m_bytesRead;
   }

   void ReadStats::setBranchNum( ::Int_t num ) {

      m_branchNum = num;
      return;
   }

   ::Int_t ReadStats::branchNum() const {

      return m_branchNum;
   }

   void ReadStats::setFileReads( ::Int_t num ) {

      m_fileReads = num;
      return;
   }

   ::Int_t ReadStats::fileReads() const {

      return m_fileReads;
   }

   void ReadStats::setCacheSize( ::Int_t size ) {

      m_cacheSize = size;
      return;
   }

   ::Int_t ReadStats::cacheSize() const {

      return m_cacheSize;
   }

   void ReadStats::setReadTime( ::Double_t time ) {

      m_readTime = time;
      return;
   }

   ::Double_t ReadStats::readTime() const {

      return m_readTime;
   }

   void ReadStats::setUnzipTime( ::Double_t time ) {

      m_unzipTime = time;
      return;
   }

   ::Double_t ReadStats::unzipTime() const {

      return m_unzipTime;
   }

   void ReadStats::setProcessTime( ::Double_t time ) {

      m_processTime = time;
      return;
   }

   ::Double_t ReadStats::processTime() const {

      return m_processTime;
   }

   /// This function is used to access a branch's description for modification.
   /// The function creates the description object if it doesn't exist yet.
   ///
   /// @param prefix The prefix for the xAOD branch
   /// @param auxid The auxiliary ID of the variable belonging to the branch
   /// @returns A pointer to the object holding the access statistics
   ///
   BranchStats* ReadStats::branch( const std::string& prefix,
                                   SG::auxid_t auxid ) {

      Vector_t& vec = m_branches[ prefix ];
      // Check if it's big enough:
      if( vec.size() <= auxid ) {
         vec.resize( auxid + 1 );
      }
      // Check if the object exists already:
      if( ! vec[ auxid ] ) {
         // Construct its name, mangling needed for dynamic variables:
         const std::string brName = prefix +
            SG::AuxTypeRegistry::instance().getName( auxid );
         const std::type_info* brType =
            SG::AuxTypeRegistry::instance().getType( auxid );
         if( ! brType ) {
            Error( "branch", "Coudln't find type_info for aux ID %i",
                   static_cast< int >( auxid ) );
            return 0;
         }
         const std::string brTypeName = SG::normalizedTypeinfoName( *brType );

         // Construct the new object:
         vec[ auxid ] = new BranchStats( brName.c_str(), brTypeName.c_str() );
      }

      // Return the object:
      return vec[ auxid ];
   }

   /// Get the access statistics of a single xAOD branch
   ///
   /// @param prefix The prefix for the xAOD branch
   /// @param auxid The auxiliary ID of the variable belonging to the branch
   /// @returns A pointer to the object describing the access statistics,
   ///          or a null pointer if the branch is unknown
   ///
   const BranchStats* ReadStats::branch( const std::string& prefix,
                                         SG::auxid_t auxid ) const {

      // Search for the variable:
      Map_t::const_iterator itr = m_branches.find( prefix );
      if( itr != m_branches.end() ) {
         const Vector_t& vec = itr->second;
         if( ( vec.size() > auxid ) && vec[ auxid ] ) {
            return vec[ auxid ];
         }
      }

      // Return a null pointer if the object was not found:
      return 0;
   }

   BranchStats* ReadStats::container( const std::string& name ) {

      // If it doesn't exist yet, create it now:
      if( m_containers.find( name ) == m_containers.end() ) {
         // Give it a starting value:
         m_containers[ name ] = BranchStats( name.c_str(), "CONTAINER" );
      }

      // Return a pointer to the object:
      return &( m_containers[ name ] );
   }

   const BranchStats* ReadStats::container( const std::string& name ) const {

      // Try to find it:
      MapC_t::const_iterator itr = m_containers.find( name );
      if( itr != m_containers.end() ) {
         return &( itr->second );
      }

      // We didn't find it:
      return 0;
   }

   const ReadStats::Map_t& ReadStats::branches() const {

      return m_branches;
   }

   const ReadStats::MapC_t& ReadStats::containers() const {
      
      return m_containers;
   }

   /// This function checks whether two objects are "compatible" with
   /// each other. This just means that it checks whether it appears as
   /// if the two statistics objects would've been recorded from the same
   /// input type or not.
   ///
   /// @param rh Object to compare to this object
   /// @returns <code>true</code> if the two objects are compatible,
   ///          <code>false</code> otherwise
   ///
   bool ReadStats::isCompatible( const ReadStats& rh ) const {

      if( ( m_branchNum == rh.branchNum() ) &&
          ( m_cacheSize == rh.cacheSize() ) ) {
         return true;
      } else {
         return false;
      }
   }

   /// This function is used to merge the information from two objects.
   ///
   /// @param rh The object to be merged into this one
   /// @returns A reference to the merged object
   ///
   ReadStats& ReadStats::add( const ReadStats& rh ) {

      // Merge the variable statistics one by one:
      Map_t::const_iterator itr = rh.m_branches.begin();
      Map_t::const_iterator end = rh.m_branches.end();
      for( ; itr != end; ++itr ) {
         const Vector_t& rhvec = itr->second;
         Vector_t& vec = m_branches[ itr->first ];
         if( vec.size() < rhvec.size() ) {
            vec.resize( rhvec.size() );
         }
         for( size_t auxid = 0; auxid < rhvec.size(); ++auxid ) {
            if( ! rhvec[ auxid ] ) continue;
            if( ! vec[ auxid ] ) {
               vec[ auxid ] = new BranchStats();
            }
            vec[ auxid ]->add( *( rhvec[ auxid ] ) );
         }
      }

      // Sum up the simple statistics:
      m_bytesRead   += rh.m_bytesRead;
      m_fileReads   += rh.m_fileReads;
      m_readTime    += rh.m_readTime;
      m_unzipTime   += rh.m_unzipTime;
      m_processTime += rh.m_processTime;

      return *this;
   }

   ReadStats& ReadStats::operator+= ( const ReadStats& rh ) {

      return add( rh );
   }

   /// This function can be used to add all the branches that this object
   /// knows about, to the cache of the TTree given to the function.
   ///
   /// It can be useful for blankly enabling caching for all the variables
   /// that an xAOD analysis accessed previously.
   ///
   /// @param tree Tree for which the caching should be configured
   ///
   void ReadStats::addToTreeCache( ::TTree* tree ) const {

      // Add all the branches to the cache:
      const int counter = addToCache( tree, m_branches,
                                      SelectByEntries( 0 ) );

      // Let the user know what we did:
      Info( "addToTreeCache", "Added %i branches to the TTreeCache",
            counter );

      return;
   }

   /// This should be one of the most useful functions of this class. It can
   /// be used to selectively enable the caching for the branches which were
   /// accessed more than a specified number of times in a previous running.
   ///
   /// @param tree Tree for which the caching should be configured
   /// @param minEntries Minimum number of entries read from the variable to
   ///                   qualify for caching
   ///
   void ReadStats::addToTreeCacheByEntries( ::TTree* tree,
                                            ::Long64_t minEntries ) const {

      // Add the selected branches to the cache:
      const int counter = addToCache( tree, m_branches,
                                      SelectByEntries( minEntries ) );

      // Let the user know what we did:
      Info( "addToTreeCacheByEntries", "Added %i branches to the TTreeCache",
            counter );

      return;
   }

   /// This should be one of the most useful functions of this class. It can
   /// be used to selectively enable the caching for the branches which were
   /// accessed more than a specified fraction of times in a previous running.
   /// This is slightly more general than the version of this function expecting
   /// an absolute entry number.
   ///
   /// @param tree Tree for which the caching should be configured
   /// @param minEvFraction Minimum fraction of entries read from the branches
   ///                      that should be added to the cache
   ///
   void ReadStats::addToTreeCacheByEntryFrac( ::TTree* tree,
                                              ::Double_t minEvFraction ) const {

      // Add the selected branches to the cache:
      const int counter = addToCache( tree, m_branches,
                                      SelectByEntries( m_branches,
                                                       minEvFraction ) );

      // Let the user know what we did:
      Info( "addToTreeCacheByEntries", "Added %i branches to the TTreeCache",
            counter );

      return;
   }

   /// This function can be used to add all the variables to the cache
   /// from which more than some bytes were read in a previous running.
   ///
   /// @param tree Tree for which the caching should be configured
   /// @param minBytes Minimum number of bytes read from the variable to
   ///                 qualify for caching
   ///
   void ReadStats::addToTreeCacheByBytes( ::TTree* tree,
                                          ::Long64_t minBytes ) const {

      // Add the selected branches to the cache:
      const int counter = addToCache( tree, m_branches,
                                      SelectByBytes( minBytes ) );

      // Let the user know what we did:
      Info( "addToTreeCacheByBytes", "Added %i branches to the TTreeCache",
            counter );

      return;
   }

   /// This function can be used to add all the variables to the cache
   /// from which more than some fraction of the bytes were read in a
   /// previous running.
   ///
   /// @param tree Tree for which the caching should be configured
   /// @param minByteFraction Minimum fraction of bytes read from the
   ///                        variable to qualify for caching
   ///
   void
   ReadStats::addToTreeCacheByByteFrac( ::TTree* tree,
                                        ::Double_t minByteFraction ) const {

      // Add the selected branches to the cache:
      const int counter = addToCache( tree, m_branches,
                                      SelectByBytes( m_branches,
                                                     minByteFraction ) );

      // Let the user know what we did:
      Info( "addToTreeCacheByBytes", "Added %i branches to the TTreeCache",
            counter );

      return;
   }

   /// This function can be used to get a list of branch names that were
   /// accessed more than a specified number of times in the analysis.
   ///
   /// @param minEntries Minimum number of entries read from the variables
   /// @returns A list of branch names fulfilling the requirement
   ///
   std::vector< std::string >
   ReadStats::branchesByEntries( ::Long64_t minEntries ) const {

      return getBranches( m_branches, SelectByEntries( minEntries ) );
   }

   /// This function can be used to get a list of branch names that were
   /// accessed more than some fraction of times in the analysis.
   ///
   /// @param minEvFrac Minimum fraction of entries read from the variables
   /// @returns A list of branch names fulfilling the requirement
   ///
   std::vector< std::string >
   ReadStats::branchesByEntryFrac( ::Double_t minEvFrac ) const {

      return getBranches( m_branches,
                          SelectByEntries( m_branches, minEvFrac ) );
   }

   /// This function can be used to get a list of branch names from which
   /// more than a specified number of bytes were read in the analysis.
   ///
   /// @param minBytes Minimum number of bytes read from the variables
   /// @returns A list of branch names fulfilling the requirement
   ///
   std::vector< std::string >
   ReadStats::branchesByBytes( ::Long64_t minBytes ) const {

      return getBranches( m_branches, SelectByBytes( minBytes ) );
   }

   /// This function can be used to get a list of branch names from which
   /// more than a specified fraction of bytes were read in the analysis.
   ///
   /// @param minByteFraction Minimum fraction of bytes read from the
   ///                        variables
   /// @returns A list of branch names fulfilling the requirement
   ///
   std::vector< std::string >
   ReadStats::branchesByByteFrac( ::Double_t minByteFraction ) const {

      return getBranches( m_branches,
                          SelectByBytes( m_branches, minByteFraction ) );
   }

   /// This function can be used to produce a nice histogram showing how many
   /// TTrees were accessed by how many of the branches.
   ///
   /// Note that the caller is responsible for deleting the created histogram.
   ///
   /// @returns The histogram showing the distribution
   ///
   ::TH1* ReadStats::treeAccessStat() const {

      // Find the branch(es) which accessed the most trees:
      ::Long64_t maxTrees = 0;
      Vector_t::const_iterator vitr, vend;
      Map_t::const_iterator itr = m_branches.begin();
      Map_t::const_iterator end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            if( ( *vitr )->treesAccessed() > maxTrees ) {
               maxTrees = ( *vitr )->treesAccessed();
            }
         }
      }

      // Create the histogram:
      ::TH1* result = new ::TH1D( "TreeAccessStat",
                                  "TTree access statistics;TTrees;Branches",
                                  100, 0.0, ( ( ::Double_t ) maxTrees ) * 1.1 );

      // Fill the histogram by looping over the variables once more:
      itr = m_branches.begin();
      end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            result->Fill( ( ::Double_t ) ( *vitr )->treesAccessed() );
         }
      }

      return result;
   }

   /// This function can be used to produce a nice histogram showing how many
   /// entries were accessed by how many of the branches.
   ///
   /// Note that the caller is responsible for deleting the created histogram.
   ///
   /// @returns The histogram showing the distribution
   ///
   ::TH1* ReadStats::entryReadStat() const {

      // Find the branch(es) which accessed the most entries:
      ::Long64_t maxEntries = 0;
      Vector_t::const_iterator vitr, vend;
      Map_t::const_iterator itr = m_branches.begin();
      Map_t::const_iterator end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            if( ( *vitr )->readEntries() > maxEntries ) {
               maxEntries = ( *vitr )->readEntries();
            }
         }
      }

      // Create the histogram:
      ::TH1* result = new ::TH1D( "EntryAccessStat",
                                  "Entry access statistics;Entries;Branches",
                                  100, 0.0,
                                  ( ( ::Double_t ) maxEntries ) * 1.1 );

      // Fill the histogram by looping over the variables once more:
      itr = m_branches.begin();
      end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            result->Fill( ( ::Double_t ) ( *vitr )->readEntries() );
         }
      }

      return result;
   }

   /// This function can be used to produce a nice histogram showing how many
   /// raw bytes were accessed by how many of the branches.
   ///
   /// Note that the caller is responsible for deleting the created histogram.
   ///
   /// @returns The histogram showing the distribution
   ///
   ::TH1* ReadStats::zippedByteReadStat() const {

      // Find the branch(es) which accessed the most bytes:
      ::Long64_t maxBytes = 0;
      Vector_t::const_iterator vitr, vend;
      Map_t::const_iterator itr = m_branches.begin();
      Map_t::const_iterator end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            if( ( *vitr )->zippedBytesRead() > maxBytes ) {
               maxBytes = ( *vitr )->zippedBytesRead();
            }
         }
      }

      // Create the histogram:
      ::TH1* result = new ::TH1D( "ZippedByteAccessStat",
                                  "Zipped byte access statistics;Bytes;Branches",
                                  100, 0.0, ( ( ::Double_t ) maxBytes ) * 1.1 );

      // Fill the histogram by looping over the variables once more:
      itr = m_branches.begin();
      end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            result->Fill( ( ::Double_t ) ( *vitr )->zippedBytesRead() );
         }
      }

      return result;
   }

   /// This function can be used to produce a nice histogram showing how many
   /// uncompressed bytes were accessed by how many of the branches.
   ///
   /// Note that the caller is responsible for deleting the created histogram.
   ///
   /// @returns The histogram showing the distribution
   ///
   ::TH1* ReadStats::unzippedByteReadStat() const {

      // Find the branch(es) which accessed the most bytes:
      ::Long64_t maxBytes = 0;
      Vector_t::const_iterator vitr, vend;
      Map_t::const_iterator itr = m_branches.begin();
      Map_t::const_iterator end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            if( ( *vitr )->unzippedBytesRead() > maxBytes ) {
               maxBytes = ( *vitr )->unzippedBytesRead();
            }
         }
      }

      // Create the histogram:
      ::TH1* result =
           new ::TH1D( "UnzippedByteAccessStat",
                       "Unzipped byte access statistics;Bytes;Branches",
                       100, 0.0, ( ( ::Double_t ) maxBytes ) * 1.1 );

      // Fill the histogram by looping over the variables once more:
      itr = m_branches.begin();
      end = m_branches.end();
      for( ; itr != end; ++itr ) {
         vitr = itr->second.begin();
         vend = itr->second.end();
         for( ; vitr != vend; ++vitr ) {
            if( ! *vitr ) continue;
            result->Fill( ( ::Double_t ) ( *vitr )->unzippedBytesRead() );
         }
      }

      return result;
   }

   /// This function makes it possible to properly merge objects coming from
   /// PROOF workers, or from separate ROOT files.
   ///
   /// @param coll A collection of xAOD::ReadStats objects
   /// @returns <code>0</code> in case of failure, a positive number
   ///          in case of success
   ///
   ::Int_t ReadStats::Merge( ::TCollection* coll ) {

      // Some security checks:
      if( ! coll ) return 0;
      if( coll->IsEmpty() ) return 0;

      // Iterate over the elements of the collection:
      ::TIter next( coll );
      ::TObject* obj = 0;
      ::Int_t result = 0;
      while( ( obj = next() ) ) {

         // Check that the element is of the right type:
         ReadStats* dobj = dynamic_cast< ReadStats* >( obj );
         if( ! dobj ) {
            Error( "Merge", "Unknown object type encountered: %s",
                   obj->ClassName() );
            return 0;
         }

         // The compatibility of the objects is no longer checked. When
         // processing a large dataset, it's probable that the objects
         // created by the different PROOF workers will not be "compatible".

         // Add this element to this object:
         add( *dobj );
         ++result;
      }

      return result;
   }

   /// Standard ROOT printing function. It prints the gathered information
   /// about the variables accessed in an analysis.
   ///
   /// The option parameter understands the following value(s):
   ///   - "Summary": Only the summary information is printed.
   ///   - "ByEntries": Order the variables by the number of entries
   ///     read from them.
   ///   - "ByBytes": Order the variables by the number of bytes
   ///     read from them.
   ///
   /// @param option Possible options for the printing
   ///
   void ReadStats::Print( ::Option_t* option ) const {

      Info( "Print", "Printing xAOD I/O statistics" );

      // Create a temporary vector of the containers, so they can be ordered
      // if necessary:
      std::vector< BranchStats > conts;
      MapC_t::const_iterator cont_itr = m_containers.begin();
      MapC_t::const_iterator cont_end = m_containers.end();
      for( ; cont_itr != cont_end; ++cont_itr ) {
         conts.push_back( cont_itr->second );
      }

      // Create a temporary vector of the branches, so they can be ordered
      // if necessary:
      std::vector< BranchStats > vars;
      Vector_t::const_iterator vec_itr, vec_end;
      Map_t::const_iterator map_itr = m_branches.begin();
      Map_t::const_iterator map_end = m_branches.end();
      for( ; map_itr != map_end; ++map_itr ) {
         vec_itr = map_itr->second.begin();
         vec_end = map_itr->second.end();
         for( ; vec_itr != vec_end; ++vec_itr ) {
            if( ! *vec_itr ) continue;
            vars.push_back( **vec_itr );
         }
      }

      // Container and branch access summary
      if( ! ::strcmp( option, "SmartSlimming" ) ) {

         Info( "Print", " " );
         Info( "Print", "Smart Slimming Statistics" );
         Info( "Print", " " );

         const Double_t proFrac = ( ( Double_t ) m_nEventsProcessed /
                                    ( Double_t ) m_nEvents ) * 100.0;
         Info( "Print", "  Processed %lli events from %lli (%g%%)",
               m_nEventsProcessed, m_nEvents, proFrac );

         Info( "Print", "  Number of containers in on the input: %i",
               static_cast< int >( m_containers.size() ) );
         Info( "Print", "  Number of branches on the input: %i",
               static_cast< int >( vars.size() ) );

         // Sort the containers by number of accesses:
         std::sort( conts.begin(), conts.end(), sortByEntries );

         // Print which containers got accessed:
         Info( "Print", " " );
         Info( "Print", "Accessed containers:" );
         Info( "Print", " " );
         std::vector< BranchStats >::const_iterator itr = conts.begin();
         std::vector< BranchStats >::const_iterator end = conts.end();
         for( ; itr != end; ++itr ) {
            if( ! itr->readEntries() ) continue;
            itr->Print( option );
         }

         // Sort the branches by number of accesses:
         std::sort( vars.begin(), vars.end(), sortByEntries );

         // Print which branches got accessed:
         Info( "Print", " " );
         Info( "Print", "Accessed branches:" );
         Info( "Print", " " );
         itr = vars.begin();
         end = vars.end();
         for( ; itr != end; ++itr ) {
            if( ! itr->readEntries() ) continue;
            itr->Print( option );
         }
         Info( "Print", " " );

         // Let's exit at this point:
         return;
      }

      // Print the summary information:
      Info( "Print", "  Number of variables in the input xAOD : %i",
            m_branchNum );
      const Double_t proFrac = ( ( Double_t ) m_nEventsProcessed /
                                 ( Double_t ) m_nEvents ) * 100.0;
      Info( "Print", "  Processed events                      : "
            "%lli/%lli (%g%%)", m_nEventsProcessed, m_nEvents, proFrac );
      Info( "Print", "  TTreeCache size used                  : %s",
            Utils::sizeToString( m_cacheSize ).c_str() );
      Info( "Print", "  Total number of bytes read            : %s",
            Utils::sizeToString( m_bytesRead ).c_str() );
      Info( "Print", "  Data reading speed per process        : %s",
            ( std::abs( m_processTime ) > 0.0001 ?
              Utils::speedToString( m_bytesRead / m_processTime ).c_str() :
              "N/A" ) );
      Info( "Print", "  Total number of file read operations  : %i",
            m_fileReads );
      const ::Long64_t readInOneGo =
         ( ::Long64_t ) ( ( ::Double_t ) m_bytesRead /
                          ( ::Double_t ) m_fileReads );
      Info( "Print", "  Data read in one go (on average)      : %s",
            Utils::sizeToString( readInOneGo ).c_str() );
      Info( "Print", "  Cumulative time spent processing data : %s (%s/event)",
            Utils::timeToString( m_processTime ).c_str(),
            ( m_nEventsProcessed ?
              Utils::timeToString( m_processTime / m_nEventsProcessed ).c_str() :
              "N/A" ) );
      Info( "Print", "  Cumulative time spent reading data    : %s (%s/event)",
            Utils::timeToString( m_readTime ).c_str(),
            ( m_nEventsProcessed ?
              Utils::timeToString( m_readTime / m_nEventsProcessed ).c_str() :
              "N/A" ) );
      Info( "Print", "  Cumulative time spent unzipping data  : %s (%s/event)",
            Utils::timeToString( m_unzipTime ).c_str(),
            ( m_nEventsProcessed ?
              Utils::timeToString( m_unzipTime / m_nEventsProcessed ).c_str() :
              "N/A" ) );

      // If we just needed summary information, stop here:
      if( ! ::strcmp( option, "Summary" ) ) {
         return;
      }

      // Select the kind of ordering for the containers:
      if( ! ::strcmp( option, "ByEntries" ) ) {
         Info( "Print", "Containers, sorted by number of accesses:" );
         std::sort( conts.begin(), conts.end(), sortByEntries );
      } else if( ! ::strcmp( option, "ByBytes" ) ) {
         Info( "Print", "Containers, sorted by number of bytes read:" );
         std::sort( conts.begin(), conts.end(), sortByUnzippedBytes );
      } else {
         Info( "Print", "Containers, sorted by name:" );
      }

      // Print the statistics from each container:
      std::vector< BranchStats >::const_iterator itr = conts.begin();
      std::vector< BranchStats >::const_iterator end = conts.end();
      for( ; itr != end; ++itr ) {
         itr->Print( option );
      }

      // Select the kind of ordering for the variables:
      if( ! ::strcmp( option, "ByEntries" ) ) {
         Info( "Print", "Variables, sorted by number of accesses:" );
         std::sort( vars.begin(), vars.end(), sortByEntries );
      } else if( ! ::strcmp( option, "ByBytes" ) ) {
         Info( "Print", "Variables, sorted by number of bytes read:" );
         std::sort( vars.begin(), vars.end(), sortByUnzippedBytes );
      } else {
         Info( "Print", "Variables, sorted by name:" );
      }

      // Print the statistics from each variable:
      itr = vars.begin();
      end = vars.end();
      for( ; itr != end; ++itr ) {
         itr->Print( option );
      }

      return;
   }

   /// This is a tricky function. It prints the list of accessed variables in
   /// a way that can be copy-pasted directly into the ItemList of a derivation
   /// job. (Or into the C++ code of some analysis code.)
   ///
   /// @param autoIncludeLinks Since removing links from objects can cause
   ///                         problems for slow-merging in Athena, one can
   ///                         request all ElementLink variables to be added to
   ///                         the variable list, irrespective whether they're
   ///                         needed or not.
   ///
   void ReadStats::printSmartSlimmingBranchList( bool autoIncludeLinks ) const {

      /// Object used to collect the information
      std::map< ::TString, ::TString > items;

      // Collect all the containers that were accessed during the job:
      MapC_t::const_iterator cont_itr = m_containers.begin();
      MapC_t::const_iterator cont_end = m_containers.end();
      for( ; cont_itr != cont_end; ++cont_itr ) {
         // Skip non-accessed containers:
         if( ! cont_itr->second.readEntries() ) continue;
         // Remember the container:
         items[ cont_itr->first ] = "";
      }

      // Now look for variables in all these containers:
      Map_t::const_iterator br_itr = m_branches.begin();
      Map_t::const_iterator br_end = m_branches.end();
      for( ; br_itr != br_end; ++br_itr ) {
         // First check if any variables got accessed. It has an effect on what
         // to do with the ElementLink types.
         bool containerAccessed = false;
         Vector_t::const_iterator itr = br_itr->second.begin();
         Vector_t::const_iterator end = br_itr->second.end();
         for( ; itr != end; ++itr ) {
            if( ( *itr ) && ( *itr )->readEntries() ) {
               containerAccessed = true;
               break;
            }
         }
         // If none were accessed, abandon this container:
         if( ! containerAccessed ) continue;
         // Now collect all the variables:
         for( ; itr != end; ++itr ) {
            // Check if it's an ElementLink type:
            const bool linkType =
               ( ( *itr ) &&
                 ::TString( ( *itr )->GetTitle() ).Contains( "ElementLink" ) );
            // Skip non-existent, non-accessed, and not requested link
            // variables:
            if( ( ( ! *itr ) || ( ! ( *itr )->readEntries() ) ) &&
                ( ! ( autoIncludeLinks && linkType ) ) ) continue;
            // Extract the name of the container and the variable from the
            // branch's name:
            const ::TString brname = ( *itr )->GetName();
            const ::Size_t dotPos = brname.First( '.' );
            if( dotPos == ::kNPOS ) {
               // Ignore the unknown containers:
               continue;
            }
            const ::TString cname = brname( 0, dotPos + 1 );
            const ::TString vname = brname( dotPos + 1,
                                            brname.Length() - dotPos );
            // Access the current variable list:
            ::TString& vars = items[ cname ];
            // Add a dot if there are already variables defined:
            if( vars.Length() ) {
               vars.Append( '.' );
            }
            // Add this variable:
            vars.Append( vname );
         }
      }

      // Print the collected information:
      Info( "printSmartSlimmingBranchList", "ItemList to use:" );
      if( autoIncludeLinks ) {
         Info( "printSmartSlimmingBranchList", "(All link variables added)" );
      }
      std::map< ::TString, ::TString >::const_iterator itr = items.begin();
      std::map< ::TString, ::TString >::const_iterator end = items.end();
      for( ; itr != end; ++itr ) {
         if( itr->first.EndsWith( "Aux." ) ) {
            Info( "printSmartSlimmingBranchList", "  %s%s",
                  itr->first.Data(),
                  ( itr->second.Length() ? itr->second.Data() : "-" ) );
         } else {
            Info( "printSmartSlimmingBranchList", "  %s",
                  itr->first.Data() );
         }
      }

      // Return gracefully:
      return;
   }

   void ReadStats::nextEvent(){

      // event counters
      ++m_nEventsProcessed;
      return;
   }

   ::Long64_t ReadStats::nEvents() const {

      return m_nEvents;
   }

   void ReadStats::setNEvents( ::Long64_t nevents ) {

      m_nEvents = nevents;
      return;
   }

   void ReadStats::readBranch( const std::string& prefix,
                               SG::auxid_t auxid ){

      // Access the branch:
      BranchStats* stat = branch( prefix, auxid );
      // Increment its access counter:
      stat->setReadEntries( stat->readEntries() + 1 );
      return;
   }

   void ReadStats::readContainer( const std::string& name ){

      // Access the branch:
      BranchStats* stat = container( name );
      // Increment its access counter:
      stat->setReadEntries( stat->readEntries() + 1 );
      return;
   }

} // namespace D3PDReader
