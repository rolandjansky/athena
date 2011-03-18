/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARJJCNV_P1_H
#define TTBARJJCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "TopEvent/TTbarJJ.h"

class TTbarJJ_p1;
class MsgStream;

struct TTbarJJCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarJJ, TTbarJJ_p1>
{
public: 
  virtual void persToTrans( const TTbarJJ_p1* persObj, 
                            TopRec::TTbarJJ* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarJJ* transObj, 
                            TTbarJJ_p1* persObj, 
                            MsgStream& msg );

};

#endif/*TTBARJJCNV_P1_H*/
