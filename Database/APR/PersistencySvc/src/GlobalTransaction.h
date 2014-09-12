/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_GLOBALTRANSACTION_H
#define INCLUDE_PERSISTENCYSVC_GLOBALTRANSACTION_H

#include "PersistencySvc/ITransaction.h"
#include "DatabaseRegistry.h"
#include <list>

namespace pool {
  namespace PersistencySvc {

    /** @class GlobalTransaction
     *
     *  GlobalTransaction is an implementation of the ITransaction interface.
     *
     */
    class GlobalTransaction : virtual public ITransaction {
    public:
      /// Constructor
      GlobalTransaction( DatabaseRegistry& registry );

      /// Destructor
      virtual ~GlobalTransaction();

      /// Starts a new transaction. Returns the success of the operation
      bool start( ITransaction::Type type = READ );

      /// Commits the transaction.
      bool commit();

      /// Commits and holds the transaction.
      bool commitAndHold();

      /// Rolls back the transaction
      void rollback();

      /// Checks if the transaction is active
      bool isActive() const;

      /// Reverts to update mode
      void update();

      /// Returns the transaction type
      ITransaction::Type type() const;

    private:
      /// The current transaction type
      ITransaction::Type m_type;

      /// The registered databases.
      DatabaseRegistry&  m_databases;
    };
  }
}

#endif
