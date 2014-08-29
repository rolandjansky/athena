/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTPREPDATACNV_P1_H
#define MDTPREPDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MdtPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------

#include "MuonPrepRawData/MdtPrepData.h"
#include "MdtPrepData_p1.h"

#include "TrkEventTPCnv/TrkPrepRawData/PrepRawDataCnv_p1.h"

class MsgStream;

class MdtPrepDataCnv_p1
    : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::MdtPrepData, Muon::MdtPrepData_p1 >
{
public:
    MdtPrepDataCnv_p1() : m_prdCnv(0)  {}

    void persToTrans(   const Muon::MdtPrepData_p1  *persObj,
                        Muon::MdtPrepData           *transObj,
                        MsgStream                   &log );
    void transToPers(   const Muon::MdtPrepData     *transObj,
                        Muon::MdtPrepData_p1        *persObj,
                        MsgStream                   &log );

protected:        
    PrepRawDataCnv_p1       *m_prdCnv;
};

#endif 
