/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_DATABASEREGISTRY_H
#define INCLUDE_PERSISTENCYSVC_DATABASEREGISTRY_H

#include <string>
#include <set>
#include <map>

namespace pool {
  
  class Context;
  
  namespace PersistencySvc {

    // Forward declarations
      //class PersistencySvcConfiguration;
    class DatabaseHandler;

    class DatabaseRegistry {
    public:
      /// Constructor
      explicit DatabaseRegistry( Context& context );

      /// Destructor
      ~DatabaseRegistry();

      // returns the POOL global context
      pool::Context& context();

      /// Registers a database.
      void registerDatabaseHandler( DatabaseHandler* dbHandler );

      /// Registers a database with an LFN
      void registerDatabaseHandler( DatabaseHandler* dbHandler,
				    const std::string& lfn );

      /// Deregisters a database.
      void deregisterDatabaseHandler( DatabaseHandler* dbHandler );

      /// Looks up a database by its file id
      DatabaseHandler* lookupByFID( const std::string& fid );

      /// Looks up a database by its physical file name
      DatabaseHandler* lookupByPFN( const std::string& pfn );

      /// Looks up a database by its logical file name
      DatabaseHandler* lookupByLFN( const std::string& lfn );

      typedef std::set< DatabaseHandler* >::iterator iterator;
      typedef std::set< DatabaseHandler* >::const_iterator const_iterator;

      /// The begin iterators
      iterator begin();
      const_iterator begin() const;

      /// The end iterators
      iterator end();
      const_iterator end() const;

      /// The number of elements registered
      std::size_t size() const;

    private:
      std::set< DatabaseHandler* >                     m_databases;
      std::map< std::string, DatabaseHandler* >        m_fidToDb;
      std::map< std::string, DatabaseHandler* >        m_pfnToDb;
      std::map< std::string, DatabaseHandler* >        m_lfnToDb;
      std::map< std::string, std::set< std::string > > m_fidToLfns;

      // Registers a database
      std::set< DatabaseHandler* >::iterator registerDbHandler( DatabaseHandler* dbHandler );

      // Pool global context
      Context& m_context;
    };
  }
}

#include "DatabaseRegistry.inl"

#endif
