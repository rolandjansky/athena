/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGITCNV_P2_H
#define MMDIGITCNV_P2_H

//-----------------------------------------------------------------------------
//
// file:   MM_DigitCnv_p2.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonDigitContainer/MmDigit.h"
#include "MuonEventTPCnv/MuonDigitContainer/MM_Digit_p2.h"

class MsgStream;

class MM_DigitCnv_p2 : public T_AthenaPoolTPCnvBase< MmDigit, Muon::MM_Digit_p2 >
{
    public:
  MM_DigitCnv_p2() {}

  void persToTrans( const Muon::MM_Digit_p2 *persObj,
                    MmDigit    *transObj,
                    MsgStream                &log );
  void transToPers( const MmDigit    *transObj,
                    Muon::MM_Digit_p2 *persObj,
                    MsgStream                &log );
protected:        
};

#endif
