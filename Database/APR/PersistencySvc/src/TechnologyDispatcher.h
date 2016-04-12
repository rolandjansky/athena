/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_TECHNOLOGYDISPATCHER_H
#define INCLUDE_PERSISTENCYSVC_TECHNOLOGYDISPATCHER_H

#include <string>
#include <map>

namespace pool {

  // forward declarations
  class ITransaction;

  namespace PersistencySvc {

    // forward declarations
    class MicroSessionManager;
    class DatabaseRegistry;

    /** @class TechnologyDispatcher
     * 
     * TechnologyDispatcher is a class that delivers
     * the appropriate MicroSessionManager given a
     * technology.
     *
     */

    class TechnologyDispatcher {
    public:
      /// Constructor
      TechnologyDispatcher( DatabaseRegistry& registry,
			    ITransaction& transaction );

      /// Destructor
      ~TechnologyDispatcher();

      /// Returns the technology given a technology type.
      MicroSessionManager& microSessionManager( long technology );
      const MicroSessionManager& microSessionManager( long technology ) const;
      MicroSessionManager& microSessionManager( const std::string& technology );
      const MicroSessionManager& microSessionManager( const std::string& technology ) const;

      /// Disconnects from all the databases
      void disconnectAll();

    private:
      DatabaseRegistry&                             m_registry;
      ITransaction&                                 m_transaction;
      std::map< long, MicroSessionManager* >        m_technologyTypes;
      std::map< std::string, MicroSessionManager* > m_technologyNames;
    };
  }
}

#endif
