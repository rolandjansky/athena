// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ReadStats.h 642099 2015-01-27 16:43:18Z krasznaa $
#ifndef XAODCORE_TOOLS_READSTATS_H
#define XAODCORE_TOOLS_READSTATS_H

// System include(s):
#include <map>
#include <vector>
#include <string>

// ROOT include(s):
#include <TNamed.h>

// EDM include(s):
#ifndef __MAKECINT__
#ifndef __ROOTCLING__
#   include "AthContainersInterfaces/AuxTypes.h"
#endif // not __ROOTCLING__
#endif // not __MAKECINT__

// Forward declaration(s):
class TCollection;
class TTree;
class TH1;

namespace xAOD {

   /// Class describing the access statistics of one (sub-)branch
   ///
   /// Objects of this class are used to describe a single xAOD branch's
   /// access pattern.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 642099 $
   /// $Date: 2015-01-27 17:43:18 +0100 (Tue, 27 Jan 2015) $
   ///
   class BranchStats : public ::TNamed {

   public:
      /// Constructor specifying all parameters
      BranchStats( const char* name = "xAODBranch",
                   const char* type = "Int_t",
                   ::Int_t nTreesAccessed = 0, ::Long64_t nReadEntries = 0,
                   ::Long64_t nUnzippedBytes = 0, ::Long64_t nZippedBytes = 0 );
      /// Copy constructor
      BranchStats( const BranchStats& parent );

      /// Assignment operator
      BranchStats& operator= ( const BranchStats& parent );

      /// @name Accessor/setter functions for the kept information
      /// @{

      /// Get how many trees/files were accessed to read this branch
      ::Int_t treesAccessed() const;
      /// Set how many trees were accessed to read this branch
      void setTreesAccessed( ::Int_t nTreesAccessed );

      /// Get how many entries were read from this branch
      ::Long64_t readEntries() const;
      /// Set how many entries were read from this branch
      void setReadEntries( ::Long64_t nReadEntries );

      /// Get how many unzipped bytes were read from this branch in total
      ::Long64_t unzippedBytesRead() const;
      /// Set how many unzipped bytes were read from this branch in total
      void setUnzippedBytesRead( ::Long64_t nUnzippedBytes );

      /// Get how many zipped bytes were read from this branch in total
      ::Long64_t zippedBytesRead() const;
      /// Set how many zipped bytes were read from this branch in total
      void setZippedBytesRead( ::Long64_t nZippedBytes );

      /// @}

      /// Function merging two objects
      BranchStats& add( const BranchStats& rh );
      /// Operator merging two objects
      BranchStats& operator+= ( const BranchStats& rh );

      /// @name "ROOT-specific" functions
      /// @{

      /// Function merging the information from multiple objects
      ::Int_t Merge( ::TCollection* coll );

      /// Print information about the collected statistics
      void Print( ::Option_t* option = "" ) const;

      /// @}

   private:
      /// Number of trees/files accessed for this branch
      ::Int_t m_treesAccessed;
      /// Number of entries read from this branch
      ::Long64_t m_readEntries;
      /// Number of unzipped bytes read from this branch
      ::Long64_t m_unzippedBytes;
      /// Number of zipped bytes read from this branch
      ::Long64_t m_zippedBytes;

      ClassDef( xAOD::BranchStats, 1 )

   }; // class BranchStats

   /// Class describing the access statistics of a collection of branches
   ///
   /// Objects of this class are used to describe the access pattern of an xAOD
   /// analysis. The object can also be used to optimize the caching for the
   /// analysis in subsequent runs.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 642099 $
   /// $Date: 2015-01-27 17:43:18 +0100 (Tue, 27 Jan 2015) $
   ///
   class ReadStats : public ::TNamed {

   public:
      /// Type of the internal vectors describing one auxiliary store
      typedef std::vector< BranchStats* > Vector_t;
      /// Type of the internal object gathering information on aux branches
      typedef std::map< std::string, Vector_t > Map_t;

      /// Type of the internal object gathering information on containers
      typedef std::map< std::string, BranchStats > MapC_t;

      /// Constructor just specifying the name of the object
      ReadStats( const char* name = "xAODReadStats",
                 const char* title = "xAOD reading statistics" );
      /// Copy constructor
      ReadStats( const ReadStats& parent );
      /// Destructor
      ~ReadStats();

      /// Assignment operator
      ReadStats& operator= ( const ReadStats& parent );

      /// Clear the statistics information (inherited from TNamed...)
      void Clear( ::Option_t* opt = "" );

      /// @name Raw information accessor/setter functions
      /// @{

      /// Set how many bytes were read in total during the analysis
      void setBytesRead( ::Long64_t num );
      /// Get how many bytes were read in total during the analysis
      ::Long64_t bytesRead() const;

      /// Set the total number of branches on the input
      void setBranchNum( ::Int_t num );
      /// Get the total number of branches on the input
      ::Int_t branchNum() const;

      /// Set the total number of file read calls
      void setFileReads( ::Int_t num );
      /// Get the total number of file read calls
      ::Int_t fileReads() const;

      /// Set the TTreeCache size used
      void setCacheSize( ::Int_t size );
      /// Get the TTreeCache size used
      ::Int_t cacheSize() const;

      /// Set the time spent in reading the data
      void setReadTime( ::Double_t time );
      /// Get the time spent in reading the data
      ::Double_t readTime() const;

      /// Set the time spent in unzipping the data
      void setUnzipTime( ::Double_t time );
      /// Get the time spent in unzipping the data
      ::Double_t unzipTime() const;

      /// Set the time spent in processing events
      void setProcessTime( ::Double_t time );
      /// Get the time spent in processing events
      ::Double_t processTime() const;

      /// @}

      /// @name Functions used for filling the object
      /// @{

#ifndef __MAKECINT__
#ifndef __ROOTCLING__
      /// Access the description of a branch. Creating it if necessary.
      BranchStats* branch( const std::string& prefix,
                           SG::auxid_t auxid );
      /// Get information about a specific variable
      const BranchStats* branch( const std::string& prefix,
                                 SG::auxid_t auxid ) const;
#endif // not __ROOTCLING__
#endif // not __MAKECINT__

      /// Access the description of a container. Creating it if necessary.
      BranchStats* container( const std::string& name );
      /// Access the description of a container.
      const BranchStats* container( const std::string& name ) const;

      /// Get all variable information
      const Map_t& branches() const;
      /// Get information about all the containers
      const MapC_t& containers() const;

      /// @}

      /// @name Function used for merging objects of this type
      /// @{

      /// Function checking if two objects are "compatible"
      bool isCompatible( const ReadStats& rh ) const;
      /// Function merging two objects
      ReadStats& add( const ReadStats& rh );
      /// Operator merging two objects
      ReadStats& operator+= ( const ReadStats& rh );

      /// Function merging the information from multiple objects
      ::Int_t Merge( ::TCollection* coll );

      /// @}

      /// @name Function for setting up a TTreeCache object
      /// @{

      /// Add all branches that were ever accessed, to the TTreeCache
      void addToTreeCache( ::TTree* tree ) const;
      /// Add the branches accessed more than n times to the TTreeCache
      void addToTreeCacheByEntries( ::TTree* tree,
                                    ::Long64_t minEntries ) const;
      /// Add the branches accessed more than a given fraction of times to the
      /// TTreeCache
      void addToTreeCacheByEntryFrac( ::TTree* tree,
                                      ::Double_t minEvFraction ) const;
      /// Add the branches from which more than x bytes were read, to the
      /// TTreeCache
      void addToTreeCacheByBytes( ::TTree* tree, ::Long64_t minBytes ) const;
      /// Add the branches from whith more than a given fraction of the bytes
      /// were read, to the TTreeCache
      void addToTreeCacheByByteFrac( ::TTree* tree,
                                     ::Double_t minByteFraction ) const;

      /// @}

      /// @name Functions accessing branch names according to certain rules
      /// @{

      /// Get the branches accessed more than n times
      std::vector< std::string >
      branchesByEntries( ::Long64_t minEntries ) const;
      /// Get the branches accessed more than a given fraction of times
      std::vector< std::string >
      branchesByEntryFrac( ::Double_t minEvFraction ) const;
      /// Get the branches from which more than x bytes were read
      std::vector< std::string >
      branchesByBytes( ::Long64_t minBytes ) const;
      /// Get the branches from which more than a given fraction of bytes were
      /// read
      std::vector< std::string >
      branchesByByteFrac( ::Double_t minByteFraction ) const;

      /// @}

      /// @name Functions getting the statistics as histograms
      /// @{

      /// Get a histogram with the TTree access statistics
      ::TH1* treeAccessStat() const;
      /// Get a histogram with the entry reading statistics
      ::TH1* entryReadStat() const;
      /// Get a histogram with the zipped byte reading statistics
      ::TH1* zippedByteReadStat() const;
      /// Get a histogram with the unzipped byte reading statistics
      ::TH1* unzippedByteReadStat() const;

      /// @}

      /// Print information about the collected statistics
      void Print( ::Option_t* option = "" ) const;

      /// Print the accessed variables, formatted for smart slimming
      void printSmartSlimmingBranchList( bool autoIncludeLinks = false ) const;

      /// Function incrementing the processed event counter
      void nextEvent();

      /// Function getting the total number of input event
      ::Long64_t nEvents() const;
      /// Function setting the total number of input events
      void setNEvents( ::Long64_t nevents );

#ifndef __MAKECINT__
#ifndef __ROOTCLING__
      /// Function incrementing the read counter on a specific branch
      void readBranch( const std::string& prefix,
                       SG::auxid_t auxid );
#endif // not __ROOTCLING__
#endif // not __MAKECINT__

      /// Function incrementing the read counter on a specific container
      void readContainer( const std::string& name );

   private:
      /// Full Statistics about the branches
      Map_t m_branches;
      /// Statistics about the containers
      MapC_t m_containers;

      /// Total number of bytes read
      ::Long64_t m_bytesRead;
      /// Total number of branches in the input xAOD TTree
      ::Int_t m_branchNum;
      /// Total number of file reading operations during the analysis
      ::Int_t m_fileReads;
      /// Cache size used in the analysis
      ::Int_t m_cacheSize;
      /// Time spent reading the events
      ::Double_t m_readTime;
      /// Time spent unzipping the events
      ::Double_t m_unzipTime;
      /// Time spent in processing the events
      ::Double_t m_processTime;

      /// Total num events
      ::Long64_t m_nEvents;
      /// Total processed events
      ::Long64_t m_nEventsProcessed;

      ClassDef( xAOD::ReadStats, 1 )

   }; // class ReadStats

} // namespace xAOD

#endif // XAODCORE_TOOLS_READSTATS_H
