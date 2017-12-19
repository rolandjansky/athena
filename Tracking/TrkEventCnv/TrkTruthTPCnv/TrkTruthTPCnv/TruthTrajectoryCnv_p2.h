/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007
// Olivier.Arnaez@cern.ch, 2015

#ifndef TRUTHTRAJECTORYCNV_P2_H 
#define TRUTHTRAJECTORYCNV_P2_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TruthTrajectory;
namespace Trk { 
  class TruthTrajectory_p2; 
}


class TruthTrajectoryCnv_p2 
  : public T_AthenaPoolTPCnvBase<TruthTrajectory,
				 Trk::TruthTrajectory_p2>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::TruthTrajectory
   *  from its persistent representation @c TruthTrajectory_p2
   */
  virtual void persToTrans( const Trk::TruthTrajectory_p2* persObj, 
                            TruthTrajectory* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TruthTrajectory_p2
   *  from its transient representation @c Analysis::TruthTrajectory
   */
  virtual void transToPers( const TruthTrajectory* transObj, 
                            Trk::TruthTrajectory_p2* persObj, 
                            MsgStream& msg );

}; 

#endif/*TRUTHTRAJECTORYCNV_P2_H*/
