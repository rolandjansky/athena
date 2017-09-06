/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EXAMPLETRACKCNV_P1_H
#define EXAMPLETRACKCNV_P1_H

/**
 *  @file ExampleTrackCnv_p1.h
 *  @brief This file contains the class definition for the ExampleTrackCnv_p1 class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: ExampleTrackCnv_p1.h,v 1.1 2008-03-25 22:03:54 gemmeren Exp $
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/NavigableCnv_p2.h"
#include "AthenaPoolExampleData/ExampleTrack.h"
#include "AthenaPoolExampleConverter/ExampleTrack_p1.h"

class ExampleTrackCnv_p1 : public T_AthenaPoolTPCnvBase<ExampleTrack, ExampleTrack_p1> { 

public:
   ExampleTrackCnv_p1() {}

   /** Method creating the transient representation of @c ExampleTrack
    *  from its persistent representation @c ExampleTrack_p1
    */
   virtual void persToTrans(const ExampleTrack_p1* persObj, ExampleTrack* transObj, MsgStream& msg) {
      transObj->setDetector (persObj->m_detector);
      elCnv.persToTrans(&persObj->m_elementlink1, transObj->getElementLink1(), msg);
      elCnv.persToTrans(&persObj->m_elementlink2, transObj->getElementLink2(), msg);
      elvCnv.persToTrans(&persObj->m_elementlinkvector, transObj->getElementLinkVector(), msg);
      navCnv.persToTrans(persObj->m_navigable, *transObj->getNavigable(), msg);
      wNavCnv.persToTrans(persObj->m_weightednavigable, *transObj->getWeightedNavigable(), msg);
      transObj->setPT (persObj->m_pt);
      transObj->setEta (persObj->m_eta);
      transObj->setPhi (persObj->m_phi);
   }

   /** Method creating the persistent representation @c ExampleTrack_p1
    *  from its transient representation @c ExampleTrack
    */
   virtual void transToPers(const ExampleTrack* transObj, ExampleTrack_p1* persObj, MsgStream& msg) {
      persObj->m_detector = transObj->getDetector();
      elCnv.transToPers(transObj->getElementLink1(), &persObj->m_elementlink1, msg);
      elCnv.transToPers(transObj->getElementLink2(), &persObj->m_elementlink2, msg);
      elvCnv.transToPers(transObj->getElementLinkVector(), &persObj->m_elementlinkvector, msg);
      navCnv.transToPers(*transObj->getNavigable(), persObj->m_navigable, msg);
      wNavCnv.transToPers(*transObj->getWeightedNavigable(), persObj->m_weightednavigable, msg);
      persObj->m_pt = transObj->getPT();
      persObj->m_eta = transObj->getEta();
      persObj->m_phi = transObj->getPhi();
   }

private:
   ElementLinkCnv_p3<ElementLink<ExampleHitContainer> > elCnv;
   ElementLinkVectorCnv_p1<ElementLinkVector<ExampleHitContainer> > elvCnv;
   NavigableCnv_p2<Navigable<ExampleHitContainer> > navCnv;
   NavigableCnv_p2<Navigable<ExampleHitContainer, double> > wNavCnv;
}; 

#endif
