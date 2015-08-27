/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p2.cxx
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkParameters/TrackParameters.h"
#undef private

#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p2.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"

//#include "TrkEventTPCnv/TrkParameters/TrackParametersCnv_p1.h"


void TrackStateOnSurfaceCnv_p2::
persToTrans( const Trk::TrackStateOnSurface_p2 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
   //std::cout << "AW DEBUG TSoSCnv_p2::persToTrans() !!" << std::endl;

   ITPConverterFor<Trk::TrackParameters>	*paramsCnv = 0;
   transObj->m_trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &paramsCnv, persObj->m_trackParameters, log ));

   transObj->m_fitQualityOnSurface = createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log );

   ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = 0;
   transObj->m_materialEffectsOnTrack = createTransFromPStore( &matBaseCnv, persObj->m_materialEffects, log );

   ITPConverterFor<Trk::MeasurementBase>	*measureCnv = 0;
   //log << MSG::INFO << " ->>->  persObj->m_measurementOnTrack=(" << persObj->m_measurementOnTrack.m_typeID << "," << persObj->m_measurementOnTrack.m_index << ")" << endreq;
   transObj->m_measurementOnTrack =  createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );
   //log << MSG::INFO << " ->>->  transObj->m_measurementOnTrack=" << (void*)(transObj->m_measurementOnTrack) << endreq;
   transObj->m_typeFlags = persObj->m_typeFlags;
}


void TrackStateOnSurfaceCnv_p2::
transToPers( const Trk::TrackStateOnSurface *, Trk::TrackStateOnSurface_p2 *, MsgStream & ) 
{
  throw std::runtime_error("TrackStateOnSurfaceCnv_p2::transToPers is deprecated!");   
}
