/*
 *   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 *   */

// Local implementation files
#include "StorageSvc/DbOption.h"
#include "RootDatabase.h"
#include "RootTreeIndexContainer.h"

// Root include files
#include "TTree.h"
#include "TBranch.h"

using namespace pool;
using namespace std;

RootTreeIndexContainer::RootTreeIndexContainer() :
   m_indexBranch(nullptr), m_index_entries(0), m_index_multi( getpid() ), m_indexBump(0)
{ }


DbStatus RootTreeIndexContainer::open( DbDatabase& dbH, 
                                       const std::string& nam,
                                       const DbTypeInfo* info,
                                       DbAccessMode mod)
{
   auto db = static_cast<const RootDatabase*>( dbH.info() );
   m_indexBump = db? db->currentIndexMasterID() : 0;
   return RootTreeContainer::open( dbH, nam, info, mod );
}


long long int RootTreeIndexContainer::nextRecordId()
{
   long long id = m_index_multi;
   return (id << 32) + RootTreeContainer::nextRecordId() + m_indexBump;
}

void RootTreeIndexContainer::useNextRecordId(long long int nextID)
{
   // Find out how this TTree index is behind the master index in the DB
   m_indexBump = m_indexBranch? nextID - m_indexBranch->GetEntries() : nextID;
   if( m_indexBump < 0 ) {
      // Seems this index is ahead of the master, cannot sync
      m_indexBump = 0;
   }
}


DbStatus RootTreeIndexContainer::writeObject(ActionList::value_type& action)
{
   // Prepare for writing - grab/create the index branch
   if( !m_indexBranch ) {
      m_indexBranch = m_tree->GetBranch("index_ref");
      if( !m_indexBranch ) {
         m_indexBranch = m_tree->Branch("index_ref", &m_index);
      }
   }
   if( m_indexBranch && m_index_entries >= m_indexBranch->GetEntries() ) {
      // need to update the index branch
      m_index = action.link.second;
      m_indexBranch->SetAddress(&m_index);
      if (isBranchContainer() && !m_treeFillMode) m_indexBranch->Fill();
   }
   m_index_entries++;

   return RootTreeContainer::writeObject(action);
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
