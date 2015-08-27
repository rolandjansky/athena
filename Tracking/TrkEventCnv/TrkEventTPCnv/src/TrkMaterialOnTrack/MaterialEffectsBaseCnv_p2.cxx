/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#undef private

#include "TrkEventTPCnv/TrkMaterialOnTrack/MaterialEffectsBaseCnv_p2.h"
#include "TrkEventTPCnv/TrkSurfaces/SurfaceCnv_p2.h"
#include "TrkSurfaces/Surface.h"
#include "TrkEventCnvTools/DetElementSurface.h"

void MaterialEffectsBaseCnv_p2::persToTrans(
  const Trk::MaterialEffectsBase_p1 *persObj,
  Trk::MaterialEffectsBase          *transObj,
  MsgStream& log)
{
  transObj->m_tInX0              = persObj->m_tInX0;
  transObj->m_associatedSurface = this->createTransFromPStore( (ITPConverterFor<Trk::Surface>**)0, persObj->m_associatedSurface, log );
  transObj->m_typeFlags          = persObj->m_typeFlags;
}

void MaterialEffectsBaseCnv_p2 :: transToPers(
  const Trk::MaterialEffectsBase *transObj,
  Trk :: MaterialEffectsBase_p1  *persObj,
  MsgStream& log) 
{
  persObj->m_tInX0              = (float)transObj->m_tInX0;
  persObj->m_associatedSurface  = toPersistent( (ITPConverterFor<Trk::Surface>**)0, transObj->m_associatedSurface, log );
  persObj->m_typeFlags          = (short)transObj->m_typeFlags;
}
