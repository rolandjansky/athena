/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TOPJETHADRONICCNV_P2_H
#define TOPJETHADRONICCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TopEvent/TopJetHadronic.h"

class TopJetHadronic_p2;
class MsgStream;

struct TopJetHadronicCnv_p2 
  : public T_AthenaPoolTPCnvBase<TopRec::TopJetHadronic, TopJetHadronic_p2>
{
public: 
  virtual void persToTrans( const TopJetHadronic_p2* persObj, 
                            TopRec::TopJetHadronic* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TopJetHadronic* transObj, 
                            TopJetHadronic_p2* persObj, 
                            MsgStream& msg );
  
};

#endif/*TOPJETHADRONICCNV_P2_H*/
