/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKCOLLECTIONCNV_P1_H
#define LUMIBLOCKCOLLECTIONCNV_P1_H

///////////////////////////////////////////////////////////////////////////
//  Header file for class LumiBlockCollectionCnv_p1
//  Author: Marjorie Shapiro
///////////////////////////////////////////////////////////////////////////

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p1.h"
#include "LumiBlockRangeCnv_p1.h"


typedef T_AthenaPoolTPCnvVector< LumiBlockCollection, LumiBlockCollection_p1, LumiBlockRangeCnv_p1 >  LumiBlockCollectionCnv_p1;

template<>
class T_TPCnv<LumiBlockCollection, LumiBlockCollection_p1>
  : public LumiBlockCollectionCnv_p1
{
public:
};

#endif  //LUMIBLOCKCOLLECTIONCNV_P1_H
