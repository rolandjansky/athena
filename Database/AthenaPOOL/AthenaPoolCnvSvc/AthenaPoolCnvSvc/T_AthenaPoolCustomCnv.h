/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCUSTOMCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCUSTOMCNV_H

/** @file T_AthenaPoolCustomCnv.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolCustomCnv class.
 *  @author Marcin.Nowak@cern.ch
 **/

#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"
#include "T_AthenaPoolCustCnv.h"

#include <vector>

// class TopLevelTPCnvBase;
// need the TopLevelTPCnvBase typedef still
#include "AthenaPoolTopLevelTPCnvBase.h"

// forward declarations:
template <class T, class P> class T_AthenaPoolExtendingCnv;

/**
 * @class T_AthenaPoolCustomCnvWithKey
 * @brief This templated class extends T_AthenaPoolCustCnv
 *        to provide management of the persistent objects
 *        created by the converter and to correctly delete
 *        retrieved persistent objects.
 */
template <class TRANS, class PERS>
class T_AthenaPoolCustomCnvWithKey : public T_AthenaPoolCustCnv<TRANS, PERS>
{
   /// need the extending converter class as friend,
   /// so clones can access the original converter methods
friend class T_AthenaPoolExtendingCnv<TRANS,PERS>;

public:
   typedef T_AthenaPoolCustCnv<TRANS, PERS> BaseType;

   /// Constructor
   /// @param svcloc [IN] Gaudi service locator
   /// @param name [IN] Optional name, for error reporting.
   T_AthenaPoolCustomCnvWithKey(ISvcLocator* pSvcLocator,
                                const char* name = nullptr);

protected:

   // the 2 following methods are allowed to throw std::runtime_error

   /** method to be implemented by the developer.
      It should create the persistent representation of the object,
      using the default top-level TP converter.
      @param obj [IN] transient object
      @return the created persistent representation (by pointer)
   */
   virtual PERS* createPersistentWithKey(TRANS* obj, const std::string& key) = 0;

   /** method to be implemented by the developer.
      It has to find out the type of the
      persistent object to be read (by comparing GUIDs), read it using
      poolReadObject<P>(), call TLP converter to create a transient
      representation and return it.
      if the version 1 of poolReadObject is used, the persistent
      object HAS TO BE DELETED manually.
   */
  virtual TRANS* createTransientWithKey(const std::string& key) = 0;

   /** Read object of type P.  This is an exception-throwing version of poolToObject()
      plus reading of all extending objects.
      Version 1 - (see createTransient() above)
      @return object read from POOL (by pointer)
   */
   template <class P>
   P* poolReadObject();

   /** Read object of type P (plus all extending objects)
      using the indicated top-level TP converter.
      Version 2 - (see createTransient() above)
      NOTE:  the TLP converter will delete the persistent object after createTransient()
      @param tlp_converter [IN] top-level TP converter to be used when reading
   */
   template <class P>
   void poolReadObject(TopLevelTPCnvBase& tlp_converter);

   /// Remember the POOL object to be written out (will be deleted after commit)
   /// @param obj [IN] persistent object
   void keepPoolObj(PERS* obj, const std::string& output);

   /// Obsolete methods replaced by createPersistent() and createTransient()
   /// obsolete
   virtual StatusCode transToPers(TRANS*, PERS*&) override { return(StatusCode::FAILURE); }
   /// obsolete
   virtual StatusCode persToTrans(TRANS*&, PERS*) override { return(StatusCode::FAILURE); }

   /// Convert an object into Persistent.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPers(DataObject* pObj, IOpaqueAddress*& pAddr) override;

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPool(IOpaqueAddress* pAddr, DataObject* pObj) override;

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   /// @param key [IN] SG key of the object being read.
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token,
                                       const std::string& key) override;

   /// Callback from the CleanupSvc to delete persistent object in the local list
   virtual StatusCode cleanUp(const std::string& output) override;

   /// Local cache for persistent objects created by this converter, grouped by processing slot
   /// These objects are deleted after a commit.
   std::map<std::string, std::vector< std::unique_ptr<PERS> > > m_persObjLists;

   /// protection mutex for m_persObjLists
   std::mutex  m_pListMutex;
};

/**
 * @brief Compatibility for old converter classes that don't get passed the key.
 */
template <class TRANS, class PERS>
class T_AthenaPoolCustomCnv
  : public T_AthenaPoolCustomCnvWithKey<TRANS, PERS>
{
protected:
  using T_AthenaPoolCustomCnvWithKey<TRANS, PERS>::T_AthenaPoolCustomCnvWithKey;

  virtual PERS* createPersistent(TRANS* obj) = 0;
  virtual PERS* createPersistentWithKey(TRANS* obj, const std::string& /*key*/) override;

  virtual TRANS* createTransient() = 0;
  virtual TRANS* createTransientWithKey(const std::string& /*key*/) override;
};

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.icc"

#endif
