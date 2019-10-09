/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008
// Olivier.Arnaez@cern.ch, 2015


#ifndef DETAILEDTRACKTRUTHCOLLECTIONCNV_P4_H 
#define DETAILEDTRACKTRUTHCOLLECTIONCNV_P4_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class StoreGateSvc;
class MsgStream;
class DetailedTrackTruthCollection;
namespace Trk { class DetailedTrackTruthCollection_p4; }


class DetailedTrackTruthCollectionCnv_p4 
  : public T_AthenaPoolTPCnvBase<DetailedTrackTruthCollection,
				 Trk::DetailedTrackTruthCollection_p4>
{ 
public: 
  /** Method creating the transient representation of @c Analysis::DetailedTrackTruthCollection
   *  from its persistent representation @c DetailedTrackTruthCollection_p4
   */
  virtual void persToTrans( const Trk::DetailedTrackTruthCollection_p4* persObj, 
                            DetailedTrackTruthCollection* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c DetailedTrackTruthCollection_p4
   *  from its transient representation @c Analysis::DetailedTrackTruthCollection
   */
  virtual void transToPers( const DetailedTrackTruthCollection* transObj, 
                            Trk::DetailedTrackTruthCollection_p4* persObj, 
                            MsgStream& msg );

}; 

#endif/*DETAILEDTRACKTRUTHCOLLECTIONCNV_P4_H*/
