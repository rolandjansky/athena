/*
 *   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

RootTreeIndexContainer::RootTreeIndexContainer() : RootTreeContainer(), m_index_ref(nullptr), m_index_foreign(nullptr), m_index_multi(0), m_index(nullptr) {
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
   if (m_tree != nullptr) {
      if (m_index_foreign != nullptr) {
         s += m_index_foreign->GetEntries();
      } else {
         m_index_foreign = (TBranch*)m_tree->GetBranch("index_ref");
         if (m_index_foreign != nullptr) {
            s += m_index_foreign->GetEntries();
         } else {
            s += RootTreeContainer::nextRecordId();
         }
      }
   }
   return s;
}


DbStatus RootTreeIndexContainer::writeObject(ActionList::value_type& action) {
   long long int s = 0;
   if( isBranchContainer() ) {
      TBranch * pBranch = m_tree->GetBranch(m_branchName.c_str());
      if (pBranch != nullptr) s = pBranch->GetEntries();
   } else {
      s = m_tree->GetEntries();
   }
   if (m_index_ref  == nullptr && m_index_foreign == nullptr) {
      if (m_tree->GetBranch("index_ref") == nullptr) {
         m_index_ref = (TBranch*)m_tree->Branch("index_ref", m_index);
      } else {
         m_index_foreign = (TBranch*)m_tree->GetBranch("index_ref");
      }
   }
   if (m_index_ref != nullptr && s >= m_index_ref->GetEntries()) {
      *m_index = this->nextRecordId();
      m_index_ref->SetAddress(m_index);
      if( isBranchContainer() && !m_treeFillMode ) m_index_ref->Fill();
   }
   if( isBranchContainer() && !m_treeFillMode ) m_tree->SetEntries(s);
   DbStatus status = RootTreeContainer::writeObject(action);
   if( isBranchContainer() && !m_treeFillMode ) m_tree->SetEntries(s + 1);
   return status;
}


DbStatus RootTreeIndexContainer::transAct(Transaction::Action action) {
   DbStatus status = RootTreeContainer::transAct(action);
   if (action == Transaction::TRANSACT_FLUSH) {
      if (m_tree == nullptr) return Error;
      if (m_index_ref != nullptr && m_tree->GetEntryNumberWithIndex(nextRecordId()) == -1) {
         m_tree->BuildIndex("index_ref");
      }
   }
   return status;
}

DbStatus RootTreeIndexContainer::loadObject(DataCallBack* call, Token::OID_t& oid) {
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
   return RootTreeContainer::loadObject(call, oid);
}
