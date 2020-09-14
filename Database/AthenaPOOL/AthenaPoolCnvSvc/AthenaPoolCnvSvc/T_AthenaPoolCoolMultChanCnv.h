/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLCNVSVC_T_ATHENAPOOLCOOLMULTCHANCNV_H
#define ATHENAPOOLCNVSVC_T_ATHENAPOOLCOOLMULTCHANCNV_H

/** @file T_AthenaPoolCoolMultChanCnv.h
 *  @brief This templated class provides the POOL converter to
 *  translate a DataVector<T> to and from a CondAttrListCollection. In
 *  this case, the elements of T are written/read and their POOL
 *  tokens are stored in CondAttrListCollection.
 *  @author RD Schaffer <R.D.Schaffer@cern.ch>
 *  $Id: T_AthenaPoolCoolMultChanCnv.h,v 1.4 2007-10-26 15:42:51 schaffer Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"

#include <string>

class DataObject;
class StatusCode;
class CondAttrListCollection;

/// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

/** @class T_AthenaPoolCoolMultChanCnv
 *  @brief This templated class provides the POOL converter to
 *  translate a DataVector<T> to and from a CondAttrListCollection. In
 *  this case, the elements of T are written/read and their POOL
 *  tokens are stored in CondAttrListCollection.
 **/
template <class COLL_T, class ELEM_T, class ELEM_P = ELEM_T>
class T_AthenaPoolCoolMultChanCnv : public T_AthenaPoolCustCnv<COLL_T, ELEM_T> {

public:
    /// Constructor
    T_AthenaPoolCoolMultChanCnv(ISvcLocator* svcloc);

protected:
    /// Create a transient object from a POOL persistent representation.
    /// @param pAddr [IN] IOpaqueAddress of POOL persistent representation.
    /// @param pObj [OUT] pointer to the transient object.
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    /// Create a POOL persistent representation for a transient object.
    /// @param pObj [IN] pointer to the transient object.
    /// @param pAddr [OUT] IOpaqueAddress of POOL persistent representation.
    virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr);

    /// Create a POOL persistent representation for a transient object.
    /// @param pAddr [IN] IOpaqueAddress of POOL persistent representation.
    /// @param pObj [IN] pointer to the transient object.
    virtual StatusCode fillRepRefs(IOpaqueAddress* pAddr, DataObject* pObj);

    /// Write out objects stored in "obj" into POOL and save the tokens
    /// in a CondAttrListCollection, as well write out "obj" i.e. CondMultChanCollImpl
    /// @param obj          [IN]  pointer to the collection of objects.
    /// @param attrListColl [OUT] CondAttrListCollection filled with tokens
    /// @param token        [OUT] Token from writing out CondMultChanCollImpl
    StatusCode objectToAttrListColl ATLAS_NOT_THREAD_SAFE
                                   (COLL_T* obj, IOpaqueAddress*& pAddr,
				    CondAttrListCollection*& attrListColl,
				    Token*& token);

    /// Read in objects from POOL for the tokens stored
    /// CondAttrListCollection and save the objects in the output
    /// collection
    /// @param attrListColl [IN]  CondAttrListCollection filled with tokens
    /// @param obj          [OUT] pointer to the collection of objects.
    StatusCode attrListCollToObject(CondAttrListCollection* attrListColl, 
				    COLL_T*& obj);

    /// Read in CondAttrListCollImpl and the objects for its tokens,
    /// saving them in the output collection
    /// @param collImplToken [IN]  CondAttrListCollImpl token
    /// @param obj          [OUT] pointer to the collection of objects.
    StatusCode condMultChanCollImplToObject(const std::string& collImplToken, 
					    COLL_T*& obj);

    // the 2 following methods are allowed to throw std::runtime_error

    /** method to be implemented by the developer.
        It should create the persistent representation of the object,
        using the default top-level TP converter.
        @param obj [IN] transient object
        @return the created persistent representation (by pointer)
    */
    virtual ELEM_P* createPersistent(ELEM_T* obj);

    /** method to be implemented by the developer.
        It has to find out the type of the
        persistent object to be read (by comparing GUIDs),  read it using
        poolReadObject(), call TLP converter to create a transient
        representation and return it.
        if the version 1 of poolReadObject is used, the persistent
        object HAS TO BE DELETED manually.
    */
    virtual ELEM_T* createTransient();

    //-------------------------------------------------------------------
    // Helper methods intended to by used when implementing createTransient()

    /** Read object of type P.  This is an exception-throwing version of poolToObject()
        plus reading of all extending objects.
        Version 1 - (see createTransient() above)
        @return object read from POOL (by pointer)
    */
    template <class P>
    P*                 poolReadObject();

    ELEM_T*            poolReadObject();

    /// Dummy methods not needed here
    virtual StatusCode transToPers(COLL_T* obj, ELEM_T*& persObj);
    virtual StatusCode persToTrans(COLL_T*& transObj, ELEM_T* obj);

};

#include "AthenaPoolCnvSvc/T_AthenaPoolCoolMultChanCnv.icc"
#endif
