/*
 *   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 *   */

// Local implementation files
#include "RootTreeIndexContainer.h"

#include "StorageSvc/Transaction.h"

#include "RootDataPtr.h"
#include "RootDatabase.h"

// Root include files
#include "TTree.h"
#include "TBranch.h"

using namespace pool;
using namespace std;

RootTreeIndexContainer::RootTreeIndexContainer(IOODatabase* idb) : RootTreeContainer(idb) {
}

/// Standard destructor
RootTreeIndexContainer::~RootTreeIndexContainer() {
}

DbStatus RootTreeIndexContainer::loadObject(DataCallBack* call, Token::OID_t& oid, DbAccessMode mode) {
   if ((oid.second >> 32) > 0) {
      long long int evt_id = m_tree->GetEntryNumberWithIndex(oid.second);
      if (evt_id == -1) {
         m_tree->BuildIndex("index_ref");
         evt_id = m_tree->GetEntryNumberWithIndex(oid.second);
      }
      if (evt_id >= 0) {
         oid.second = evt_id;
      }
   }
   return RootTreeContainer::loadObject(call, oid, mode);
}
