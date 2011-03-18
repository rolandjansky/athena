/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETCNV_P1_H
#define TOPJETCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "JetEvent/JetCollection.h"

#include "TopEvent/TopJet.h"
#include "TopEventTPCnv/TopJet_p1.h"

class MsgStream;

struct TopJetCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TopJet, TopJet_p1>
{
  ElementLinkCnv_p3< ElementLink<JetCollection> > m_jetCnv;

public: 
  virtual void persToTrans( const TopJet_p1* persObj, 
                            TopRec::TopJet* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TopJet* transObj, 
                            TopJet_p1* persObj, 
                            MsgStream& msg );
  
};

#endif/*TOPJETCNV_P1_H*/
