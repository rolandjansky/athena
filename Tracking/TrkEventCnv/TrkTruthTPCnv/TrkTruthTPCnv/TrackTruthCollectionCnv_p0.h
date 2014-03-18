/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef TRACKTRUTHCOLLECTIONCNV_P0_H 
#define TRACKTRUTHCOLLECTIONCNV_P0_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class TrackTruthCollection;
class TrackTruthVector;

namespace Trk {
  typedef TrackTruthVector  TrackTruthCollection_p0;
}

class TrackTruthCollectionCnv_p0 
  : public T_AthenaPoolTPCnvBase<TrackTruthCollection,
				 Trk::TrackTruthCollection_p0>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::TrackTruthCollection
   *  from its persistent representation @c TrackTruthCollection_p0
   */
  virtual void persToTrans( const Trk::TrackTruthCollection_p0* persObj, 
                            TrackTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TrackTruthCollection_p0
   *  from its transient representation @c Analysis::TrackTruthCollection
   */
  virtual void transToPers( const TrackTruthCollection* transObj, 
                            Trk::TrackTruthCollection_p0* persObj, 
                            MsgStream& msg );

}; 

#endif/*TRACKTRUTHCOLLECTIONCNV_P0_H*/
