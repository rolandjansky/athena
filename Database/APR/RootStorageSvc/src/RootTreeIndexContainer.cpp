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
#include "TMemFile.h"
#include "TBranch.h"

using namespace pool;
using namespace std;

RootTreeIndexContainer::RootTreeIndexContainer() : RootTreeContainer(), m_index_ref(nullptr), m_index_shr(nullptr), m_index_entries(0), m_ttree_entries(0), m_index_multi(0), m_index(nullptr) {
   m_index = new long long int;
   m_index_multi = getpid();
}

/// Standard destructor
RootTreeIndexContainer::~RootTreeIndexContainer() {
   delete m_index; m_index = nullptr;
}


long long int RootTreeIndexContainer::nextRecordId()    {
   long long int s = m_index_multi;
   s = s << 32;
   if (m_index_ref == nullptr && m_index_shr == nullptr) {
      s += RootTreeContainer::nextRecordId();
   } else {
      s += m_index_entries;
   }
   return s;
}


DbStatus RootTreeIndexContainer::writeObject(ActionList::value_type& action) {
   // If ROOT TTree has index_ref TBranch share it, otherwise create one and make it part of this POOL container
   if (m_index_ref == nullptr && m_index_shr == nullptr) {
      m_index_shr = m_tree->GetBranch("index_ref");
      if (m_index_shr == nullptr) {
         m_index_ref = static_cast<TBranch*>(m_tree->Branch("index_ref", m_index));
      }
   }
   // POOL container that owns the index_ref TBranch fills it.
   if (m_index_ref != nullptr && m_index_entries >= m_index_ref->GetEntries()) {
      *m_index = this->nextRecordId();
      m_index_ref->SetAddress(m_index);
      if (isBranchContainer() && !m_treeFillMode) m_index_ref->Fill();
   }
   if (isBranchContainer() && !m_treeFillMode) {
      m_tree->SetEntries(m_ttree_entries);
      m_index_entries++;
      m_ttree_entries++;
   }
   DbStatus status = RootTreeContainer::writeObject(action);
   if (isBranchContainer() && !m_treeFillMode) {
      m_tree->SetEntries(m_ttree_entries);
   } else {
      m_index_entries++;
      m_ttree_entries++;
   }
   return status;
}


DbStatus RootTreeIndexContainer::transAct(Transaction::Action action) {
   DbStatus status = RootTreeContainer::transAct(action);
   if (action == Transaction::TRANSACT_FLUSH) {
      m_ttree_entries = 0;
      if (m_tree == nullptr) return Error;
      if (m_index_ref != nullptr && m_tree->GetEntries() > 0 && dynamic_cast<TMemFile*>(m_tree->GetCurrentFile()) == nullptr && m_tree->GetEntryNumberWithIndex(nextRecordId()) == -1) {
         m_tree->BuildIndex("index_ref");
      }
   }
   return status;
}

DbStatus RootTreeIndexContainer::loadObject(void** ptr, ShapeH shape, Token::OID_t& oid) {
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
   return RootTreeContainer::loadObject(ptr, shape, oid);
}
