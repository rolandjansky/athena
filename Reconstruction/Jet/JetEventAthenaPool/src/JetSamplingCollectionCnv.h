///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetSamplingCollectionCnv.h 
// Header file for class JetSamplingCollectionCnv
// Author: S.Binet<binet@cern.ch>
// Modif: Belen.Salvachua@cern.ch  Dec 2008  Add JetSamplingCollection_p2
/////////////////////////////////////////////////////////////////// 
#ifndef RECATHENAPOOL_JETSAMPLINGCOLLECTIONCNV_H 
#define RECATHENAPOOL_JETSAMPLINGCOLLECTIONCNV_H 

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPCnvCnv.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetSamplingCollection_p1.h"
#include "JetEventTPCnv/JetSamplingCollection_p2.h"
#include "JetEventTPCnv/JetSamplingCollectionCnv_p1.h"
#include "JetEventTPCnv/JetSamplingCollectionCnv_p2.h"

// JetEvent includes
#include "JetEvent/JetSamplingCollection.h"


typedef T_AthenaPoolTPCnvCnv< JetSamplingCollection,
                              JetSamplingCollectionCnv_p2,
                              JetSamplingCollectionCnv_p1,
                              T_TPCnvNull<JetSamplingCollection> >
  JetSamplingCollectionCnv;


#endif //> RECATHENAPOOL_JETSAMPLINGCOLLECTIONCNV_H
