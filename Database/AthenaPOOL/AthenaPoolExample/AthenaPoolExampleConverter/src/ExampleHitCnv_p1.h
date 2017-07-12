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
#include "AthenaPoolExampleData/ExampleHit.h"
#include "AthenaPoolExampleConverter/ExampleHit_p1.h"


class ExampleHitCnv_p1 : public T_AthenaPoolTPCnvBase<ExampleHit, ExampleHit_p1> { 

public:
   ExampleHitCnv_p1() {}

   /** Method creating the transient representation of @c ExampleHit
    *  from its persistent representation @c ExampleHit_p1
    */
   virtual void persToTrans(const ExampleHit_p1* persObj, ExampleHit* transObj, MsgStream&/* msg*/) {
      transObj->setX (persObj->m_vec.x());
      transObj->setY (persObj->m_vec.y());
      transObj->setZ (persObj->m_vec.z());
      transObj->setDetector (persObj->m_detector);
   }

   /** Method creating the persistent representation @c ExampleHit_p1
    *  from its transient representation @c ExampleHit
    */
   virtual void transToPers(const ExampleHit* transObj, ExampleHit_p1* persObj, MsgStream&/* msg*/) {
      persObj->m_vec.setX (transObj->getX());
      persObj->m_vec.setY (transObj->getY());
      persObj->m_vec.setZ (transObj->getZ());
      persObj->m_detector = transObj->getDetector();
   }
}; 

#endif
