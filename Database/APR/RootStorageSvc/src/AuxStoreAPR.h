/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTSTORAGESVC_AUXSTOREAPR
#define ROOTSTORAGESVC_AUXSTOREAPR

#include "AthContainers/AuxStoreInternal.h" 
#include "RootTreeContainer.h" 

#include <stdexcept>
#include "AthContainersInterfaces/IAuxTypeVector.h"

#include "TClass.h"

namespace pool {

   class AuxStoreAPR : public SG::AuxStoreInternal
   {
  public:
     AuxStoreAPR(RootTreeContainer &container, long long entry, bool standalone=false);
     virtual ~AuxStoreAPR() {}

     /// implementation of the IAuxStore interface
     virtual const void*                getData(SG::auxid_t auxid) const;
     virtual void*                      getData(SG::auxid_t auxid, size_t size, size_t capacity);

     ///  implementation of the IAuxStoreIO interface
     virtual const void*                getIOData(SG::auxid_t auxid) const;


     /**
      * @brief Return the data vector for one aux data decoration item.
      * @param auxid The identifier of the desired aux data item.
      * @param size The current size of the container (in case the data item
      *             does not already exist).
      * @param capacity The current capacity of the container (in case
      *                 the data item does not already exist).
      */
     void* getDecoration (SG::auxid_t auxid, size_t size, size_t capacity);

    
  protected:
     /// read data from ROOT and store it in m_vecs. Returns False on error
     bool readData(SG::auxid_t auxid);
     
  protected:
     long long          m_entry;
     RootTreeContainer  &m_container;


   private:
     /// Mutex used to synchronize modifications to the cache vector.
     typedef AthContainers_detail::mutex mutex_t;
     typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
     mutable mutex_t m_mutex;
   };

  

   inline
   void* AuxStoreAPR::getData(SG::auxid_t auxid, size_t /*size*/, size_t /*capacity*/)
   {
      // MN:  how do we add new attributes to this store? A:for now we don't
      return const_cast<void*>(getData(auxid));
   }

}   

#endif

