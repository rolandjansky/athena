/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCollection_tlp1
 *
 * @brief "top-level" persistent partner for MdtTrackSegmentCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: MdtTrackSegmentCollection_tlp1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCONTAINER_TLP1_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCONTAINER_TLP1_H

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p1.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegment_p1.h"

class MdtTrackSegmentCollection_tlp1 
{
   
 public:
   
  MdtTrackSegmentCollection_tlp1(){}
 
  std::vector<MdtTrackSegmentCollection_p1>      m_mdtTrackSegmentCollectionVec;
  std::vector<MdtTrackSegment_p1>                m_mdtTrackSegmentVec;
     
};//end of class definitions
 
#endif
