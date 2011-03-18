/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLJTOPCNV_P1_H
#define TTBARLJTOPCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopEventTPCnv/TTbarLJCnv_p1.h"

#include "TopEvent/TTbarLJTop.h"

class TTbarLJTop_p1;
class MsgStream;

template<class LeptonCollection>
struct TTbarLJTopCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarLJTop<LeptonCollection>, TTbarLJTop_p1>
{
  TTbarLJCnv_p1<LeptonCollection> m_baseCnv;

public: 
  virtual void persToTrans(const TTbarLJTop_p1* persObj, 
			   TopRec::TTbarLJTop<LeptonCollection>* transObj, 
			   MsgStream& msg );
  
  virtual void transToPers(const TopRec::TTbarLJTop<LeptonCollection>* transObj, 
			   TTbarLJTop_p1* persObj, 
			   MsgStream& msg );
  
};

#endif //TTBARLJTOPCNV_P1_H
