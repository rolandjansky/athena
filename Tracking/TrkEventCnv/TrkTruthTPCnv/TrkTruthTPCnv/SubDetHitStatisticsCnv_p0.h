/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

// Andrei.Gaponenko@cern.ch, 2007


#ifndef SUBDETHITSTATISTICSCNV_P0_H 
#define SUBDETHITSTATISTICSCNV_P0_H 

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;
class SubDetHitStatistics;
namespace Trk { 
  class SubDetHitStatistics_p0; 
}


class SubDetHitStatisticsCnv_p0 
  : public T_AthenaPoolTPCnvConstBase<SubDetHitStatistics,
                                      Trk::SubDetHitStatistics_p0>
{ 
public: 
  using base_class::persToTrans;
  using base_class::transToPers;

  /** Method creating the transient representation of @c Analysis::SubDetHitStatistics
   *  from its persistent representation @c SubDetHitStatistics_p0
   */
  virtual void persToTrans( const Trk::SubDetHitStatistics_p0* persObj, 
                            SubDetHitStatistics* transObj, 
                            MsgStream& msg ) const override;
  
  /** Method creating the persistent representation @c SubDetHitStatistics_p0
   *  from its transient representation @c Analysis::SubDetHitStatistics
   */
  virtual void transToPers( const SubDetHitStatistics* transObj, 
                            Trk::SubDetHitStatistics_p0* persObj, 
                            MsgStream& msg ) const override;

}; 

#endif/*SUBDETHITSTATISTICSCNV_P0_H*/
