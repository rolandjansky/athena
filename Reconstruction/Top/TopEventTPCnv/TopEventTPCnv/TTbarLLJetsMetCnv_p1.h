/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLLJETSMETCNV_P1_H
#define TTBARLLJETSMETCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "TopEvent/TTbarLLJetsMet.h"

#include "MissingETEvent/MissingET.h"

class TTbarLLJetsMet_p1;

class MsgStream;

template<class LeptonCollection1,class LeptonCollection2>
struct TTbarLLJetsMetCnv_p1 
  : public T_AthenaPoolTPCnvBase< TopRec::TTbarLLJetsMet < LeptonCollection1, LeptonCollection2 >, TTbarLLJetsMet_p1 >
{
  DataLinkCnv_p1< DataLink<MissingET> > m_metLinkCnv;

public: 
  virtual void persToTrans( const TTbarLLJetsMet_p1* persObj, 
                            TopRec::TTbarLLJetsMet<LeptonCollection1, LeptonCollection2>* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarLLJetsMet<LeptonCollection1, LeptonCollection2>* transObj, 
                            TTbarLLJetsMet_p1* persObj, 
                            MsgStream& msg );
  
};

#endif/*TTBARLLJETSMETCNV_P1_H*/
