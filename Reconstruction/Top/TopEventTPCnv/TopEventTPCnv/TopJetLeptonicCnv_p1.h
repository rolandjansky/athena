/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETLEPTONICCNV_P1_H
#define TOPJETLEPTONICCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TopEvent/TopJetLeptonic.h"

class TopJetLeptonic_p1;
class MsgStream;

template<class LeptonCollection>
struct TopJetLeptonicCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TopJetLeptonic<LeptonCollection>, TopJetLeptonic_p1>
{
  ElementLinkCnv_p3< ElementLink<LeptonCollection> > m_leptonCnv;
public: 
  virtual void persToTrans( const TopJetLeptonic_p1* persObj, 
                            TopRec::TopJetLeptonic<LeptonCollection>* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TopJetLeptonic<LeptonCollection>* transObj, 
                            TopJetLeptonic_p1* persObj, 
                            MsgStream& msg );
  
};

#endif/*TOPJETLEPTONICCNV_P1_H*/
