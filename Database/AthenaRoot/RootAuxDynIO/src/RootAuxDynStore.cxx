/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "AthContainers/AuxTypeRegistry.h"
#include "AthContainers/tools/error.h"
#include "AthContainers/exceptions.h"
#include "AthContainersInterfaces/AuxDataOption.h"

#include "RootAuxDynStore.h"
#include "RootAuxDynReader.h"

#include <iostream>
using namespace std;




RootAuxDynStore::RootAuxDynStore(RootAuxDynReader& reader, long long entry, bool standalone)
  : SG::AuxStoreInternal( standalone ),
    m_reader(reader),
    m_entry(entry)
{
   for( auto id : reader.auxIDs() ) {
      addAuxID(id);
   }
   lock();
}


      
const void* RootAuxDynStore::getData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  // lock
  const void* ret = SG::AuxStoreInternal::getData (auxid);
  if (!ret) {
    const_cast<RootAuxDynStore*>(this)->readData(auxid);
    ret = SG::AuxStoreInternal::getData (auxid);
  }
  return ret;
}


const void* RootAuxDynStore::getIOData(SG::auxid_t auxid) const
{
  guard_t guard (m_mutex);
  const void* ret = SG::AuxStoreInternal::getIODataInternal (auxid, true);
  if (!ret) {
    const_cast<RootAuxDynStore*>(this)->readData(auxid);
    ret = SG::AuxStoreInternal::getIOData (auxid);
  }
  return ret;
}


bool RootAuxDynStore::readData(SG::auxid_t auxid)
{
   //cout << "RootAuxDynStore::getData() id=" << auxid << ",  name="
   //     << reg.getName(auxid) << endl;
   try {
      // cout << "---  RootAuxDynStore  reading id=" << auxid << endl;
      auto& brInfo = m_reader.getBranchInfo(auxid, *this);
      if( !brInfo.branch ) return false; 
 
      // Make a 1-element vector.
      SG::AuxStoreInternal::getDataInternal(auxid, 1, 1, true);
      if( brInfo.isPackedContainer ) {
         setOption (auxid, SG::AuxDataOption ("nbits", 32));
      }

      // get memory location where to write data from the branch entry
      void *       vector = const_cast<void*>(SG::AuxStoreInternal::getIOData (auxid)); // xxx
      void *       data = &vector;
      if( standalone() && !brInfo.tclass ) {
         // reading fundamental type - ROOT expects a direct pointer
         data = vector;
      }
   
      // read branch
      brInfo.setAddress(data);
      int  nbytes = brInfo.branch->GetEntry(m_entry);
      if( nbytes <= 0 )
         throw string("Error reading branch ") + brInfo.branch->GetName();
      // read OK
      m_reader.addBytes(nbytes);
   }
   catch(const string& e_str) {
      ATHCONTAINERS_ERROR("RootAuxDynStore::getData", e_str);
      return false;
   }
   return true;
}


/**
 * @brief Return the data vector for one aux data decoration item.
 * @param auxid The identifier of the desired aux data item.
 * @param size The current size of the container (in case the data item
 *             does not already exist).
 * @param capacity The current capacity of the container (in case
 *                 the data item does not already exist).
 *
 * Each aux data item is stored as a vector, with one entry
 * per entry in the owning container.  This returns a pointer
 * to the start of the vector.
 *
 * The base class implementation works except for the case where we have
 * not yet created a vector for an item in the root file.  We need to 
 * detect that case and raise an exception.
 */
void*
RootAuxDynStore::getDecoration (SG::auxid_t auxid, size_t size, size_t capacity)
{
  guard_t guard (m_mutex);
  if (SG::AuxStoreInternal::getIODataInternal (auxid, true) == 0 &&
      SG::AuxStoreInternal::getAuxIDs().count (auxid) > 0)
  {
    throw SG::ExcStoreLocked (auxid);
  }
  return SG::AuxStoreInternal::getDecoration (auxid, size, capacity);
}



