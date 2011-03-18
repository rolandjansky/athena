/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Jean-Raphael Lessard, 2009

#ifndef TTBARLJDITOPSFITTEDCNV_P1_H
#define TTBARLJDITOPSFITTEDCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopEventTPCnv/TTbarLJDiTopsCnv_p1.h"

#include "TopEvent/TTbarLJDiTopsFitted.h"

class TTbarLJDiTopsFitted_p1;
class MsgStream;

template<class LeptonCollection>
struct TTbarLJDiTopsFittedCnv_p1 : public T_AthenaPoolTPCnvBase<TopRec::TTbarLJDiTopsFitted<LeptonCollection>, TTbarLJDiTopsFitted_p1> {
  TTbarLJCnv_p1<LeptonCollection> m_baseCnv;

  public:
  virtual void persToTrans(const TTbarLJDiTopsFitted_p1* persObj, 
			   TopRec::TTbarLJDiTopsFitted<LeptonCollection>* transObj, 
			   MsgStream& msg );
  
  virtual void transToPers(const TopRec::TTbarLJDiTopsFitted<LeptonCollection>* transObj, 
			   TTbarLJDiTopsFitted_p1* persObj, 
			   MsgStream& msg );
};

#endif //TTBARLJDITOPSFITTEDCNV_P1_H
