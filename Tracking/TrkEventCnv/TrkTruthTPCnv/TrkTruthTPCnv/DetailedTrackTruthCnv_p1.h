/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef DETAILEDTRACKTRUTHCNV_P1_H 
#define DETAILEDTRACKTRUTHCNV_P1_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class DetailedTrackTruth;
namespace Trk { class DetailedTrackTruth_p1; }


class DetailedTrackTruthCnv_p1 
  : public T_AthenaPoolTPCnvBase<DetailedTrackTruth,
				 Trk::DetailedTrackTruth_p1>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::DetailedTrackTruth
   *  from its persistent representation @c DetailedTrackTruth_p1
   */
  virtual void persToTrans( const Trk::DetailedTrackTruth_p1* persObj, 
                            DetailedTrackTruth* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c DetailedTrackTruth_p1
   *  from its transient representation @c Analysis::DetailedTrackTruth
   */
  virtual void transToPers( const DetailedTrackTruth* transObj, 
                            Trk::DetailedTrackTruth_p1* persObj, 
                            MsgStream& msg );

}; 

#endif/*DETAILEDTRACKTRUTHCNV_P1_H*/
