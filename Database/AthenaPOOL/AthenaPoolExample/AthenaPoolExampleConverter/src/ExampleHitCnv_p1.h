/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLEHITCNV_P1_H
#define EXAMPLEHITCNV_P1_H

/**
 *  @file ExampleHitCnv_p1.h
 *  @brief This file contains the class definition for the ExampleHitCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleHitCnv_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#define private public
#define protected public
#include "AthenaPoolExampleData/ExampleHit.h"
#include "AthenaPoolExampleConverter/ExampleHit_p1.h"
#undef private
#undef protected


class ExampleHitCnv_p1 : public T_AthenaPoolTPCnvBase<ExampleHit, ExampleHit_p1> { 

public:
   ExampleHitCnv_p1() {}

   /** Method creating the transient representation of @c ExampleHit
    *  from its persistent representation @c ExampleHit_p1
    */
   virtual void persToTrans(const ExampleHit_p1* persObj, ExampleHit* transObj, MsgStream&/* msg*/) {
      transObj->m_vec = persObj->m_vec;
      transObj->m_detector = persObj->m_detector;
   }

   /** Method creating the persistent representation @c ExampleHit_p1
    *  from its transient representation @c ExampleHit
    */
   virtual void transToPers(const ExampleHit* transObj, ExampleHit_p1* persObj, MsgStream&/* msg*/) {
      persObj->m_vec = transObj->m_vec;
      persObj->m_detector = transObj->m_detector;
   }
}; 

#endif
