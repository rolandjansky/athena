/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007

#ifndef TRUTHTRAJECTORYCNV_P1_H 
#define TRUTHTRAJECTORYCNV_P1_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TruthTrajectory;
namespace Trk { 
  class TruthTrajectory_p1; 
}


class TruthTrajectoryCnv_p1 
  : public T_AthenaPoolTPCnvBase<TruthTrajectory,
				 Trk::TruthTrajectory_p1>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::TruthTrajectory
   *  from its persistent representation @c TruthTrajectory_p1
   */
  virtual void persToTrans( const Trk::TruthTrajectory_p1* persObj, 
                            TruthTrajectory* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TruthTrajectory_p1
   *  from its transient representation @c Analysis::TruthTrajectory
   */
  virtual void transToPers( const TruthTrajectory* transObj, 
                            Trk::TruthTrajectory_p1* persObj, 
                            MsgStream& msg );

}; 

#endif/*TRUTHTRAJECTORYCNV_P1_H*/
