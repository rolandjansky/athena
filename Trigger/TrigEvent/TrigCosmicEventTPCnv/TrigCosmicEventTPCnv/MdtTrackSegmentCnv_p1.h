/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCnv_p1
 *
 * @brief transient-persistent converter for MdtTrackSegment_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: MdtTrackSegmentCnv_p1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCNV_P1_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCNV_P1_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//MdtTrackSegment persistent class
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p1.h"


class MsgStream;


class MdtTrackSegmentCnv_p1 : public T_AthenaPoolTPCnvBase<MdtTrackSegment, MdtTrackSegment_p1> 
{

 public:

  MdtTrackSegmentCnv_p1() {}

  virtual void persToTrans(const MdtTrackSegment_p1* persObj, MdtTrackSegment* transObj, MsgStream &log);
  virtual void transToPers(const MdtTrackSegment* transObj, MdtTrackSegment_p1* persObj, MsgStream &log);
 
};

#endif
