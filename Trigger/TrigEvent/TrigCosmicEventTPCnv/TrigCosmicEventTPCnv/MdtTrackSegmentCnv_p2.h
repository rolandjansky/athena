/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCnv_p2
 *
 * @brief transient-persistent converter for MdtTrackSegment_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * V2 06.01.2011 gagu - Stefano.Giagu@cern.ch
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCNV_P2_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCNV_P2_H

//converter base
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

//MdtTrackSegment tarnsient and persistent class
#include "TrigCosmicEvent/MdtTrackSegment.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p2.h"

class MsgStream;

class MdtTrackSegmentCnv_p2 : public T_AthenaPoolTPCnvBase<MdtTrackSegment, MdtTrackSegment_p2> {

 public:

  MdtTrackSegmentCnv_p2() {}

  virtual void persToTrans(const MdtTrackSegment_p2* persObj, MdtTrackSegment* transObj, MsgStream &log);
  virtual void transToPers(const MdtTrackSegment* transObj, MdtTrackSegment_p2* persObj, MsgStream &log);
 
 protected:

};

#endif
