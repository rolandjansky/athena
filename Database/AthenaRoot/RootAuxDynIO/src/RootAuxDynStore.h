/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROOTAUXDYNSTORE_H
#define ROOTAUXDYNSTORE_H

#include "AthContainers/AuxStoreInternal.h" 

#include <string>
#include <mutex>

class RootAuxDynReader;

class RootAuxDynStore : public SG::AuxStoreInternal
{
public:
  RootAuxDynStore(RootAuxDynReader& reader, long long entry, bool standalone,
                  std::mutex* iomtx = nullptr);
  
  virtual ~RootAuxDynStore() {}

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


  /**
   * @brief Test if a particular variable is tagged as a decoration.
   * @param auxid The identifier of the desired aux data item.
   */
  virtual bool isDecoration (SG::auxid_t auxid) const override;

     
protected:
  /// read data from ROOT and store it in m_vecs. Returns False on error
  bool readData(SG::auxid_t auxid);
     
protected:
  RootAuxDynReader&  m_reader;
  long long          m_entry;

private:
  /// Mutex used to synchronize modifications to the cache vector.
  typedef AthContainers_detail::mutex mutex_t;
  typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
  mutable mutex_t m_mutex;

  /// Optional mutex used to serialize I/O with RootStorageSvc (not owned)
  std::mutex       *m_iomutex;
};

  

inline
void* RootAuxDynStore::getData(SG::auxid_t auxid, size_t /*size*/, size_t /*capacity*/)
{
   // MN:  how do we add new attributes to this store? A:for now we don't
   return const_cast<void*>(getData(auxid));
}

  

#endif
