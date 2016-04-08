// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaROOTAcccess/AuxStoreARA.h
 * @author marcin, scott
 * @brief Aux store implementation to manage dynamic aux variables.
 *
 * This is basically the same as the corresponding class in RootStorageSvc.
 * Duplicated here due to the lack of any suitable common packages
 * with the correct dependencies.
 */


#ifndef ATHENAROOTACCESS_AUXSTOREARA
#define ATHENAROOTACCESS_AUXSTOREARA

#include "AthContainers/AuxStoreInternal.h" 
#include "AthenaROOTAccess/IAuxBranches.h"

#include "AthContainersInterfaces/IAuxTypeVector.h"
#include <stdexcept>
#include <vector>
#include <list>

#include "TClass.h"
class TBranch;


namespace AthenaROOTAccess {


  /**
   * @brief Aux store implementation to manage dynamic aux variables.
   */
  class AuxStoreARA : public SG::AuxStoreInternal
  {
  public:
     AuxStoreARA(IAuxBranches &container, long long entry, bool standalone=false);
     virtual ~AuxStoreARA() override;

     void GetEntry (long long entry);
     void SetEntry (long long entry) { m_entry = entry; }

     /// implementation of the IAuxStore interface
     virtual const void*                getData(SG::auxid_t auxid) const override;
     virtual void*                      getData(SG::auxid_t auxid, size_t size, size_t capacity) override;

     ///  implementation of the IAuxStoreIO interface
     virtual const void*                getIOData(SG::auxid_t auxid) const override;


     /**
      * @brief Return the data vector for one aux data decoration item.
      * @param auxid The identifier of the desired aux data item.
      * @param size The current size of the container (in case the data item
      *             does not already exist).
      * @param capacity The current capacity of the container (in case
      *                 the data item does not already exist).
      */
     virtual void* getDecoration (SG::auxid_t auxid, size_t size, size_t capacity) override;

    
  protected:
     /// read data from ROOT and store it in m_vecs. Returns False on error
     bool readData(SG::auxid_t auxid);
     
  protected:
     long long          m_entry;
     IAuxBranches      &m_container;


   private:
     std::list<void*> m_ptrs;
     std::vector<TBranch*> m_branches;

     /// Mutex used to synchronize modifications to the cache vector.
     typedef AthContainers_detail::mutex mutex_t;
     typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
     mutable mutex_t m_mutex;
   };

  

   inline
   void* AuxStoreARA::getData(SG::auxid_t auxid, size_t /*size*/, size_t /*capacity*/)
   {
      // MN:  how do we add new attributes to this store? A:for now we don't
      return const_cast<void*>(getData(auxid));
   }

}   

#endif

