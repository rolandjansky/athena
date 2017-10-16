/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETSIMDATACOLLECTIONCNV_H
#define INDETSIMDATACOLLECTIONCNV_H
                                                                                                                                                             
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
                                                                                                                                                             
#include "InDetSimData/InDetSimDataCollection.h"
#include "InDetSimDataCollectionCnv_p1.h"
#include "InDetSimDataCollectionCnv_p2.h"
#include "InDetSimDataCollectionCnv_p3.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef  InDetSimDataCollection_p2     InDetSimDataCollection_PERS;
typedef  InDetSimDataCollectionCnv_p2  InDetSimDataCollectionCnv_PERS;

// base class 
typedef  T_AthenaPoolCustomCnv<InDetSimDataCollection, InDetSimDataCollection_PERS >   InDetSimDataCollectionCnvBase;

class InDetSimDataCollectionCnv : public InDetSimDataCollectionCnvBase {
  friend class CnvFactory<InDetSimDataCollectionCnv >;
                                                                                                                                                             
protected:
  InDetSimDataCollectionCnv (ISvcLocator* svcloc) : InDetSimDataCollectionCnvBase(svcloc) {}
  virtual InDetSimDataCollection_PERS*   createPersistent (InDetSimDataCollection* transCont);
  virtual InDetSimDataCollection* createTransient ();
};
                                                                                                                                                             
#endif

