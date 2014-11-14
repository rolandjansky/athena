/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TOPEVENTTPCNV_TTBARLLJETSCNV_P1_H
#define TOPEVENTTPCNV_TTBARLLJETSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TopEvent/TTbarLLJets.h"

class TTbarLLJets_p1;
class MsgStream;

template<class LeptonCollection1, class LeptonCollection2>
struct TTbarLLJetsCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarLLJets<LeptonCollection1,LeptonCollection2>, TTbarLLJets_p1>
{
  ElementLinkCnv_p3<ElementLink<LeptonCollection1> > m_lepton1Cnv;
  ElementLinkCnv_p3<ElementLink<LeptonCollection2> > m_lepton2Cnv;

public: 
  virtual void persToTrans( const TTbarLLJets_p1* persObj, 
                            TopRec::TTbarLLJets<LeptonCollection1,LeptonCollection2>* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarLLJets<LeptonCollection1,LeptonCollection2>* transObj, 
                            TTbarLLJets_p1* persObj, 
                            MsgStream& msg );
  
};

#endif // not TOPEVENTTPCNV_TTBARLLJETSCNV_P1_H
