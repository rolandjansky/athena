/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETHADRONICCNV_P1_H
#define TOPJETHADRONICCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TopEvent/TopJetHadronic.h"

class TopJetHadronic_p1;
class MsgStream;

struct TopJetHadronicCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TopJetHadronic, TopJetHadronic_p1>
{
public: 
  virtual void persToTrans( const TopJetHadronic_p1* persObj, 
                            TopRec::TopJetHadronic* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TopJetHadronic* transObj, 
                            TopJetHadronic_p1* persObj, 
                            MsgStream& msg );
  
};

#endif/*TOPJETHADRONICCNV_P1_H*/
