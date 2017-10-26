/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCDIGITCNV_P1_H
#define STGCDIGITCNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   STGC_DigitCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "MuonDigitContainer/sTgcDigit.h"
#include "MuonEventTPCnv/MuonDigitContainer/STGC_Digit_p1.h"

class MsgStream;
/*******************************************************************************/
class STGC_DigitCnv_p1: public T_AthenaPoolTPCnvBase< sTgcDigit, Muon::STGC_Digit_p1 >
{
 public:
  STGC_DigitCnv_p1() {}
  void persToTrans( const Muon::STGC_Digit_p1 *persObj, sTgcDigit    *transObj, MsgStream &log );
  void transToPers( const sTgcDigit    *transObj, Muon::STGC_Digit_p1 *persObj, MsgStream &log );
 protected:        
};
/*******************************************************************************/
#endif
/*******************************************************************************/
