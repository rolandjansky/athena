/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKPool/FTKTestData.h"
#include "FTKTestDataCnv_p0.h"

void FTKTestDataCnv_p0::persToTrans(const FTKTestData_p0 *persObj, 
				    FTKTestData *transObj, MsgStream&) const
{
  transObj->setValue (persObj->m_ival);
}


void FTKTestDataCnv_p0::transToPers(const FTKTestData *transObj,
				    FTKTestData_p0 *persObj, MsgStream&) const
{
  persObj->m_ival = transObj->getValue();
}
