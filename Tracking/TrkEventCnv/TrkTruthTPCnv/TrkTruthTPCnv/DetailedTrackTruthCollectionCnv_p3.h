/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008
// Olivier.Arnaez@cern.ch, 2015


#ifndef DETAILEDTRACKTRUTHCOLLECTIONCNV_P3_H 
#define DETAILEDTRACKTRUTHCOLLECTIONCNV_P3_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class DetailedTrackTruthCollection;
namespace Trk { class DetailedTrackTruthCollection_p3; }


class DetailedTrackTruthCollectionCnv_p3 
  : public T_AthenaPoolTPCnvBase<DetailedTrackTruthCollection,
				 Trk::DetailedTrackTruthCollection_p3>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::DetailedTrackTruthCollection
   *  from its persistent representation @c DetailedTrackTruthCollection_p3
   */
  virtual void persToTrans( const Trk::DetailedTrackTruthCollection_p3* persObj, 
                            DetailedTrackTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c DetailedTrackTruthCollection_p3
   *  from its transient representation @c Analysis::DetailedTrackTruthCollection
   */
  virtual void transToPers( const DetailedTrackTruthCollection* transObj, 
                            Trk::DetailedTrackTruthCollection_p3* persObj, 
                            MsgStream& msg );

}; 

#endif/*DETAILEDTRACKTRUTHCOLLECTIONCNV_P3_H*/
