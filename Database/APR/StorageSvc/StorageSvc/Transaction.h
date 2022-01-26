/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//====================================================================
//  Transaction definition file
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//====================================================================
#ifndef POOL_TRANSACTION_H
#define POOL_TRANSACTION_H

/*
 *   POOL namespace declaration
 */
namespace pool  {

  /** @class Transaction Transaction.h POOLCore/Transaction.h
    *
    * Base class for Database Transactions
    *
    * @author  M.Frank
    * @version 1.0
    */
  class Transaction {
  public:
    enum Type {
      TRANSACT_DOM      = 1<<0,
      TRANSACT_DB       = 1<<1,
      TRANSACT_CNT      = 1<<2,
      TRANSACT_NONE     = 1<<4
    };
    enum Action {
      TRANSACT_START    = 1<<8,
      TRANSACT_ACTIVE   = 1<<9,
      TRANSACT_COMMIT   = 1<<10,
      TRANSACT_FLUSH    = 1<<11,
      TRANSACT_ROLLBACK = 1<<12,
      TRANSACT_ENDED    = 1<<13
    };
    enum Mode  {
      AUTOCOMMIT_OFF    = 1<<16,
      AUTOCOMMIT_ON     = 1<<17
    };
  protected:
    /// Technology type
    int m_technology;
  private:
    /// No copy constructor
    Transaction(const Transaction& c);
    /// No assignment
    Transaction& operator=(const Transaction& c);
    /// No equals operator
    bool operator==(const Transaction& c) const;
  protected:
    /// Standard empty constructor
    Transaction () : m_technology(0)  {                         }
    /// Destructor (called only by sub-classes)
    virtual ~Transaction()            {                         }
  public:
    /// Access to the technology type
    int technology() const            { return m_technology;    }
    /// Access to the state of the transaction
    virtual Action state() const = 0;
    /// Access to the transaction type
    virtual Type type() const = 0;

    /// Translate Action to string
    static const char* actionAsString(Action action);
 
  };
}       // End namespace pool
#endif  // POOL_TRANSACTION_H
