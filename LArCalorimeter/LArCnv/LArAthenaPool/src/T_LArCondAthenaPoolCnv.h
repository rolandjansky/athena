/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T_LARCONDATHENAPOOLCNV_H
#define T_LARCONDATHENAPOOLCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustCnv.h"

class StoreGateSvc;

//  Template AthenaPool converter for LAr Conditions data
//  T == Transient Conditions data class 
//     T should have T::P defined as a type, and P is the persistent data
//     type to be stored in POOL


template <class T>
class T_LArCondAthenaPoolCnv 
	: public T_AthenaPoolCustCnv<T,typename T::PERSIST>
{


protected:

   typedef T_AthenaPoolCustCnv<T,typename T::PERSIST> LArCondAthenaPoolCnvBase;

   friend class CnvFactory<T_LArCondAthenaPoolCnv<T> >;

   /// Constructor
   T_LArCondAthenaPoolCnv(ISvcLocator* svcloc);

public:

    // persistent class defined in Transient class
    typedef typename T::PERSIST PERSIST ; 

    virtual ~T_LArCondAthenaPoolCnv();

    /// initialization

    virtual StatusCode transToPers(T* obj, PERSIST*& persObj) ;
    virtual StatusCode persToTrans(T*& transObj, PERSIST* obj) ;


private:

}  ;

#include "T_LArCondAthenaPoolCnv.icc" 

#endif


