/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARJETSBASECNV_P1_H
#define TTBARJETSBASECNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TopEvent/TTbarJetsBase.h"

class TTbarJetsBase_p1;
class MsgStream;

struct TTbarJetsBaseCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarJetsBase, TTbarJetsBase_p1>
{
public: 
  virtual void persToTrans( const TTbarJetsBase_p1* persObj, 
                            TopRec::TTbarJetsBase* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarJetsBase* transObj, 
                            TTbarJetsBase_p1* persObj, 
                            MsgStream& msg );

};

#endif/*TTBARJETSBASECNV_P1_H*/
