/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLLCNV_P1_H
#define TTBARLLCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TopEvent/TTbarLL.h"

class TTbarLL_p1;
class MsgStream;

template<class LeptonCollection1, class LeptonCollection2>
struct TTbarLLCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarLL<LeptonCollection1,LeptonCollection2>, TTbarLL_p1>
{
  ElementLinkCnv_p3<ElementLink<LeptonCollection1> > m_lepton1Cnv;
  ElementLinkCnv_p3<ElementLink<LeptonCollection2> > m_lepton2Cnv;

public: 
  virtual void persToTrans( const TTbarLL_p1* persObj, 
                            TopRec::TTbarLL<LeptonCollection1,LeptonCollection2>* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarLL<LeptonCollection1,LeptonCollection2>* transObj, 
                            TTbarLL_p1* persObj, 
                            MsgStream& msg );
  
};

#endif/*TTBARLLCNV_P1_H*/
