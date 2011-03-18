/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

#ifndef TTBARLJMETCNV_P1_H
#define TTBARLJMETCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "TopEvent/TTbarLJMet.h"

#include "MissingETEvent/MissingET.h"

class TTbarLJMet_p1;

class MsgStream;

template<class LeptonCollection>
struct TTbarLJMetCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarLJMet<LeptonCollection>, TTbarLJMet_p1>
{
  DataLinkCnv_p1< DataLink<MissingET> > m_metLinkCnv;

public: 
  virtual void persToTrans( const TTbarLJMet_p1* persObj, 
                            TopRec::TTbarLJMet<LeptonCollection>* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarLJMet<LeptonCollection>* transObj, 
                            TTbarLJMet_p1* persObj, 
                            MsgStream& msg );
  
};

#endif //TTBARLJMETCNV_P1_H
