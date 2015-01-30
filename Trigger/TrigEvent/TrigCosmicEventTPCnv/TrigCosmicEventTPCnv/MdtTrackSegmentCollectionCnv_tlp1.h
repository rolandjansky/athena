/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for MdtTrackSegmentCollection_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: MdtTrackSegmentCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCOLLECTION_CNV_TLP1_H
#define  TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_p1.h"   

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_tlp1.h"
 
 
class MdtTrackSegmentCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< MdtTrackSegmentCollectionCnv_p1, MdtTrackSegmentCollection_tlp1 >
{
 
 public:
 
  MdtTrackSegmentCollectionCnv_tlp1();
  virtual ~MdtTrackSegmentCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( MdtTrackSegmentCollection_tlp1 *storage );
 
 protected: 
  MdtTrackSegmentCollectionCnv_p1    m_mdtTrackSegmentCollectionCnv;
  MdtTrackSegmentCnv_p1              m_mdtTrackSegmentCnv;
 
}; //end of class definitions
 

#endif
