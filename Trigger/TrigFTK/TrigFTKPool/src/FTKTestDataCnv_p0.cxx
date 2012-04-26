/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigFTKPool/FTKTestData.h"
#undef private
#undef protected

#include "FTKTestDataCnv_p0.h"

void FTKTestDataCnv_p0::persToTrans(const FTKTestData_p0 *persObj, 
				    FTKTestData *transObj, MsgStream&)
{
  transObj->m_ival = persObj->m_ival;
}


void FTKTestDataCnv_p0::transToPers(const FTKTestData *transObj,
				    FTKTestData_p0 *persObj, MsgStream&)
{
   persObj->m_ival = transObj->m_ival;
}
