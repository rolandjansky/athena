/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

//  ====================================================================
//  Transaction.cpp
//  --------------------------------------------------------------------
//
//  Package   : POOLCore
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "StorageSvc/Transaction.h"
using namespace pool;

/// No copy constructor
Transaction::Transaction(const Transaction& c) {
  m_technology = c.m_technology;
}

/// No assignment
Transaction& Transaction::operator=(const Transaction& c)  {
  m_technology = c.m_technology;
  return *this;
}

/// No equals operator
bool Transaction::operator==(const Transaction& /* c */ ) const  {
  return false;
}

/// Translate Action to string
const char* Transaction::actionAsString(Action action) {
   switch(action) {
    case TRANSACT_START    : return "START";
    case TRANSACT_ACTIVE   : return "ACTIVE";
    case TRANSACT_COMMIT   : return "COMMIT";
    case TRANSACT_FLUSH    : return "FLUSH";
    case TRANSACT_ROLLBACK : return "ROLLBACK";
    case TRANSACT_ENDED    : return "ENDED";
   }
   return "UNDEFINED";
}
