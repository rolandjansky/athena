/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMPREPDATACNV_P1_H
#define MMPREPDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MMPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonPrepRawData/MMPrepData.h"
#include "MMPrepData_p1.h"

class MsgStream;

class MMPrepDataCnv_p1
   : public T_AthenaPoolTPPolyCnvBase< Trk::PrepRawData, Muon::MMPrepData, Muon::MMPrepData_p1 >
{
    public:
  MMPrepDataCnv_p1() {}

  Muon::MMPrepData
  createMMPrepData ( const Muon::MMPrepData_p1 *persObj,
                     const MuonGM::MMReadoutElement* detEl,
                     MsgStream & log );

  void persToTrans( const Muon::MMPrepData_p1 *persObj,
                    Muon::MMPrepData    *transObj,
                    MsgStream                &log );
  void transToPers( const Muon::MMPrepData    *transObj,
                    Muon::MMPrepData_p1 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
