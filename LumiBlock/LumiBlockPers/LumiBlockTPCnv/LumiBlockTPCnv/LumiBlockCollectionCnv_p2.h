/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUMIBLOCKCOLLECTIONCNV_P2_H
#define LUMIBLOCKCOLLECTIONCNV_P2_H

///////////////////////////////////////////////////////////////////////////
//  Header file for class LumiBlockCollectionCnv_p2
//  Author: Marjorie Shapiro
///////////////////////////////////////////////////////////////////////////

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "LumiBlockData/LumiBlockCollection.h"
#include "LumiBlockTPCnv/LumiBlockCollection_p2.h"
#include "LumiBlockRangeCnv_p2.h"


typedef T_AthenaPoolTPCnvVector< LumiBlockCollection, LumiBlockCollection_p2, LumiBlockRangeCnv_p2 >  LumiBlockCollectionCnv_p2;

template<>
class T_TPCnv<LumiBlockCollection, LumiBlockCollection_p2>
  : public LumiBlockCollectionCnv_p2
{
public:
};

#endif  //LUMIBLOCKCOLLECTIONCNV_P2_H
