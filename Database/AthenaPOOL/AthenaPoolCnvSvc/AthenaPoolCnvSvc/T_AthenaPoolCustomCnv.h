/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCUSTOMCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCUSTOMCNV_H

/** @file T_AthenaPoolCustomCnv.h
 *  @brief his file contains the class definition for the templated T_AthenaPoolCustomCnv class.
 *  @author Marcin.Nowak@cern.ch
 **/
#include "T_AthenaPoolCustCnv.h"
#include <vector>

#include "AthenaPoolTopLevelTPCnvBase.h"
//class TopLevelTPCnvBase;
template <class T, class P> class T_AthenaPoolExtendingCnv;

/** @class T_AthenaPoolCustomCnv
 *  @brief This templated class extends T_AthenaPoolCustCnv to provide management of the persistent objects
 *  created by the converter and to correctly delete retrieved persistent objects.
 **/
template <class TRANS, class PERS>
class T_AthenaPoolCustomCnv : public T_AthenaPoolCustCnv<TRANS, PERS> {

friend class CnvFactory<T_AthenaPoolCustomCnv<TRANS, PERS> >;
   /// need the extending converter class as friend,
   /// so clones can access the original converter methods
friend class T_AthenaPoolExtendingCnv<TRANS,PERS>;

public:
   typedef T_AthenaPoolCustCnv<TRANS, PERS> BaseType;

protected:
   /// Constructor
   /// @param svcloc [IN] Gaudi service locator
   T_AthenaPoolCustomCnv(ISvcLocator* pSvcLocator);

   // the 2 following methods are allowed to throw std::runtime_error

   /** method to be implemented by the developer.
      It should create the persistent representation of the object,
      using the default top-level TP converter.
      @param obj [IN] transient object
      @return the created persistent representation (by pointer)
   */
   virtual PERS* createPersistent(TRANS* obj) = 0;

   /** method to be implemented by the developer.
      It has to find out the type of the
      persistent object to be read (by comparing GUIDs), read it using
      poolReadObject<P>(), call TLP converter to create a transient
      representation and return it.
      if the version 1 of poolReadObject is used, the persistent
      object HAS TO BE DELETED manually.
   */
   virtual TRANS* createTransient() = 0;

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
   void poolReadObject(AthenaPoolTopLevelTPCnvBase& tlp_converter);

   /// Remember the POOL object to be written out (will be deleted after commit)
   /// @param obj [IN] persistent object
   void keepPoolObj(PERS* obj);

   /// Obsolete methods replaced by createPersistent() and createTransient()
   /// obsolete
   virtual StatusCode transToPers(TRANS*, PERS*&) { return(StatusCode::FAILURE); }
   /// obsolete
   virtual StatusCode persToTrans(TRANS*&, PERS*) { return(StatusCode::FAILURE); }

   /// Write an object into POOL.
   /// @param pObj [IN] pointer to the transient object.
   /// @param key [IN] StoreGate key (string) - placement hint to generate POOL container name
   virtual StatusCode DataObjectToPool(DataObject* pObj, const std::string& key);

   /// Read an object from POOL.
   /// @param pObj [OUT] pointer to the transient object.
   /// @param token [IN] POOL token of the persistent representation.
   virtual StatusCode PoolToDataObject(DataObject*& pObj, const Token* token);

   /// Callback from the CleanupSvc to delete persistent object in the local list
   virtual StatusCode cleanUp();

   /// Local cache for persistent objects created by this converter (Ahena does not use POOL cache).
   /// These objects are deleted after a commit.
   std::vector<PERS*> m_persObjList;
};

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.icc"
#endif
