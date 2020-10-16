/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RELATIONALCOLLECTION_DBLOCK_H
#define RELATIONALCOLLECTION_DBLOCK_H


#include "RelationalAccess/ISessionProxy.h"

namespace pool {
   namespace RelationalCollection {
      
      class DBLock
      {
     public:
        DBLock( coral::ISessionProxy& session, const std::string& collName );
        ~DBLock();
 
        /// set a global lock to serialize collection operations (creation etc)
        /// if XTrans is flase, the lock will be gone when transaction is committed
        /// if XTrans is true, the lock will be permanent
        void    lock( bool Xtrans );
 
        /// remove global lock
        void    unlock();

        /// set lock release behavior in the destructor
        void    setAutorelease( bool ar );
        
     private:
 
        /// Reference to current database access session.
        coral::ISessionProxy&   m_session;
 
        std::string             m_name;

        /// true if this object holds a database lock
        bool                    m_locked;

        /// if true (default) try to remove the lock in the destructor
        bool                    m_autorelease;

        /// ignore database lock if older than this limit (in sec)
        long long               m_lockIngoreAge;

        /// give up waiting for database access after this time (in sec)
        int                     m_maxWait;
      };
 
      
   }
}

#endif


