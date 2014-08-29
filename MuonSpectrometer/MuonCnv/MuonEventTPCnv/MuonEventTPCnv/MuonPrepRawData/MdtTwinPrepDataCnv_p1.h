/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACNV_p1_H
#define MDTPREPDATACNV_p1_H

//-----------------------------------------------------------------------------
//
// file:   MdtTwinPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/MdtTwinPrepData.h"
#include "MdtTwinPrepData_p1.h"

class MsgStream;

class MdtTwinPrepDataCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::MdtTwinPrepData, Muon::MdtTwinPrepData_p1 >
{
public:
    MdtTwinPrepDataCnv_p1()  {}

    void persToTrans(   const Muon::MdtTwinPrepData_p1  *persObj,
                        Muon::MdtTwinPrepData           *transObj,
                        MsgStream                   &log );
    void transToPers(   const Muon::MdtTwinPrepData     *transObj,
                        Muon::MdtTwinPrepData_p1        *persObj,
                        MsgStream                   &log );
};

#endif 
