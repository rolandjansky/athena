/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCollection_p1
 *
 * @brief persistent partner for MdtTrackSegmentCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: MdtTrackSegmentCollection_p1.h,v 1.2 2009-04-01 22:02:52 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCONTAINER_P1_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include <vector>
  
class MdtTrackSegmentCollection_p1 : public std::vector<TPObjRef>
{
 public:
    
  MdtTrackSegmentCollection_p1(){}
  friend class MdtTrackSegmentCollectionCnv_p1;

 private:

  std::vector<TPObjRef> m_mdtTrackSegment;

};// end of class definitions
 
 
#endif
