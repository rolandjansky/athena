/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ESTIMATED_BREM_ON_TRACK_CNV_P1_TRK_H
#define ESTIMATED_BREM_ON_TRACK_CNV_P1_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/EstimatedBremOnTrack_p1.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p1.h"

class MsgStream;

/** @brief T/P converter v1 for EstimatedBremOnTrack
    @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
*/
class EstimatedBremOnTrackCnv_p1: 
     public T_AthenaPoolTPPolyCnvBase< Trk::MaterialEffectsBase,
                                       Trk::EstimatedBremOnTrack,
                                       Trk::EstimatedBremOnTrack_p1 >
{
 public:

  EstimatedBremOnTrackCnv_p1() : m_mefBaseCnv(0)  {}

  void persToTrans( const Trk::EstimatedBremOnTrack_p1 *persObj,
                    Trk::EstimatedBremOnTrack          *transObj,
                    MsgStream                          &log );
  void transToPers( const Trk::EstimatedBremOnTrack    *transObj,
                    Trk::EstimatedBremOnTrack_p1       *persObj,
                    MsgStream                          &log );
 private:
  MaterialEffectsBaseCnv_p1*  m_mefBaseCnv;

};

#endif
