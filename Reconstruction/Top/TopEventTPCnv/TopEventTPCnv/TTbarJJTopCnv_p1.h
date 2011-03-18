/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARJJTOPCNV_P1_H
#define TTBARJJTOPCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TopEvent/TTbarJJTop.h"

class TTbarJJTop_p1;
class MsgStream;

struct TTbarJJTopCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarJJTop, TTbarJJTop_p1>
{
public: 
  virtual void persToTrans( const TTbarJJTop_p1* persObj, 
                            TopRec::TTbarJJTop* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarJJTop* transObj, 
                            TTbarJJTop_p1* persObj, 
                            MsgStream& msg );

};

#endif/*TTBARJJTOPCNV_P1_H*/
