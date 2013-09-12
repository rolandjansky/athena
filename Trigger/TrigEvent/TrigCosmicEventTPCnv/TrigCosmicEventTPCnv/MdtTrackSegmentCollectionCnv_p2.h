/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : MdtTrackSegmentCollectionCnv_p2
 *
 * @brief transient-persistent converter for MdtTrackSegmentCollection_p2
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * V2 06.01.2011 giagu - Stefano.Giagu@cern.ch
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCOLLECTION_CNV_P2_H
#define TRIGCOSMICEVENTTPCNV_MDTTRACKSEGMENTCOLLECTION_CNV_P2_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCosmicEvent/MdtTrackSegmentCollection.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p2.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCnv_p2.h"


typedef T_AthenaPoolTPCnvVector< MdtTrackSegmentCollection, MdtTrackSegmentCollection_p2, MdtTrackSegmentCnv_p2 > MdtTrackSegmentCollectionCnv_p2;

#endif
