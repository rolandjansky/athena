/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACNV_p2_H
#define MDTPREPDATACNV_p2_H

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataCnv_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/MdtPrepData.h"
#include "MdtPrepData_p2.h"

class MsgStream;

class MdtPrepDataCnv_p2
    : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::MdtPrepData, Muon::MdtPrepData_p2 >
{
public:
    MdtPrepDataCnv_p2()  {}

    Muon::MdtPrepData
    createMdtPrepData( const Muon::MdtPrepData_p2 *persObj,
                       const Identifier& id,
                       const MuonGM::MdtReadoutElement* detEl,
                       MsgStream & log );

    void persToTrans(   const Muon::MdtPrepData_p2  *persObj,
                        Muon::MdtPrepData           *transObj,
                        MsgStream                   &log );
    void transToPers(   const Muon::MdtPrepData     *transObj,
                        Muon::MdtPrepData_p2        *persObj,
                        MsgStream                   &log );
};

#endif 
