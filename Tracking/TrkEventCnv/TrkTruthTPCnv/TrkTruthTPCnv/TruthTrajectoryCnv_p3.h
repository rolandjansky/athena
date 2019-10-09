/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007
// Olivier.Arnaez@cern.ch, 2015

#ifndef TRUTHTRAJECTORYCNV_P3_H 
#define TRUTHTRAJECTORYCNV_P3_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class TruthTrajectory;
namespace Trk { 
  class TruthTrajectory_p3; 
}


class TruthTrajectoryCnv_p3 
  : public T_AthenaPoolTPCnvBase<TruthTrajectory,
				 Trk::TruthTrajectory_p3>
{ 
public: 
  
  /** Method creating the transient representation of @c Analysis::TruthTrajectory
   *  from its persistent representation @c TruthTrajectory_p3
   */
  virtual void persToTrans( const Trk::TruthTrajectory_p3* persObj, 
                            TruthTrajectory* transObj, 
                            MsgStream& msg );
  
  /** Method creating the persistent representation @c TruthTrajectory_p3
   *  from its transient representation @c Analysis::TruthTrajectory
   */
  virtual void transToPers( const TruthTrajectory* transObj, 
                            Trk::TruthTrajectory_p3* persObj, 
                            MsgStream& msg );

}; 

#endif/*TRUTHTRAJECTORYCNV_P3_H*/
