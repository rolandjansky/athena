/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MaterialEffectsBaseCnv_p1.h
//
//-----------------------------------------------------------------------------

#ifndef MATERIAL_EFFECTS_BASE_CNV_P1_H
#define MATERIAL_EFFECTS_BASE_CNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBase_p1.h"

class MsgStream;

/**
   @brief Converter for T/P separation of the 
   TrkMaterialOnTrack/MaterialEffectsBase class.

   @author Wolfgang Liebig <http://consult.cern.ch/xwho/people/54608>
 */

class MaterialEffectsBaseCnv_p1: public T_AthenaPoolTPAbstractPolyCnvBase
< Trk::MaterialEffectsBase, Trk::MaterialEffectsBase,
  Trk::MaterialEffectsBase_p1 >
{
    public:
        MaterialEffectsBaseCnv_p1() {}
        virtual void persToTrans( const Trk :: MaterialEffectsBase_p1 *persObj,
                                        Trk :: MaterialEffectsBase    *transObj,
                                        MsgStream                     &log );
        virtual void transToPers( const Trk :: MaterialEffectsBase    *transObj,
                                        Trk :: MaterialEffectsBase_p1 *persObj,
                                        MsgStream                     &log );
};

#endif
