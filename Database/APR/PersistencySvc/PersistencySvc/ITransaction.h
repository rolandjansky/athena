/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INCLUDE_PERSISTENCYSVC_ITRANSACTION_H
#define INCLUDE_PERSISTENCYSVC_ITRANSACTION_H

/*
 *   POOL namespace declaration
 */
namespace pool {

  /** @class ITransaction ITransaction.h PersistencySvc/ITransaction.h
   *
   *  ITransaction is the interface class for user (macroscopic transactions)
   *  Every operation with the pool storage system should be performed
   *  within a transaction. It is up to the specific implementation to
   *  perform proper transaction handling (rollbacks, locking, etc...)
   */

  class ITransaction {
  protected:
    /// Default destructor
    virtual ~ITransaction() {}

  public:
    /// Transaction type enumeration
    typedef enum { UNDEFINED,
                   READ,
                   UPDATE } Type;

    /// Starts a new transaction. Returns the success of the operation
    virtual bool start( Type type = READ ) = 0;

    /// Commits the transaction.
    virtual bool commit() = 0;

    /// Commits the holds transaction.
    virtual bool commitAndHold() = 0;

    /// Rolls back the transaction
    virtual void rollback() = 0;

    /// Checks if the transaction is active
    virtual bool isActive() const = 0;

    /// Reverts to update mode
    virtual void update() = 0;

    /// Returns the transaction type
    virtual Type type() const = 0;
  };

}

#endif

