/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CORACOOL_CORACOOLSEQUENCE_H
#define CORACOOL_CORACOOLSEQUENCE_H
// CoraCoolSequence.h
// class to implement a sequence table to generate primary and foreign keys
// using an auxillary table with SELECT ... FOR UPDATE semantics
// table is called <dbname>_CORACOOLKEYS with one row per sequence
// Richard Hawkings, started 24/10/06

#include <string>

class CoraCoolSequence {
 public:
  // access a sequence - try to create required tables if create set
  // assumes a database transaction is already active
  CoraCoolSequence(const std::string& dbname, const std::string& seqname,
		   coral::ISessionProxy* proxy, bool create=false);

  // fetch the next value in the sequence, reserving inc values
  // assumes a database transaction is already active
  // locks the corresponding table rows until the transaction is committed
  int fetch(const int inc=1);

  // query to find the current value (which is not yet used), but no increment
  // must be followed by a fetch if this key value is actually used
  bool querySeq(int& keyval,bool update=false,bool gettable=false);

  // drop the current sequence from the table
  // assumes a transaction is already active
  // after this, the current sequence object is invalid and will not work
  bool dropSeq();

 private:
  // hide default constructor
  CoraCoolSequence();

  std::string m_dbname;           // the parent COOL database instance
  std::string m_seqname;          // name of the sequence
  coral::ISessionProxy* m_proxy;  // pointer to CORAL session
  coral::ITable* m_table;         // pointer to the sequence table
};

#endif // CORACOOL_CORACOOLSEQUENCE_H
