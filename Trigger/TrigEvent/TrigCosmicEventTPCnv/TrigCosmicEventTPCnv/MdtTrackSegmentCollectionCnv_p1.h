/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCollectionCnv_p1
 *
 * @brief transient-persistent converter for MdtTrackSegmentCollection_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: MdtTrackSegmentCollectionCnv_p1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCONTAINER_CNV_P1_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCosmicEvent/MdtTrackSegmentCollection.h"

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCnv_p1.h"

class MdtTrackSegmentCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< MdtTrackSegmentCollection, MdtTrackSegmentCollection_p1, MdtTrackSegmentCnv_p1 >

{
 
 public:
  MdtTrackSegmentCollectionCnv_p1() {}
 
}; //end of class definitions
 
#endif
