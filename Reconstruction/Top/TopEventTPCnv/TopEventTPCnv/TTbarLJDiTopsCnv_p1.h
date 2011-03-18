/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Jean-Raphael Lessard, 2009

#ifndef TTBARLJDITOPSCNV_P1_H
#define TTBARLJDITOPSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopEventTPCnv/TTbarLJCnv_p1.h"
#include "TopEvent/TTbarLJDiTops.h"

class TTbarLJDiTops_p1;
class MsgStream;

template<class LeptonCollection>
struct TTbarLJDiTopsCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarLJDiTops<LeptonCollection>, TTbarLJDiTops_p1> 
{
  TTbarLJCnv_p1<LeptonCollection> m_baseCnv;

public:
  virtual void persToTrans(const TTbarLJDiTops_p1* persObj, 
			   TopRec::TTbarLJDiTops<LeptonCollection>* transObj, 
			   MsgStream& msg );
  
  virtual void transToPers(const TopRec::TTbarLJDiTops<LeptonCollection>* transObj, 
			   TTbarLJDiTops_p1* persObj, 
			   MsgStream& msg );
};

#endif //TTBARLJDITOPSCNV_P1_H
