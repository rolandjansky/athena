/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p1.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p1.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"
#include "TrkEventTPCnv/TrkDetElementSurface/DetElementSurfaceCnv_p1.h"



void MaterialEffectsBaseCnv_p1::persToTrans(
   const Trk::MaterialEffectsBase_p1 *persObj,
   Trk::MaterialEffectsBase          *transObj,
   MsgStream& log)
{
   transObj->m_tInX0              = persObj->m_tInX0;
   
   ITPConverterFor<Trk::Surface>* surfaceCnv=0;
   const Trk::Surface* temp = this->createTransFromPStore( &surfaceCnv, persObj->m_associatedSurface, log );
   
   DetElementSurfaceCnv_p1* detElCnv = dynamic_cast<DetElementSurfaceCnv_p1*>(surfaceCnv); 
   if (detElCnv) {
       // have a DetElementSurface
       temp = detElCnv->createSurface(log);
   }   
   transObj->m_associatedSurface = temp;
   
   // 
   // ITPConverterFor<Trk::Surface> *associatedSurfaceCnv(0);
   // transObj->m_associatedSurface =
   //      createTransFromPStore( &associatedSurfaceCnv, persObj->m_associatedSurface, log);
   // FIXME check that TrackCollectionCnv can make diff between
   // private and detStore surfaces
   transObj->m_typeFlags          = persObj->m_typeFlags;
}

void MaterialEffectsBaseCnv_p1 :: transToPers(
   const Trk::MaterialEffectsBase    *transObj,
   Trk :: MaterialEffectsBase_p1 *persObj,
   MsgStream& log) 
{
    persObj->m_tInX0              = (float)transObj->thicknessInX0();
   // persObj->m_associatedSurface  = toPersistent( (ITPConverterFor<Trk::Surface>**)0, transObj->m_associatedSurface, log );
    persObj->m_typeFlags          = (short)transObj->m_typeFlags;
   
   if (transObj->associatedSurface().isFree() ) // if this is a free surface, write it out 'as is'
      persObj->m_associatedSurface = toPersistent((ITPConverterFor<Trk::Surface>**)0, &transObj->associatedSurface(), log);
   else { // else, make it into a DetElementSurface, to allow the correct convertor to be called
       Trk::DetElementSurface surf( transObj->associatedSurface() );
       persObj->m_associatedSurface = toPersistent((ITPConverterFor<Trk::Surface>**)0, &surf, log );
   }
}
