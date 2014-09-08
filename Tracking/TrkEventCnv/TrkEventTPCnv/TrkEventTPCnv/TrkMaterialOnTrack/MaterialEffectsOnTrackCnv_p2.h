/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MATERIAL_EFFECTS_ON_TRACK_CNV_P2_TRK_H
#define MATERIAL_EFFECTS_ON_TRACK_CNV_P2_TRK_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsOnTrack_p2.h"

class MsgStream;

/** @brief T/P converter v2 for MaterialEffectsOnTrack
    @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
*/
class MaterialEffectsOnTrackCnv_p2: 
  public T_AthenaPoolTPPolyCnvBase< Trk::MaterialEffectsBase,
                                    Trk::MaterialEffectsOnTrack,
                                    Trk::MaterialEffectsOnTrack_p2 >
{
 public:

  MaterialEffectsOnTrackCnv_p2() : m_mefBaseCnv(0) {}

  void persToTrans( const Trk::MaterialEffectsOnTrack_p2 *persObj,
                    Trk::MaterialEffectsOnTrack          *transObj,
                    MsgStream                            &log );
  void transToPers( const Trk::MaterialEffectsOnTrack    *transObj,
                    Trk::MaterialEffectsOnTrack_p2       *persObj,
                    MsgStream                            &log );
 private:
  ITPConverterFor< Trk::MaterialEffectsBase >*  m_mefBaseCnv;

};

#endif
