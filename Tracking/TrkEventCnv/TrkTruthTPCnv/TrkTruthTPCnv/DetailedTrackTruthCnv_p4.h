/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2008
// Olivier.Arnaez@cern.ch, 2015


#ifndef DETAILEDTRACKTRUTHCNV_P4_H 
#define DETAILEDTRACKTRUTHCNV_P4_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class DetailedTrackTruth;
namespace Trk { class DetailedTrackTruth_p4; }


class DetailedTrackTruthCnv_p4 
  : public T_AthenaPoolTPCnvBase<DetailedTrackTruth,
				 Trk::DetailedTrackTruth_p4>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::DetailedTrackTruth
   *  from its persistent representation @c DetailedTrackTruth_p4
   */
  virtual void persToTrans( const Trk::DetailedTrackTruth_p4* persObj, 
                            DetailedTrackTruth* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c DetailedTrackTruth_p4
   *  from its transient representation @c Analysis::DetailedTrackTruth
   */
  virtual void transToPers( const DetailedTrackTruth* transObj, 
                            Trk::DetailedTrackTruth_p4* persObj, 
                            MsgStream& msg );

}; 

#endif/*DETAILEDTRACKTRUTHCNV_P4_H*/
