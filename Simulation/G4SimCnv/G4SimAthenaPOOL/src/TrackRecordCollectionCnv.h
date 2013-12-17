/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4SIM_TRACKRECORDCOLLECTIONCNV
#define G4SIM_TRACKRECORDCOLLECTIONCNV

#include "TrackRecord/TrackRecordCollection.h"
#include "G4SimTPCnv/TrackRecordCollection_p1.h"
#include "G4SimTPCnv/TrackRecordCollection_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef TrackRecordCollection_p2  TrackRecordCollection_PERS;

class TrackRecordCollectionCnv  : public T_AthenaPoolCustomCnv<TrackRecordCollection, TrackRecordCollection_PERS > {
  friend class CnvFactory<TrackRecordCollectionCnv>;
protected:
  TrackRecordCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<TrackRecordCollection, TrackRecordCollection_PERS >( svcloc) {}
  TrackRecordCollection_PERS* createPersistent(TrackRecordCollection* transCont);
  TrackRecordCollection*   createTransient ();
};

// #include "TrackRecordCollectionCnv.icc"

#endif

