/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
//
// Andrei Gaponenko, 2008

#ifndef TTBARLJCNV_P1_H
#define TTBARLJCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"

#include "TopEvent/TTbarLJ.h"

class TTbarLJ_p1;
class MsgStream;

template<class LeptonCollection>
struct TTbarLJCnv_p1 
  : public T_AthenaPoolTPCnvBase<TopRec::TTbarLJ<LeptonCollection>, TTbarLJ_p1>
{
  ElementLinkCnv_p3<ElementLink<LeptonCollection> > m_leptonCnv;

public: 
  virtual void persToTrans( const TTbarLJ_p1* persObj, 
                            TopRec::TTbarLJ<LeptonCollection>* transObj, 
                            MsgStream& msg );
  
  virtual void transToPers( const TopRec::TTbarLJ<LeptonCollection>* transObj, 
                            TTbarLJ_p1* persObj, 
                            MsgStream& msg );
  
};

template <> class T_TPCnv<ElementLink<ElectronContainer>, ElementLinkInt_p3>
  : public ElementLinkCnv_p3< ElementLink<ElectronContainer> > {
public:
};

#endif //TTBARLJCNV_P1_H
