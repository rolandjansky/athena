/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  $Header: /cvs/PF/pool/StorageSvc/src/Transaction.cpp,v 1.7 2007/11/20 16:43:10 frankb Exp $
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
