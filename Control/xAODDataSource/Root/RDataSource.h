// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODDATASOURCE_RDATASOURCE_H
#define XAODDATASOURCE_RDATASOURCE_H

// Local include(s).
#include "RDataSourceEvent.h"

// Framework include(s).
#include "xAODRootAccess/TStore.h"

// ROOT include(s).
#include <ROOT/RDataSource.hxx>
#include <ROOT/RStringView.hxx>
#include <TChain.h>

// System include(s).
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace xAOD {

   /// Data source for xAOD input files
   ///
   /// This data source can be used to allow @c ROOT::RDataFrame to process
   /// (D)xAOD inputs.
   ///
   /// @author Umesh Worlikar <Umesh.Dharmaji.Worlikar@cern.ch>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class RDataSource final : public ROOT::RDF::RDataSource {

   public:
      /// Constructor with the file name pattern
      RDataSource( std::string_view fileNameGlob,
                   std::string_view treeName = "CollectionTree" );
      /// Constructor with list of file names
      RDataSource( const std::vector< std::string >& fileNames,
                   std::string_view treeName = "CollectionTree" );
      /// Destructor
      ~RDataSource();

      /// Type describing the entry ranges of the input file(s)
      typedef std::vector< std::pair< ULong64_t, ULong64_t > > EntryRanges_t;

      /// @name Functions implemented from @c ROOT::RDF::RDataSource
      /// @{

      /// Set the number of threads/slots that the data source should use
      virtual void SetNSlots( unsigned int slots ) override final;
      /// Initialise the data source, before the start of the event loop
      virtual void Initialise() override final;
      /// Initialise one of the slots/threads
      virtual void
      InitSlot( unsigned int slot, ULong64_t firstEntry ) override final;

      /// Close the input file reading in one of the slots/threads
      virtual void FinaliseSlot( unsigned int slot ) override final;
      /// Finalise the data source, after the event loop
      virtual void Finalise() override final;

      /// Get the column/object names for the input file(s)
      virtual const std::vector< std::string >&
      GetColumnNames() const override final;
      /// Check if the dataset has a certain column/object
      virtual bool HasColumn( std::string_view name ) const override final;
      /// Get the type name of a given column/object
      virtual std::string
      GetTypeName( std::string_view column ) const override final;

      /// Get the entry ranges in the input file(s)
      virtual EntryRanges_t GetEntryRanges() override final;
      /// Set which entry a give slot/thread should be processing
      virtual bool
      SetEntry( unsigned int slot, ULong64_t entry ) override final;

      /// @}

      /// Set whether verbose output should be printed (for debugging)
      void setVerboseOutput( Bool_t value = kTRUE );
      /// Check whether verbose output is set up to be printed
      Bool_t isVerboseOutput() const;

   private:
      /// Return the type-erased vector of pointers to pointers to column values
      virtual Record_t
      GetColumnReadersImpl( std::string_view column,
                            const std::type_info& typeInfo ) override final;
      /// Fill the metadata variables
      void readInputMetadata();

      /// @name Arguments provided by the user
      /// @{

      /// Files to read
      std::vector< std::string > m_fileNames;
      /// Name of the event tree in the input files
      std::string m_treeName;
      /// Whether verbose output should be printed or not
      Bool_t m_verboseOutput;

      /// @}

      /// @name Variables valid right after construction
      /// @{

      /// Names of the columns/objects on the input
      std::vector< std::string > m_columnNames;
      /// The object name -> class name map
      std::unordered_map< std::string, std::string > m_classNameMap;

      /// @}

      /// @name Variables that become valid after calling @c Initialise()
      /// @{

      /// Optimal entry ranges to split the processing into
      EntryRanges_t m_entryRanges;

      /// @}

      /// @name Variables that become valid after calling @c SetNSlots(...)
      /// @{

      /// Chains used in the file I/O
      std::vector< std::unique_ptr< TChain > > m_chains;
      /// Event objects performing the file I/O
      std::vector< std::unique_ptr< RDataSourceEvent > > m_events;
      /// In-memory whiteboards used during the event loop
      std::vector< std::unique_ptr< TStore > > m_stores;

      /// @}

   }; // class RDataSource

} // namespace xAOD

#endif // XAODDATASOURCE_RDATASOURCE_H
