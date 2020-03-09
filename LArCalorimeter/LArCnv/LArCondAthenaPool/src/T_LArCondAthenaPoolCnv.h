/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T_LARCONDATHENAPOOLCNV_H
#define T_LARCONDATHENAPOOLCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCnv.h"

class StoreGateSvc;

//  Template AthenaPool converter for LAr Conditions data
//  T == Transient Conditions data class 
//     T should have T::P defined as a type, and P is the persistent data
//     type to be stored in POOL


template <class T>
class T_LArCondAthenaPoolCnv 
	: public T_AthenaPoolCnv<T>
{


protected:

   typedef T_AthenaPoolCnv<T> LArCondAthenaPoolCnvBase;

   friend class CnvFactory<T_LArCondAthenaPoolCnv<T> >;

   /// Constructor
   T_LArCondAthenaPoolCnv(ISvcLocator* svcloc);

public:

    // persistent class defined in Transient class
//    typedef typename T::MAP_P MAP_P ; 
    typedef typename T::GainMap MAP_P ; 
//    typedef typename T::GainMap GainMap; 

    virtual ~T_LArCondAthenaPoolCnv();

    virtual StatusCode DataObjectToPool(IOpaqueAddress* pAddr, DataObject* pObj) override;
    virtual StatusCode PoolToDataObject(DataObject*& pObj,
                                        const std::string& token,
                                        const std::string& key) override;


private:

}  ;

#include "T_LArCondAthenaPoolCnv.icc" 

#endif


