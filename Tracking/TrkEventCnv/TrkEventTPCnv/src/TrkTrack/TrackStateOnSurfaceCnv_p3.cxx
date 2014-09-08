/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackStateOnSurfaceCnv_p3.cxx
//
//-----------------------------------------------------------------------------

#define private public
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkParameters/TrackParameters.h"
#undef private

#include "TrkEventTPCnv/TrkTrack/TrackStateOnSurfaceCnv_p3.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkMaterialOnTrack/MaterialEffectsBase.h"

void TrackStateOnSurfaceCnv_p3::
persToTrans( const Trk::TrackStateOnSurface_p3 *persObj, Trk::TrackStateOnSurface *transObj, MsgStream &log )
{
   //std::cout << "AW DEBUG TSoSCnv_p2::persToTrans() !!" << std::endl;

  //--- Parameters
  ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
  if (!m_parametersCnv)  m_parametersCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); // FIXME - only in init?
  transObj->m_trackParameters = dynamic_cast<const Trk::TrackParameters*>(createTransFromPStore( &m_parametersCnv, persObj->m_trackParameters, log ));
  transObj->m_fitQualityOnSurface = createTransFromPStore( &m_fitQCnv, persObj->m_fitQualityOnSurface, log );
  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = 0;  
  transObj->m_materialEffectsOnTrack = createTransFromPStore( &matBaseCnv, persObj->m_materialEffects, log );

  ITPConverterFor<Trk::MeasurementBase>	*measureCnv = 0;
  //log << MSG::INFO << " ->>->  persObj->m_measurementOnTrack=(" << persObj->m_measurementOnTrack.m_typeID << "," << persObj->m_measurementOnTrack.m_index << ")" << endreq;
  transObj->m_measurementOnTrack =  createTransFromPStore( &measureCnv, persObj->m_measurementOnTrack, log );
  //log << MSG::INFO << " ->>->  transObj->m_measurementOnTrack=" << (void*)(transObj->m_measurementOnTrack) << endreq;
  transObj->m_typeFlags = persObj->m_typeFlags;
}


void TrackStateOnSurfaceCnv_p3::
transToPers( const Trk::TrackStateOnSurface *transObj, Trk::TrackStateOnSurface_p3 *persObj, MsgStream &log ) 
{
  //--- Parameters
  ITPConverter* dummy = topConverter ()->converterForType( typeid(Trk::TrackParameters));    
  if (!m_parametersCnv)  m_parametersCnv = dynamic_cast<TrackParametersCnv_p2*>(dummy); // FIXME - only in init?
  persObj->m_trackParameters = toPersistent( &m_parametersCnv, transObj->m_trackParameters, log );

  persObj->m_fitQualityOnSurface = toPersistent( &m_fitQCnv, transObj->m_fitQualityOnSurface, log );

  ITPConverterFor<Trk::MeasurementBase>  *measureCnv = 0;
  persObj->m_measurementOnTrack = toPersistent( &measureCnv, transObj->m_measurementOnTrack, log );

  ITPConverterFor<Trk::MaterialEffectsBase> *matBaseCnv = 0;  
  persObj->m_materialEffects = toPersistent( &matBaseCnv,
                                            transObj->m_materialEffectsOnTrack, log );
  persObj->m_typeFlags = transObj->m_typeFlags.to_ulong();
}
