// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/AuxStoreRoot.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief Base class for root-based dynamic auxiliary store.
 */


#ifndef ATHCONTAINERSROOT_AUXSTOREROOT_H
#define ATHCONTAINERSROOT_AUXSTOREROOT_H


#include "AthContainers/AuxStoreInternal.h"
#include "AthContainersRoot/IAuxBranches.h"
class TClass;


namespace SG {


/**
 * @brief Base class for root-based dynamic auxiliary store.
 *
 * This class is used to implement the dynamic part of an auxiliary store
 * where the data are stored in root.  It is the result of factoring
 * common code from AuxStoreAPR (pool) and AuxStoreARA (AthenaROOTAccess).
 *
 * The class is based on @c AuxStoreInternal.  Derived classes must implement
 * two methods: @c resolveAuxID, to assign an auxiliary ID to a branch,
 * and @c doReadData to actually read data from the file into the store.
 * The derived class constructor should also call @c fillAuxIDs
 * to complete initialization.
 *
 * Communication is via an instance of @c IAuxBranches, which provides
 * interfaces to enumerate the dynamic branches and to read data.
 */
class AuxStoreRoot
  : public SG::AuxStoreInternal
{
public:
  /**
   * @brief Constructor.
   * @param container Representation of the object for which we're reading.
   * @param entry The entry in the root tree to read.
   * @param standalone If true, this is a standalone auxiliary object.
   */
  AuxStoreRoot (IAuxBranches& container, long long entry, bool standalone);


  /// Destructor.
  virtual ~AuxStoreRoot() {}


  /// implementation of the IAuxStore interface

  /**
   * @brief Return the data vector for one aux data item
   * @param auxid The identifier of the desired aux data item.
   *
   * Each aux data item is stored as a vector, with one entry
   * per entry in the owning container.  This returns a pointer
   * to the start of the vector.
   *
   * This should return 0 if the item doesn't exist.
   */
  virtual const void*                getData(SG::auxid_t auxid) const override;


  /**
   * @brief Return the data vector for one aux data item
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
   * If the data item does not exist, it should be created and initialized
   * to default values.  @c size and @c capacity give the size for the
   * new aux data item vector.
   *
   * If the container is locked, throw an exception.
   */
  virtual void*
  getData(SG::auxid_t auxid, size_t size, size_t capacity) override;


  /**
   * @brief Return the data vector for one aux data decoration item.
   * @param auxid The identifier of the desired aux data item.
   * @param size The current size of the container (in case the data item
   *             does not already exist).
   * @param capacity The current capacity of the container (in case
   *                 the data item does not already exist).
   */
  virtual void*
  getDecoration (SG::auxid_t auxid, size_t size, size_t capacity) override;


  /// Implementation of the IAuxStoreIO interface

  
  /**
   * @brief Return a pointer to the data to be stored for one aux data item.
   * @param auxid The identifier of the desired aux data item.
   *
   * This will usually be a pointer to a @c std::vector; however, it may
   * be something different for a standalone object.
   *
   * Returns 0 and reports an error if the requested aux data item
   * does not exist.
   */
  virtual const void* getIOData(SG::auxid_t auxid) const override;


  /// Methods to be implemented by derived classes.


  /**
   * @brief Determine auxiliary ID for a branch.
   * @param container Representation of the object being read.
   * @param ti @c type_info for the element, or nullptr.
   * @param name Name of the attribute.
   * @param elemTypeName Name of the type of this attribute.
   * @param branchTypeName Name of the type of the object stored
   *        in the branch.  Should be the same as @c elemTypeName
   *        for standalone objects.
   */
  virtual SG::auxid_t resolveAuxID (IAuxBranches& container,
                                    const std::type_info* ti,
                                    const std::string& name,
                                    const std::string& elemTypeName,
                                    const std::string& branchTypeName) = 0;


  /**
   * @brief Actually read data into the attribute.
   * @param container Representation of the object being read.
   * @param auxid ID of the variable being read.
   * @param branch Root branch being read.
   * @param cl Class of the data stored in the branch.
   * @param vector Pointer to the I/O data for this attribute.
   *               (Will be a vector for the normal case, the element
   *               itself for standalone objects.)
   * @param entry Entry being read in the TTree.
   *
   * Returns true on success, false on failure.
   */
  virtual bool doReadData (IAuxBranches& container,
                           SG::auxid_t auxid,
                           TBranch& branch,
                           TClass* cl,
                           void* vector,
                           long long entry) = 0;



protected:
  /// Methods to be called by derived classes.

  /**
   * @brief Derived classes should call this from their constructor
   *        to complete initialization.
   *
   * Creates the auxid set from the list of branches.
   * This cannot be done from the @c AuxStoreRoot constructor as it needs
   * to call the @c resolveAuxID virtual method.
   */  
  void fillAuxIDs();


  /**
   * @brief Change the TTree entry being read.
   * @param entry New entry being read.
   */
  void setEntry (long long entry);

     
private:
  /**
   * @brief Read data from ROOT and store it in the internal vector.
   *
   * Returns true on success, false on failure.
   */
  bool readData(SG::auxid_t auxid);


  /// Current TTree entry being read.
  long long          m_entry;

  /// Representation of the object being read.
  IAuxBranches&      m_container;

  
  /// Mutex used to synchronize modifications to the cache vector.
  typedef AthContainers_detail::mutex mutex_t;
  typedef AthContainers_detail::lock_guard<mutex_t> guard_t;
  mutable mutex_t m_mutex;
};


} // namespace SG


#endif // not ATHCONTAINERSROOT_AUXSTOREROOT_H
