/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigEventAthenaPool
 * @Class  : MdtTrackSegmentCollectionCnv
 *
 * @brief transient persistent converter for MdtTrackSegmentCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * V2 06.01.2011 giagu - Stefano.Giagu@cern.ch
 **********************************************************************************/
#ifndef TRIGEVENTATHENAPOOL_MDTTRACKSEGMENTCOLLECTION_CNV_H
#define TRIGEVENTATHENAPOOL_MDTTRACKSEGMENTCOLLECTION_CNV_H
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCosmicEvent/MdtTrackSegmentCollection.h"
#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollection_p2.h"

typedef MdtTrackSegmentCollection_p2   MdtTrackSegmentCollection_PERS;
 
typedef T_AthenaPoolCustomCnv<MdtTrackSegmentCollection, MdtTrackSegmentCollection_PERS> MdtTrackSegmentCollectionCnvBase;
 
class MdtTrackSegmentCollectionCnv : public MdtTrackSegmentCollectionCnvBase {
  
  friend class CnvFactory<MdtTrackSegmentCollectionCnv>;
  
 protected:
  
  MdtTrackSegmentCollectionCnv( ISvcLocator *svcloc ): MdtTrackSegmentCollectionCnvBase( svcloc ) {}
  
  virtual MdtTrackSegmentCollection_PERS *createPersistent( MdtTrackSegmentCollection *transObj);
  virtual MdtTrackSegmentCollection      *createTransient();
  
};//end of class definitions

#endif //MDTTRACKSEGMENTCOLLECTION_CNV_H
