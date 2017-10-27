/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007
// Olivier.Arnaez@cern.ch, 2015


#ifndef TRACKTRUTHCOLLECTIONCNV_P2_H 
#define TRACKTRUTHCOLLECTIONCNV_P2_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class TrackTruthCollection;
namespace Trk { class TrackTruthCollection_p2; }


class TrackTruthCollectionCnv_p2 
  : public T_AthenaPoolTPCnvBase<TrackTruthCollection,
				 Trk::TrackTruthCollection_p2>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::TrackTruthCollection
   *  from its persistent representation @c TrackTruthCollection_p2
   */
  virtual void persToTrans( const Trk::TrackTruthCollection_p2* persObj, 
                            TrackTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TrackTruthCollection_p2
   *  from its transient representation @c Analysis::TrackTruthCollection
   */
  virtual void transToPers( const TrackTruthCollection* transObj, 
                            Trk::TrackTruthCollection_p2* persObj, 
                            MsgStream& msg );

}; 

#endif/*TRACKTRUTHCOLLECTIONCNV_P2_H*/
