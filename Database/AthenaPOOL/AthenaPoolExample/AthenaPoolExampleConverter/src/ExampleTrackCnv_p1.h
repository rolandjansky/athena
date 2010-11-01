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

#define private public
#define protected public
#include "AthenaPoolExampleData/ExampleTrack.h"
#include "AthenaPoolExampleConverter/ExampleTrack_p1.h"
#undef private
#undef protected

class ExampleTrackCnv_p1 : public T_AthenaPoolTPCnvBase<ExampleTrack, ExampleTrack_p1> { 

public:
   ExampleTrackCnv_p1() {}

   /** Method creating the transient representation of @c ExampleTrack
    *  from its persistent representation @c ExampleTrack_p1
    */
   virtual void persToTrans(const ExampleTrack_p1* persObj, ExampleTrack* transObj, MsgStream& msg) {
      transObj->m_detector = persObj->m_detector;
      elCnv.persToTrans(&persObj->m_elementlink1, &transObj->m_elementlink1, msg);
      elCnv.persToTrans(&persObj->m_elementlink2, &transObj->m_elementlink2, msg);
      elvCnv.persToTrans(&persObj->m_elementlinkvector, &transObj->m_elementlinkvector, msg);
      navCnv.persToTrans(persObj->m_navigable, transObj->m_navigable, msg);
      wNavCnv.persToTrans(persObj->m_weightednavigable, transObj->m_weightednavigable, msg);
      transObj->m_pt = persObj->m_pt;
      transObj->m_eta = persObj->m_eta;
      transObj->m_phi = persObj->m_phi;
   }

   /** Method creating the persistent representation @c ExampleTrack_p1
    *  from its transient representation @c ExampleTrack
    */
   virtual void transToPers(const ExampleTrack* transObj, ExampleTrack_p1* persObj, MsgStream& msg) {
      persObj->m_detector = transObj->m_detector;
      elCnv.transToPers(&transObj->m_elementlink1, &persObj->m_elementlink1, msg);
      elCnv.transToPers(&transObj->m_elementlink2, &persObj->m_elementlink2, msg);
      elvCnv.transToPers(&transObj->m_elementlinkvector, &persObj->m_elementlinkvector, msg);
      navCnv.transToPers(transObj->m_navigable, persObj->m_navigable, msg);
      wNavCnv.transToPers(transObj->m_weightednavigable, persObj->m_weightednavigable, msg);
      persObj->m_pt = transObj->m_pt;
      persObj->m_eta = transObj->m_eta;
      persObj->m_phi = transObj->m_phi;
   }

private:
   ElementLinkCnv_p3<ElementLink<ExampleHitContainer> > elCnv;
   ElementLinkVectorCnv_p1<ElementLinkVector<ExampleHitContainer> > elvCnv;
   NavigableCnv_p2<Navigable<ExampleHitContainer> > navCnv;
   NavigableCnv_p2<Navigable<ExampleHitContainer, double> > wNavCnv;
}; 

#endif
