/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MaterialEffectsBaseCnv_p2.h
//
//-----------------------------------------------------------------------------

#ifndef MATERIAL_EFFECTS_BASE_CNV_P2_H
#define MATERIAL_EFFECTS_BASE_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBase_p1.h"
#include "TrkSurfaces/Surface.h"

class MsgStream;

/**
   @brief Converter for T/P separation of the 
   TrkMaterialOnTrack/MaterialEffectsBase class.

  Still using _p1 object, because main purpose of update is to use SurfaceCnv_p2 

   @author edward.moyse@cern.ch
 */

class MaterialEffectsBaseCnv_p2: public T_AthenaPoolTPAbstractPolyCnvBase
< Trk::MaterialEffectsBase, Trk::MaterialEffectsBase,
  Trk::MaterialEffectsBase_p1 >
{
    public:
        MaterialEffectsBaseCnv_p2() {}
        virtual void persToTrans( const Trk :: MaterialEffectsBase_p1 *persObj,
                                        Trk :: MaterialEffectsBase    *transObj,
                                        MsgStream                     &log );
        virtual void transToPers( const Trk :: MaterialEffectsBase    *transObj,
                                        Trk :: MaterialEffectsBase_p1 *persObj,
                                        MsgStream                     &log );
                                        
    private:
};

#endif
