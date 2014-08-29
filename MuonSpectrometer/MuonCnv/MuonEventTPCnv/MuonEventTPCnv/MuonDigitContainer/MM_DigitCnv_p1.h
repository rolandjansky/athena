/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGITCNV_P1_H
#define MMDIGITCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   MM_DigitCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonDigitContainer/MmDigit.h"
#include "MuonEventTPCnv/MuonDigitContainer/MM_Digit_p1.h"

class MsgStream;

class MM_DigitCnv_p1 : public T_AthenaPoolTPCnvBase< MmDigit, Muon::MM_Digit_p1 >
{
    public:
  MM_DigitCnv_p1() {}

  void persToTrans( const Muon::MM_Digit_p1 *persObj,
                    MmDigit    *transObj,
                    MsgStream                &log );
  void transToPers( const MmDigit    *transObj,
                    Muon::MM_Digit_p1 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
